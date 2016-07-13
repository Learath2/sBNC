#include <stdio.h>

#include "core.h"
#include "srv.h"
#include "clt.h"
#include "proc.h"

#define LISTENER 0
#define SERVER 1
#define POLLTIMEOUT 1 * 60 * 1000

int main(int argc, char **argv)
{
	int nfds = 0, s_server = 0, s_listener = 0;
	struct pollfd fds[MAX_SOCKETS];

	process_args(argc, &argv);

	s_server = srv_init();
	srv_connect("irc.quakenet.org:6667")

	s_listener = clt_init();

	fds[LISTENER].fd 		= s_listener;
	fds[LISTENER].events 	= POLLIN;

	fds[SERVER].fd 			= s_server;
	fds[SERVER].events		= POLLIN;

	while(running){
		int res = poll(fds, nfds, POLLTIMEOUT);
		if(res < 0){ //Poll failed
			running = false;
			break;
		}
		else if(res != 0) { //Poll didn't timeout
			int csize = nfds;
			for(int i = 0; i < csize; i++){
				if(!fds[i].revents)
					continue;
				else if(i == LISTENER){
					int new = 0;
					while((new = clt_accept()) > 0){ //TODO: Check if fds is full
						fds[nfds].fd 		= new;
						fds[nfds++].events 	= POLLIN;
					}
				}
				else{
					char buffer[513];
					int len = 0;
					if((len = recv(fds[i].fd, buffer, sizeof buffer, 0)) < 0)
						break;
					proc_read(fds[i].fd, buffer, sizeof buffer, len);
				}
			}
		}

		proc_tick();

		while(proc_wqueue_length()){
			wqueue_entry_t *ent = proc_wqueue_head();
			write(ENT_GET(ent, target), ENT_GET(ent, data), ENT_GET(ent, datasz));
			proc_wqueue_next();
		}
	}
}