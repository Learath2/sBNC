#include "clt.h"

#define BACKLOG 10

int g_socket;
clt_state_t g_state = -1;

clt_state_t clt_state()
{
	return g_state;
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
	return g_socket;
}

int clt_accept()
{
	return accept(g_socket, NULL, NULL);
}

void clt_process_msg(int fd, int *syn, char **tokarr, int ntok)
{
	if()
}