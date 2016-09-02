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


	g_sett.server.host[0] = '\0';
	g_sett.server.port = 0;
	g_sett.server.pass[0] = '\0';
	g_sett.server.ssl = false;

	#undef SSET
}

void sett_dump(void)
{
	DBGF("password = %s", g_sett.pass);
	DBGF("heartbeat = %d", g_sett.hbeat);
	DBGF("quit message = %s", g_sett.qmsg);
	DBGF("nickname = %s", g_sett.nick);
	DBGF("username = %s", g_sett.uname);
	DBGF("realname = %s", g_sett.rname);
	DBGF("host = %s", g_sett.host);
	DBGF("port = %s", g_sett.port);
	DBG("store:");
	DBGF("\tpath = %s", g_sett.spath);
	DBGF("\tformat = %s", g_sett.sfmt);
	DBGF("\tjoin = %s", g_sett.sjoin ? "true", "false");
	DBG("server:");
	DBGF("\thost = %s", g_sett.server.host);
	DBGF("\tport = %d", g_sett.server.port);
	DBGF("\tpass = %s", g_sett.server.pass);
	DBGF("\tssl = %s", g_sett.server.ssl ? "true", "false");
}

#undef MODULE_NAME
