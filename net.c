#define MODULE_NAME "net"

#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include "util.h"
#include "clt.h"
#include "net.h"
#include "log.h"

#define MAX_SOCKETS 25
#define BUFSIZE 513

static struct pollfd g_fds[MAX_SOCKETS];
static int g_nfds = 0;
static int gs_listener = 0;
static char *g_buf[MAX_SOCKETS];

int net_nfds(void) { return g_nfds; }

int net_init(void)
{
	INF("Initializing...");

	memset(g_fds, 0, sizeof g_fds);
	return 0;
}

int net_poll_add_listener(int fd, short events)
{
	g_fds[g_nfds].fd = fd;
	g_fds[g_nfds].events = events;
	gs_listener = g_nfds++;

	return 0;
}

int net_poll_add(int fd, short events)
{
	g_buf[g_nfds] = malloc(sizeof *g_buf[0] * BUFSIZE); //Free this some time
	if(!g_buf[g_nfds])
		return -1;
	g_buf[g_nfds][0] = '\0';

	g_fds[g_nfds].fd = fd;
	g_fds[g_nfds].events = events;

	return g_nfds++;
}

int net_poll_remove(int id)
{
	int fd = net_id2fd(id);
	memmove(&g_fds[id], &g_fds[id+1], (g_nfds - id -1) * sizeof (struct pollfd));
	g_nfds--;
	return close(fd);
}

int net_poll_remove_fd(int fd)
{
	return net_poll_remove(net_fd2id(fd));
}

int net_poll(int timeout)
{
	int r = poll(g_fds, g_nfds, timeout);
	if(r < 0){ //Poll failed
		ERR("poll() failed. Exiting...");
		return -1;
	}
	else if(r != 0) { //Poll didn't timeout
		int csize = g_nfds;
		for(int i = 0; i < csize; i++){
			if(!g_fds[i].revents)
				continue;
			else if(i == gs_listener){
				while(true)
					if(clt_accept() < 0)
						break;
			}
			else{
				net_socket_read(i);
				if(errno != EWOULDBLOCK && errno != EAGAIN)
					break;
			}
		}
	}
	return 0;
}

int net_socket_create(void)
{
	int s = socket(AF_INET, SOCK_STREAM, 0);
	int re = 1;
	if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &re, sizeof re) < 0){
		ERR("setsockopt() failed exiting...");
		return -1;
	}

	if(fcntl(s, F_SETFL, fcntl(s, F_GETFL, 0) | O_NONBLOCK) < 0){
		ERR("fcntl() failed exiting...");
		return -1;
	}
	return s;
}

int net_fd2id(int fd)
{
	for(int i = 0; i < g_nfds; i++)
		if(g_fds[i].fd == fd)
			return i;

	return -1;
}

int net_id2fd(int id)
{
	return g_fds[id].fd;
}

int net_socket_read(int id)
{
	int len = strlen(g_buf[id]);

	int r = read(g_fds[id].fd, g_buf[id] + len, BUFSIZE - len);
	g_buf[id][len + r] = '\0';
	return r;
}

int net_socket_read_fd(int fd)
{
	return net_socket_read(net_fd2id(fd));
}

int net_socket_write(int fd, void *data, size_t sz)
{
	return write(fd, data, sz);
}

bool net_socket_avail(int id)
{
	if(id == gs_listener)
		return false;

	if(strchr(g_buf[id], '\n'))
		return true;

	return false;
}

bool net_socket_avail_fd(int fd)
{
	return net_socket_avail(net_fd2id(fd));
}

void net_socket_msg(int id, char *buf, size_t sz)
{
	char *a = buf, *b = g_buf[id], *c = strchr(b, '\n') + 1;
	while(sz-- && b < c)
		*a++ = *b++;

	*a = '\0';
	buf[strcspn(buf, "\r\n")] = '\0';

	memmove(g_buf[id], c, strlen(g_buf[id]) - (c - g_buf[id]) + 1);
}

void net_socket_msg_fd(int fd, char *buf, size_t sz)
{
	net_socket_msg(net_fd2id(fd), buf, sz);
}

#undef MODULE_NAME
