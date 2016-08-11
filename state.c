#define MODULE_NAME "state"

#include "state.h"

#define MAX_CHANNELS 20
#define MAX_005LINES 5

char g_sumodes[32];
char g_scmodes[32];
char g_005[MAX_005LINES][513];
int g_n005 = 0;

char g_nick[10];
char g_channels[MAX_CHANNELS][200] = {0};
int g_nchannels = 0;

bool state_nick_set(char *nick)
{
	if(strlen(nick) > 9)
		return false;
	strcpy(g_nick, nick);
	return true;
}

const char *state_nick() { return g_nick; }
const char *state_server_umodes() { return g_sumodes; }
const char *state_server_cmodes() { return g_scmodes; }

void state_server_umodes_set(char *s)
{
	util_strncpy(g_sumodes, s, sizeof g_sumodes);
}

void state_server_cmodes_set(char *s)
{
	util_strncpy(g_scmodes, s, sizeof g_scmodes);
}

void state_server_005_store(char *s)
{
	if(g_n005 == MAX_005LINES) //IDK wat do
		return;

	strcpy(g_005[g_n005++], s); //Probably need to modify this a bit
}

void state_server_005(int id)
{
	for(int i = 0; i < g_n005; i++)
		clt_message_send(g_005[i], strlen(g_005[i]));
}

void state_channel_join(char *chan)
{
	for(int i = 0; i < MAX_CHANNELS; i++)
		if(g_channels[i][0] == '\0')
			strcpy(g_channels[i], chan);
	g_nchannels++;
}

int state_channel_joined(char *chan)
{
	for(int i = 0; i < MAX_CHANNELS; i++)
		if(!strcmp(g_channels[i], char))
			return i;
	return -1;
}

void state_channel_part(char *chan)
{
	g_channels[state_channel_joined(chan)][0] = '\0';
	g_nchannels--;
}

void state_channel_client_init(int id)
{
	for(int i = 0; i < MAX_CHANNELS; i++){
		if(g_channels[i][0] != '\0'){
			clt_message_sendf(id, ":%s!%s@%s JOIN %s", g_nick, g_user, core_host(), g_channels[i]);
			srv_message_sendf("NAMES %s", g_channels[i]); //Need to route the replies
		}
	}
}

#undef MODULE_NAME
