#include "srv.h"

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

int srv_connect(char *server)
{
	//TODO: turn host:port into int port, char *host
	struct sockaddr_in srv_addr;

	memset(srv_addr, 0, sizeof srv_addr);
	srv_addr.sin_family		= AF_INET;
	srv_addr.sin_port		= htons(port);
	inet_pton(AF_INET, host, &srv_addr.sin_addr);
	
	return (g_state = connect(g_socket, (struct sockaddr *)&srv_addr, sizeof srv_addr));
}

srv_state_t srv_state(){ return g_state; }
int srv_socket(){ return g_socket; }

void srv_message_process(char *buf)
{
	char *tmp = util_strdup(buf);
	char *tokarr[MAX_TOKENS] = {0};
	int ntok = util_tokenize(tmp, tokarr, COUNT_OF(tokarr));
	struct irc_message m = util_irc_message_parse(tokarr, ntok);
	bool me = !strcmp(m.prefix.nick, state_nick());

	if(me && !strcmp(m.tokarr[m.cmd], "JOIN")){
		for(int i = m.middle; i < m.ntok; i++){
			if(m.tokarr[i][0] == '#' || m.tokarr[i][0] == '&')
				state_channel_join(tokarr[i]);
			else
				break;
		}
	}
	else if(me && !strcmp(m.tokarr[m.cmd], "PART")){
		for(int i = m.middle; i < m.ntok; i++)
			if(m.tokarr[i][0] == '#' || m.tokarr[i][0] == '&')
				state_channel_part(tokarr[i]);
	}
	else if(me && !strcmp(m.tokarr[m.cmd], "004")){
		state_server_umodes_set(m.tokarr[m.middle + 3]);
		state_server_cmodes_set(m.tokarr[m.middle + 4]);
	}
	else if(me && !strcmp(m.tokarr[m.cmd], "005")){
		state_server_005_store(buf);
	}

	clt_send_message(-1, msg, strlen(buf));
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