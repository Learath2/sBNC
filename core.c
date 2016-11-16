#define MODULE_NAME "core"

#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "util.h"
#include "core.h"
#include "net.h"
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
	int s_server = 0, s_listener = 0;
	bool running = true;

	struct pollfd fds[MAX_SOCKETS];

	store_init();

	if((s_server = srv_init()) < 0)
		return EXIT_FAILURE;

	if(srv_connect() == -1){
		ERR("srv_connect() failed. Exiting...");
		return EXIT_FAILURE;
	}

	if((s_listener = clt_init()) == -1){
		ERR("clt_init() failed. Exiting...");
		return EXIT_FAILURE;
	}

	memset(fds, 0 , sizeof(fds));

	net_init();
	net_poll_add(s_server, POLLIN);
	net_poll_add_listener(s_listener, POLLIN);

	while(running){
		net_poll(POLLTIMEOUT);
		proc_tick();

		for(int i = 0; i < net_nfds(); i++){
			while(net_socket_avail(i)){
				char msg[513] = "";
				net_socket_msg(i, msg, sizeof msg);
				proc_proc(net_id2fd(i), msg);
			}
		}

		while(proc_wqueue_length()){
			wqueue_entry_t ent = proc_wqueue_head();
			net_socket_write(ENT_GET(ent, target), ENT_GET(ent, data), ENT_GET(ent, datasz));
			proc_wqueue_next();
		}
	}

	return EXIT_SUCCESS;
}

#undef MODULE_NAME
