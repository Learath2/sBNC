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

void srv_process_message(int *syn, char **tokarr, int ntok)
{

}