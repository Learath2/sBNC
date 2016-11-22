#define MODULE_NAME "clt"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <fcntl.h>

#include "util.h"
#include "clt.h"
#include "net.h"
#include "sett.h"
#include "srv.h"
#include "proc.h"
#include "state.h"
#include "store.h"
#include "log.h"

#define BACKLOG 10
#define MAX_CLIENTS 10

static int g_socket = -1;
static int g_nclients = 0;
static struct client g_clients[MAX_CLIENTS];

int clt_clients_nid2cid(int nid)
{
	for(int i = 0; i < MAX_CLIENTS; i++)
		if(g_clients[i].nid == nid)
			return i;
	return -1;
}

int clt_clients_add(int nid)
{
	if(nid < 0)
		return nid;

	for(int i = 0; i < MAX_CLIENTS; i++){
		if(g_clients[i].nid == -1){
			g_clients[i].nid = nid;
			g_clients[i].state = CLIENT_STATE_REGISTER;
			g_clients[i].needs_playback = (g_nclients++ == 0);
			break;
		}
	}
	return nid;
}

void clt_clients_remove_cid(int cid)
{
	g_clients[cid].nid = -1;
	g_clients[cid].state = CLIENT_STATE_EMPTY;
	if(g_nclients-- == 1){
		INF("no clients left marking away.");
		state_mark_away();
	}

	net_poll_remove(g_clients[cid].nid);
}

void clt_clients_remove_nid(int nid)
{
	clt_clients_remove_cid(clt_clients_nid2cid(nid));
}

//TODO: Check for errors :P
int clt_init(void)
{
	INF("Initializing...");

	struct settings *s = sett_get();

	struct sockaddr_in clt_addr;

	g_socket = socket(AF_INET, SOCK_STREAM, 0);

	if(fcntl(g_socket, F_SETFL, fcntl(g_socket, F_GETFL, 0) | O_NONBLOCK) < 0){
		ERR("fcntl() failed exiting...");
		return -1;
	}

	memset(&clt_addr, 0, sizeof clt_addr);
	clt_addr.sin_family			= AF_INET;
	clt_addr.sin_addr.s_addr	= htonl(INADDR_ANY);
	clt_addr.sin_port			= htons(s->port);

	if(bind(g_socket, (struct sockaddr *)&clt_addr, sizeof clt_addr))
		return -1;

	listen(g_socket, BACKLOG);

	for(int i = 0; i < MAX_CLIENTS; i++){
		g_clients[i].nid = -1;
		g_clients[i].state = CLIENT_STATE_EMPTY;
		g_clients[i].lastact = 0;
		g_clients[i].lastpong = 0;
		g_clients[i].pingsent = 0;
		g_clients[i].regping[0] = '\0';
	}
	return g_socket;
}

void clt_tick(void)
{
	#define SF(FMT, ...) clt_message_sendf(i, FMT, __VA_ARGS__)
	#define SN(DATA, SZ) clt_message_send(i, DATA, SZ)

	struct settings *s = sett_get();

	for(int i = 0; i < MAX_CLIENTS; i++){
		switch (g_clients[i].state){
			case CLIENT_STATE_EMPTY:
				continue;
			case CLIENT_STATE_REGISTER:
				break;
			case CLIENT_STATE_REGISTERED:
				break;
			case CLIENT_STATE_INIT:
				if(g_clients[i].regping[0])
					break;
				SF(":%s 001 %s :Welcome to sBNC, %s", s->host, state_nick(), state_nick());
				SF(":%s 002 %s :Your host is %s, running version "VERSION, s->host, state_nick(), s->host);
				SF(":%s 003 %s :This server was created %s", s->host, state_nick(), s->epoch);
				SF(":%s 004 %s :%s %s sBNC-"VERSION" %s", s->host, state_nick(), s->host, state_server_umodes(), state_server_cmodes());
				state_server_005(i);
				srv_message_sendf("LUSERS"); //Need to route the replies somehow
				srv_message_sendf("MOTD");   //ditto
				state_channel_client_init(i);
				g_clients[i].lastpong = time(NULL);
				g_clients[i].state = CLIENT_STATE_READY;
				break;
			case CLIENT_STATE_READY:
				if(time(NULL) - g_clients[i].lastpong > s->hbeat){
					SF(":%s PING :%s", s->host, s->host);
					g_clients[i].pingsent = time(NULL);
				}

				if(g_clients[i].lastpong < g_clients[i].pingsent && time(NULL) - g_clients[i].pingsent > 30)
					clt_clients_remove_cid(i);

				if(g_clients[i].needs_playback){
					state_buffer_play(i);
					g_clients[i].needs_playback = false;
				}
				break;
		}
	}

	#undef SF
	#undef SN
}

int clt_accept(void)
{
	if(g_nclients == MAX_CLIENTS)
		return -2;
	int new = accept(g_socket, NULL, NULL);
	if(new < 0)
		return -1;
	else{
		clt_clients_add(net_poll_add(new, POLLIN));
		return new;
	}
}

void clt_message_process(int nid, char *buf)
{
	int cid = clt_clients_nid2cid(nid);
	struct client *c = &g_clients[cid];

	INFF("<-{%d}%s", cid, buf);

	c->lastact = time(NULL);

	struct settings *s = sett_get();

	char *tmp = util_strdup(buf);
	struct irc_message m = util_irc_message_parse(tmp);

	if(s->pass[0] != '\0' && !strcmp(m.tokarr[m.cmd], "PASS")){
		if(c->state < CLIENT_STATE_REGISTERED)
			c->auth = !strcmp(m.tokarr[m.middle], s->pass);
		else
			clt_message_sendf(cid, ":%s 462 %s :Connection already registered", s->host, state_nick());
		return;
	}
	else if(!strcmp(m.tokarr[m.cmd], "NICK")){
		c->state = CLIENT_STATE_REGISTERED;
		util_randstr(c->regping, sizeof c->regping - 1, "0123456789");
		clt_message_sendf(cid, "PING :%s", c->regping);
		return;
	}
	else if(!strcmp(m.tokarr[m.cmd], "USER")){
		if(c->state < CLIENT_STATE_INIT){
			if(s->pass[0] != '\0' && !c->auth){
				clt_message_sendf(cid, ":%s 464 %s :Password incorrect", s->host, state_nick());
				clt_clients_remove_cid(cid);
			}
			else{
				c->username = util_strdup(m.tokarr[m.middle]);
				c->state = CLIENT_STATE_INIT;
			}
		}
		return;
	}
	else if(!strcmp(m.tokarr[m.cmd], "PONG")){
		if(c->state <= CLIENT_STATE_INIT){
			if(!strcmp(m.tokarr[m.trailing], c->regping))
				c->regping[0] = '\0';
			else
				clt_message_sendf(cid, "Send \"PONG :%s\" to register", c->regping);
		}
		c->lastpong = time(NULL);
		return;
	}
	else if(!strcmp(m.tokarr[m.cmd], "QUIT")){
		INFF("client disconnected. cid=%d", cid);
		clt_clients_remove_cid(cid);
		return;
	}

	srv_message_send(buf);
}

void clt_message_sendf(int id, const char *format, ... )
{
	char buf[513];
	va_list args;
	va_start(args, format);
	int n = vsnprintf(buf, sizeof buf, format, args);
	va_end(args);

	if(n < sizeof buf)
		clt_message_send(id, buf);
}

void clt_message_send(int cid, const char *data)
{
	INFF("->{%d}%s", cid, (char *)data);

	char buf[513];
	util_strncpy(buf, data, sizeof buf);
	strcat(buf, "\r\n"); //GET util_strncat which checks for bounds

	if(cid > 0)
		proc_wqueue_add(net_id2fd(g_clients[cid].nid), buf);
	else
		for(int i = 0; i < MAX_CLIENTS; i++)
			if(g_clients[i].nid != -1)
				proc_wqueue_add(net_id2fd(g_clients[i].nid), buf);
}

#undef MODULE_NAME
