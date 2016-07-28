#include "clt.h"

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
	}
	return g_socket;
}

int clt_accept()
{
	if(g_nclients == MAX_CLIENTS)
		return -2;
	return clt_clients_add(accept(g_socket, NULL, NULL));
}

void clt_message_process(int fd, char *buf)
{
	srv_send_msg(buf, strlen(buf));
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