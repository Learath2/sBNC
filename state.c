#define MODULE_NAME "state"

#include "state.h"
#include "util.h"

#define MAX_CHANNELS 20
#define INITIAL_BUFFER_SIZE 10
#define BUFFER_GFACTOR 2
#define MAX_005LINES 5

struct channel {
	char name[200];
	char *buffer;
	size_t buffer_cur;
}

char g_sumodes[32];
char g_scmodes[32];
char g_005[MAX_005LINES][513];
int g_n005 = 0;

char g_nick[10];
bool g_away = false;

struct channel g_channels[MAX_CHANNELS] = {{"", NULL}};
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
	if(g_n005 == MAX_005LINES){ //IDK wat do
		DBG("Number of 005 lines exceed "XSTR(MAX_005LINES));
		return;
	}

	util_strncpy(g_005[g_n005++], s, sizeof g_005[0]); //Probably need to modify this a bit
}

void state_server_005(int id)
{
	for(int i = 0; i < g_n005; i++)
		clt_message_send(g_005[i], strlen(g_005[i]));
}

void state_channel_join(char *chan)
{
	for(int i = 0; i < MAX_CHANNELS; i++)
		if(g_channels[i].name[0] == '\0')
			util_strncpy(g_channels[i].name, chan, sizeof g_channels[0].name);
	g_nchannels++;
}

int state_channel_id_get(char *chan)
{
	for(int i = 0; i < MAX_CHANNELS; i++)
		if(!strcmp(g_channels[i].name, chan))
			return i;
	return -1;
}

void state_channel_part(char *chan)
{
	g_channels[state_channel_id_get(chan)].name[0] = '\0';
	g_nchannels--;
}

void state_channel_client_init(int id)
{
	for(int i = 0; i < MAX_CHANNELS; i++){
		if(g_channels[i].name[0] != '\0'){
			clt_message_sendf(id, ":%s!%s@%s JOIN %s", g_nick, g_user, core_host(), g_channels[i]);
			srv_message_sendf("NAMES %s", g_channels[i].name); //Need to route the replies
		}
	}
}

bool state_is_away() { return g_away; }

void state_mark_away()
{
	g_away = true;
}

void state_unmark_away()
{
	g_away = false;
	for(int i = 0; i < MAX_CHANNELS; i++)
		free(g_channels[i].buffer);
}

void state_buffer(char *channel, char* msg)
{
	struct channel *c = g_channels[state_channel_id_get(channel)];

	if(!c->buffer){
		c->buffer = malloc(sizeof *g_channels[0].buffer * 513 * INITIAL_BUFFER_SIZE);
		c->buffer_cur = 0;
	}

	if(!c->buffer){
		DBG("OOM in state_buffer()");
		return;
	}

	char *tmp = NULL;
	if(c->buffer_cur && c->buffer_cur % INITIAL_BUFFER_SIZE == 0)
		tmp = realloc(c->buffer, sizeof *g_channels[0].buffer * 513 * c->buffer_cur * BUFFER_GFACTOR);
	if(!tmp){
		DBG("OOM in state_buffer()");
		return;
	}
	else
		c->buffer = tmp;

	util_strncpy(c->buffer[c->buffer_cur++], msg, 513);
}

void state_buffer_play(int clid)
{
	for(int i = 0; i < MAX_CHANNELS; i++){
		struct channel *c = g_channels[i];
		for(int j = 0; j < c->buffer_cur; j++)
			clt_message_send(clid, c->buffer[j], strlen(c->buffer[j]));
	}
}

#undef MODULE_NAME
