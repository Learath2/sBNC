#define MODULE_NAME "srv"

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdarg.h>

#include "util.h"
#include "srv.h"
#include "sett.h"
#include "log.h"

int g_socket = 0;
srv_state_t g_state = SRV_STATE_OFFLINE;

int srv_init()
{
	int re = 1;

	g_socket = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(g_socket, SOL_SOCKET, SO_REUSEADDR, &re, sizeof re);
	ioctl(g_socket, FIONBIO, &re);

	return g_socket;
}

int srv_connect()
{
	struct settings *s = sett_get();
	struct sockaddr_in srv_addr;

	memset(srv_addr, 0, sizeof srv_addr);
	srv_addr.sin_family		= AF_INET;
	srv_addr.sin_port		= htons(s->server.port);
	getaddrinfo();//TODO:Finish this up
	inet_pton(AF_INET, host, &srv_addr.sin_addr);
	if(connect(g_socket, (struct sockaddr *)&srv_addr, sizeof srv_addr))
		return -1;

	

	srv_message_sendf("PASS %s", s->server.pass);
	srv_message_sendf("NICK %s", s->nick);
	srv_message_sendf("USER %s 8 * :%s", s->uname, s->rname);

	return 0;
}

srv_state_t srv_state(){ return g_state; }
int srv_socket(){ return g_socket; }

void srv_message_process(char *buf)
{
	INF(buf);

	char *tmp = util_strdup(buf);
	struct irc_message m = util_irc_message_parse(tmp);
	bool me = !strcmp(m.prefix.nick, state_nick());

	if(!strcmp(m.tokarr[m.cmd], "PING")){
		srv_message_sendf("PONG :%s", m.tokarr[m.trailing] + 1);
		return;
	}
	else if(me && !strcmp(m.tokarr[m.cmd], "JOIN")){
		for(int i = m.middle; i < m.ntok; i++){
			if(m.tokarr[i][0] == '#' || m.tokarr[i][0] == '&')
				state_channel_join(tokarr[i]);
			else
				break;
		}
		return;
	}
	else if(me && !strcmp(m.tokarr[m.cmd], "PART")){
		for(int i = m.middle; i < m.ntok; i++)
			if(m.tokarr[i][0] == '#' || m.tokarr[i][0] == '&')
				state_channel_part(tokarr[i]);
		return;
	}
	else if(me && !strcmp(m.tokarr[m.cmd], "004")){
		state_server_umodes_set(m.tokarr[m.middle + 3]);
		state_server_cmodes_set(m.tokarr[m.middle + 4]);
		return;
	}
	else if(me && !strcmp(m.tokarr[m.cmd], "005")){
		state_server_005_store(buf);
		return;
	}
	else if(me && !strcmp(m.tokarr[m.cmd], "NICK")){
		state_nick_set(m.tokarr[m.middle]);
		return;
	}
	free(tmp);

	clt_send_message(-1, buf, strlen(buf));
}

void srv_message_sendf(const char *format, ... )
{
	char buf[513];
	va_list args;
	va_start(args, format);
	int n = vsnprintf(buf, sizeof buf, format, args);
	va_end(args);

	if(n > 0 && n < buf)
		srv_message_send(buf, n);
}

void srv_message_send(void *data, size_t datasz)
{
	proc_wqueue_add(g_socket, data, datasz);
}

#undef
