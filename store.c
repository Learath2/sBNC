#define MODULE_NAME "store"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "store.h"
#include "state.h"
#include "sett.h"
#include "log.h"

void store_init()
{
	struct settings *s = sett_get();
	util_mkdir_r(s->spath); //TODO: Implement this in platform.c
}

void store_msg(char *msg)
{
	struct settings *s = sett_get();
	char *t = util_strdup(msg);
	struct irc_message m = util_irc_message_parse(t);

	if(!strcmp(m.tokarr[m.cmd], "PRIVMSG")){
		char path[1024], buf[1024];

		store_format_parse(path, sizeof path, s->sfmt, m.tokarr[m.middle]);
		util_mkdir_r(path);

		util_irc_prefix_construct(buf, sizeof buf, m.prefix);
		strcat(buf, ": ");
		strcat(buf, m.tokarr[m.trailing] + 1);
		store_store(path, buf);
	}
	else if(s->sjoin && (!strcmp(m.tokarr[m.cmd], "JOIN") || !strcmp(m.tokarr[m.cmd], "PART"))){
		char path[1024], buf[1024];

		store_format_parse(path, sizeof path, s->sfmt, m.tokarr[m.middle]);
		util_mkdir_r(path);

		util_irc_prefix_construct(buf, sizeof buf, m.prefix);
		strcat(buf, " ");
		strcat(buf, (m.tokarr[m.cmd][1] == 'J') ? "joined." : "left.");
		store_store(path, buf);
	}

	if(state_is_away())
		state_buffer(m.tokarr[m.middle], msg);

	free(t);
}

bool store_store(char *path, char *msg)
{
	char buf[2048];
	time_t t = time(NULL);
	struct tm *tm = localtime(&t);

	strftime(buf, sizeof buf, "%T ", tm);
	strcat(buf, msg);

	FILE *f = fopen(path, "a");
	if(!f)
		return false;

	fputs(buf, f);
	fclose(f);

	return true;
}

bool store_format_parse(char *buf, size_t bufsz, const char *fmt, const char *channel)
{
	char *temp = malloc(sizeof *temp * bufsz);
	size_t sz = bufsz;
	char *a = temp;
	struct settings *s = sett_get();

	strcpy(a, s->spath);
	a += strlen(s->spath);
	*a++ = '/';

	while(sz-- > 0){
		if(*fmt == '%'){
			switch(*(fmt + 1)){
				case 'D':
				case 'M':
				case 'Y':
					*a++ = *fmt++;
					continue;
				case 'h':
					if(sz < strlen(s->server.host))
						return false;
					strcpy(a, s->server.host);
					a += strlen(s->server.host);
					sz -= strlen(s->server.host);
					break;
				case 'c':
					if(sz < strlen(channel))
						return false;
					strcpy(a, channel);
					a += strlen(channel);
					sz -= strlen(channel);
					break;
				case 'E': ;
					char tmp[32];
					snprintf(tmp, sizeof tmp, "%ld", s->rawepoch);
					if(sz < strlen(tmp))
						return false;
					strcpy(a, tmp);
					a += strlen(tmp);
					sz -= strlen(tmp);
					break;
				default:
					ERR("Unknown format specifier in store format.");
					return false;
			}
			fmt += 2;
		}
		else if(!(*a++ = *fmt++))
			goto fuckthisshit;
	}
	if(!sz)
		return false;

fuckthisshit: ;
	time_t t = time(NULL);
	struct tm *tm = localtime(&t);
	strftime(buf, bufsz, temp, tm);
	free(temp);
	return true;
}

#undef MODULE_NAME
