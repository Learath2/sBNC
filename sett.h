struct server{
	char *host;
	int port;
	char *pass;
};

struct settings{
	char epoch[64];
	char pass[64];
	int hbeat;
	char qmsg[512];
	int verb;

	char nick[64];
	char uname[64];
	char rname[128];
	struct server server;
};

struct settings *sett_get(void);
