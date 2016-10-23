#define MODULE_NAME "init"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "util.h"
#include "core.h"
#include "sett.h"
#include "log.h"

void usage(FILE *f, const char *a0, int ec, bool sh)
{
	#define O(STR) fputs(STR "\n", f);
	#define F(STR, ...) fprintf(f, STR "\n", __VA_ARGS__)

		O("sBNC-"VERSION"- simple irc bouncer");
		F("usage: %s [-hHVirfjvlLtTpNURPQ] <hostspec>", a0);
	if(!sh){
		O("\t-h: Display usage statement.");
		O("\t-H: Display this statement.");
		O("\t-V: Display version.");
		O("\t-i <str>: Set host to <str>.");
		O("\t-r <str>: Set store log path.");
		O("\t-f <str>: Set store filename format. (%E(poch) %D(ay)/%M(onth)/%Y(ear) %h(ost)#%c(han))");
		O("\t-j: Log join and part messages.");
		O("\t-v <int>: Set console verbosity to <int>. (0:silent,1:error,2:warn,3:info.4:debug)");
		O("\t-l <str>: Set the of the logfile to <str>.");
		O("\t-L <int>: Set logfile verbosity to <int>. (0:silent,1:error,2:warn,3:info.4:debug)");
		O("\t-t <int>: Send heartbeat pings every <int> seconds.");
		O("\t-T <int>: Bouncer port.");
		O("\t-p <str>: Use <str> as bouncer password.");
		O("\t-N <str>: Use <str> as nick.");
		O("\t-U <str>: Use <str> as username.");
		O("\t-R <str>: Use <str> as realname.");
		O("\t-P <str>: Use <str> as server password.");
		O("\t-Q <str>: Use <str> as quit message.");
		O("\t<hostspec>: In [!]<host:port> format. ! denotes ssl.");
	}

	#undef O
	#undef F
	exit(ec);
}

void process_args(int *argc, char ***argv)
{
	char *a0 = (*argv)[0];
	struct settings *s = sett_get();

	#define CSET(STR) util_strncpy(STR, optarg, sizeof STR)

	int c;
	while((c = getopt(*argc, *argv, "hHVi:r:f:jv:l:L:t:T:p:N:U:R:P:Q:")) != -1){
		switch(c){
			case 'h':
				usage(stdout, a0, EXIT_SUCCESS, true);
				break;
			case 'H':
				usage(stdout, a0, EXIT_SUCCESS, false);
				break;
			case 'V':
				puts("sBNC-"VERSION);
				exit(EXIT_SUCCESS);
				break;
			case 'i': //Should probably set this with reverse DNS or /etc/hostname
				CSET(s->host); 
				break;
			case 'r':
				CSET(s->spath);
				break;
			case 'f':
				CSET(s->sfmt);
				break;
			case 'j':
				s->sjoin = true;
				break;
			case 'v':
				log_cverb_set(util_clamp(strtol(optarg, NULL, 10), LOGGER_SILENT, LOGGER_DEBUG));
				break;
			case 'l':
				log_file_set(optarg);
				break;
			case 'L':
				log_fverb_set(util_clamp(strtol(optarg, NULL, 10), LOGGER_SILENT, LOGGER_DEBUG));
				break;
			case 't':
				s->hbeat = strtol(optarg, NULL, 10);
				break;
			case 'T':
				s->port = strtol(optarg, NULL, 10);
				break;
			case 'p':
				CSET(s->pass);
				break;
			case 'N':
				CSET(s->nick);
				break;
			case 'U':
				CSET(s->uname);
				break;
			case 'R':
				CSET(s->rname);
				break;
			case 'P':
				CSET(s->server.pass);
				break;
			case 'Q':
				CSET(s->qmsg);
				break;
			case '?':
			default:
				usage(stderr, a0, EXIT_FAILURE, true);
		}
	}
	#undef CSET

	*argc -= optind;
	*argv += optind;

	if(!*argc){
		ERR("No hostspec given.");
		usage(stderr, a0, EXIT_FAILURE, true);
	}
	util_parse_hostspec(s->server.host, sizeof s->server.host, &s->server.port, &s->server.ssl, (*argv)[0]);
}

int main(int argc, char **argv)
{
	sett_init();
	process_args(&argc, &argv);
	ALL("Running sBNC-"VERSION);
	sett_dump();

	exit(core_run());
}

#undef MODULE_NAME
