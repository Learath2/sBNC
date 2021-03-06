#ifndef SETT_H_
#define SETT_H_

#include <time.h>

struct server{
	char host[128];
	int port;
	char pass[64];
	bool ssl;
};

struct settings{
	time_t rawepoch;
	char epoch[64];
	char pass[64];
	int hbeat;
	char qmsg[512];
	int verb;
	char spath[256];
	char sfmt[128];
	bool sjoin;
	char host[64];
	int port;

	char nick[64];
	char uname[64];
	char rname[128];
	struct server server;
};

struct settings *sett_get(void);
void sett_init(void);
void sett_dump(void);

#endif
