#define MODULE_NAME "core"

#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include "util.h"
#include "core.h"
#include "srv.h"
#include "clt.h"
#include "proc.h"
#include "store.h"
#include "state.h"
#include "sett.h"
#include "log.h"

#define MAX_SOCKETS 25
#define LISTENER 0
#define SERVER 1
#define POLLTIMEOUT 1 * 2 * 1000

int core_run(void)
{
	int nfds = 0, s_server = 0, s_listener = 0;
	bool running = true;

	struct pollfd fds[MAX_SOCKETS];

	store_init();
	
	s_server = srv_init();
	if(srv_connect() == -1){
		ERR("srv_connect() failed. Exiting...");
		return EXIT_FAILURE;
	}

	if((s_listener = clt_init()) == -1){
		ERR("clt_init() failed. Exiting...");
		return EXIT_FAILURE;
	}

	memset(fds, 0 , sizeof(fds));

	fds[LISTENER].fd 		= s_listener;
	fds[LISTENER].events 	= POLLIN;

	fds[SERVER].fd 			= s_server;
	fds[SERVER].events		= POLLIN;

	nfds = 2;

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
			wqueue_entry_t ent = proc_wqueue_head();
			write(ENT_GET(ent, target), ENT_GET(ent, data), ENT_GET(ent, datasz));
			proc_wqueue_next();
		}
	}

	return EXIT_SUCCESS;
}

#undef MODULE_NAME
