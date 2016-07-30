#include "clt.h"
#include "core.h"

#define BACKLOG 10
#define MAX_CLIENTS 10

int g_socket = -1;
clt_state_t g_state = -1;
int g_nclients = 0;
struct client g_clients[MAX_CLIENTS];

clt_state_t clt_state() { return g_state; }

int clt_clients_get_id(int fd)
{
	for(int i = 0; i < MAX_CLIENTS; i++)
		if(g_clients[i].fd == fd)
			return i;
}

int clt_clients_add(int fd)
{
	for(int i = 0; i < MAX_CLIENTS; i++){
		if(g_clients[i].fd == -1){
			g_clients[i].fd = fd;
			g_clients[i].state = CLIENT_STATE_INIT;
			g_nclients++;
			break;
		}
	}
	return fd;
}

void clt_clients_remove_id(int id)
{
	g_clients[id].fd = -1;
	g_clients[id].state = CLIENT_STATE_EMPTY;
}

void clt_clients_remove_fd(int fd)
{
	clt_clients_remove_id(clt_clients_get_id(fd));
}

//TODO: Check for errors :P
int clt_init(int port)
{
	struct sockaddr_in clt_addr;
	int re = 1, rc = 0;

	g_socket = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(g_socket, SOL_SOCKET, SO_REUSEADDR, &re, sizeof re);
	ioctl(g_socket, FIONBIO, &re);

	memset(&clt_addr, 0, sizeof clt_addr);
	clt_addr.sin_family			= AF_INET;
	clt_addr.sin_addr.s_addr	= htonl(INADDR_ANY);
	clt_addr.sin_port			= htons(port);

	bind(g_socket, (struct sockaddr *)&clt_addr, sizeof clt_addr);

	listen(g_socket, BACKLOG);

	for(int i = 0; i < MAX_CLIENTS; i++){
		g_clients[i].fd = -1;
		g_clients[i].state = CLIENT_STATE_EMPTY;
		g_clients[i].lastping = 0;
	}
	return g_socket;
}

void clt_tick()
{
	#define SF(...) clt_message_sendf(i, __VA_VARGS__)
	#define SN(DATA, SZ) clt_message_send(i, DATA, SZ)

	for(int i = 0; i < MAX_CLIENTS; i++){
		switch g_clients[i].state{
			case CLIENT_STATE_EMPTY:
				continue;
			case CLIENT_STATE_REGISTER:
				break;
			case CLIENT STATE_INIT:
				SF(":%s 001 %s :Welcome to sBNC, %s", core_host(), state_nick(), state_nick());
				SF(":%s 002 %s :Your host is %s, running version %s", core_host(), state_nick(), core_host(), core_version());
				SF(":%s 003 %s :This server was created %s", core_host(), state_nick(), core_epoch());
				SF(":%s 004 %s :%s %s %s %s", core_host(), state_nick(), core_host(), core_version(), state_server_umodes(), state_server_cmodes());
				state_server_005(i);
				srv_message_sendf("LUSERS"); //Need to route the replies somehow
				srv_message_sendf("MOTD");   //ditto
				state_channel_client_init(i);
				g_clients[i].state = CLIENT_STATE_READY;
				break;
		}
	}

	#undef SF
	#undef SN
}

int clt_accept()
{
	if(g_nclients == MAX_CLIENTS)
		return -2;
	return clt_clients_add(accept(g_socket, NULL, NULL));
}

void clt_message_process(int fd, char *buf)
{
	int id = clt_clients_get_id(fd);
	char *tmp = util_strdup(buf);
	struct irc_message m = util_irc_message_parse(tmp);

	if(core_passwd() && !strcmp(m.tokarr[m.cmd], "PASS")){
		g_clients[id].auth = !strcmp(m.tokarr[m.middle], core_passwd());
		return;
	}
	else if(!strcmp(m.tokarr[m.cmd], "NICK"))
		return;
	else if(!strcmp(m.tokarr[m.cmd], "USER")){
		g_clients[id].username = util_strdup(m.tokarr[m.middle]);
		if(core_passwd() && !g_clients[id].auth){
			clt_message_sendf(id, ":%s 464 %s :Password incorrect");
			clt_disconnect(id);
		}
		else
			g_clients[id].state = CLIENT_STATE_INIT;
		return;
	}

	srv_send_msg(buf, strlen(buf));
}

void clt_message_sendf(int id, const char *format, ... )
{
	char buf[513];
	va_list args;
	va_start(args, format);
	int n = vsnprintf(buf, sizeof buf, format, args);
	va_end(args);

	if(n < buf)
		clt_message_send(id, buf, n);
}

void clt_message_send(int id, void *data, size_t datasz)
{
	if(fd > 0)
		proc_wqueue_add(g_clients[id].fd, data, datasz);
	else
		for(int i = 0; i < MAX_CLIENTS; i++)
			if(g_clients[i].fd != -1)
				proc_wqueue_add(g_clients[i].fd, data, datasz);
}
