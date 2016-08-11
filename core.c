#define MODULE_NAME "core"

#include <stdio.h>

#include "core.h"
#include "srv.h"
#include "clt.h"
#include "proc.h"
#include "state.h"
#include "log.h"

#define LISTENER 0
#define SERVER 1
#define POLLTIMEOUT 1 * 60 * 1000
#define VERSION "0.0.1"

int core_run(void)
{
	int nfds = 0, s_server = 0, s_listener = 0;
	struct pollfd fds[MAX_SOCKETS];
	struct settings *s = sett_get();
	
	s_server = srv_init();
	if(srv_connect(s->server) == -1){
		ERR("srv_connect() failed. Exiting...");
		return EXIT_FAILURE;
	}

	s_listener = clt_init();

	fds[LISTENER].fd 		= s_listener;
	fds[LISTENER].events 	= POLLIN;

	fds[SERVER].fd 			= s_server;
	fds[SERVER].events		= POLLIN;

	while(running){
		int res = poll(fds, nfds, POLLTIMEOUT);
		if(res < 0){ //Poll failed
			ERR("poll() failed. Exiting...");
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
					buffer[len+1] = '\0'; //Ensure NULL Termination
					proc_read(fds[i].fd, buffer);
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

	return EXIT_SUCCESS;
}

#undef MODULE_NAME
