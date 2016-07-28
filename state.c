#include "state.h"

#define MAX_CHANNELS 20

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

const char *state_nick_get() { return g_nick; }

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

void state_channel_new_client(int id)
{
	for(int i = 0; i < MAX_CHANNELS; i++){
		if(g_channels[i][0] != '\0'){
			clt_message_sendf(id, ":%s!%s@%s JOIN %s", g_nick, g_user, core_host(), g_channels[i]);
			srv_message_sendf("NAMES %s", g_channels[i]); //Need to route the replies
		}
	}
}