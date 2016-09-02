#define MODULE_NAME "sett"

#include "util.h"
#include "sett.h"
#include "log.h"

struct settings g_sett;

struct settings *sett_get(void) { return &g_sett; }

void sett_init(void)
{
	#define SSET(S, STR) util_strncpy(S, STR, sizeof S)

	g_sett.rawepoch = time(NULL);
	struct tm *tm = localtime(&g_sett.rawepoch);
	strftime(g_sett.epoch, 64, "%a %b %d %Y at %H:%M:%S", tm);

	SSET(g_sett.pass, "");
	g_sett.hbeat = 60 * 3;
	SSET(g_sett.qmsg , "sBNC-"VERSION": Leaving");
	SSET(g_sett.nick , "sBNC");
	SSET(g_sett.uname , "sBNC");
	SSET(g_sett.rname , "Simple BNC");
	SSET(g_sett.host , "localhost");
	g_sett.port = 6667;
	SSET(g_sett.spath , "");
	SSET(g_sett.sfmt , "%D:%M:%Y_%h#%c");
	g_sett.sjoin = false;
	g_sett.server = {NULL, 0, NULL, false};

	#undef SSET
}

void sett_dump(void)
{
	DBG("password = %s", g_sett.pass);
	DBG("heartbeat = %d", g_sett.hbeat);
	DBG("quit message = %s", g_sett.qmsg);
	DBG("nickname = %s", g_sett.nick);
	DBG("username = %s", g_sett.uname);
	DBG("realname = %s", g_sett.rname);
	DBG("host = %s", g_sett.host);
	DBG("port = %s", g_sett.port);
	DBG("store:")
	DBG("\tpath = %s", g_sett.spath);
	DBG("\tformat = %s", g_sett.sfmt);
	DBG("\tjoin = %s", g_sett.sjoin ? "true", "false");
	DBG("server:");
	DBG("\thost = %s", g_sett.server.host);
	DBG("\tport = %d", g_sett.server.port);
	DBG("\tpass = %s", g_sett.server.pass);
	DBG("\tssl = %s", g_sett.server.ssl ? "true", "false");
}

#undef MODULE_NAME
