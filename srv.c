#define MODULE_NAME "srv"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "util.h"
#include "state.h"
#include "srv.h"
#include "clt.h"
#include "proc.h"
#include "sett.h"
#include "log.h"

static int g_socket = 0;
static srv_state_t g_state = SRV_STATE_OFFLINE;

int srv_init(void)
{
	int re = 1;

	g_socket = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(g_socket, SOL_SOCKET, SO_REUSEADDR, &re, sizeof re);
	ioctl(g_socket, FIONBIO, &re);

	return g_socket;
}

int srv_connect(void)
{
	struct settings *s = sett_get();
	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	char port[7];
	snprintf(port, sizeof port ,"%d", s->server.port);
	getaddrinfo(s->server.host, port, &hints, &res);

	if(connect(g_socket, res->ai_addr, res->ai_addrlen) && errno != EINPROGRESS)
		return -1;

	if(s->server.pass[0] != '\0')
		srv_message_sendf("PASS %s", s->server.pass);
	srv_message_sendf("NICK %s", s->nick);
	srv_message_sendf("USER %s 8 * :%s", s->uname, s->rname);

	return 0;
}

srv_state_t srv_state(void){ return g_state; }
int srv_socket(void){ return g_socket; }

void srv_message_process(char *buf)
{
	INFF("%s", buf);

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
				state_channel_join(m.tokarr[i]);
			else
				break;
		}
		return;
	}
	else if(me && !strcmp(m.tokarr[m.cmd], "PART")){
		for(int i = m.middle; i < m.ntok; i++)
			if(m.tokarr[i][0] == '#' || m.tokarr[i][0] == '&')
				state_channel_part(m.tokarr[i]);
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

	clt_message_send(-1, buf, strlen(buf));
}

void srv_message_sendf(const char *format, ... )
{
	char buf[513];
	va_list args;
	va_start(args, format);
	int n = vsnprintf(buf, sizeof buf, format, args);
	va_end(args);

	if(n > 0 && n < sizeof buf)
		srv_message_send(buf, n);
}

void srv_message_send(char *data, size_t datasz)
{
	char buf[513];
	snprintf(buf, sizeof buf, "%s\r\n", data);
	proc_wqueue_add(g_socket, buf, datasz + 2);
}

#undef MODULE_NAME

