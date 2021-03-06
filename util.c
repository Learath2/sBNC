#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "util.h"

void *util_dup(const void *src, size_t len)
{
    void *new = malloc(len);

    if(!new)
        return NULL;
    return (void *)memcpy(new, src, len);
}

char *util_strdup(const char *src)
{
	size_t len = strlen(src) + 1;
	return (char *)util_dup(src, len);
}

void util_strncpy(char *dst, const char *src, size_t dstsz)
{
	if(!dstsz)
		return;

	while(dstsz--)
		if(!(*dst++ = *src++))
			return;
	dst[-1] = '\0';
}

int util_clamp(int val, int min, int max)
{
	if(val < min)
		return min;
	if(val > max)
		return max;
	return val;
}

void util_randstr(char *buf, size_t len, const char *alphabet)
{
	int alphalen = strlen(alphabet);

	while(len--)
		*buf++=alphabet[rand()%alphalen];
	*buf = '\0';
}

int util_tokenize(char *buf, char **tokarr, size_t toksize)
{
	if(buf[0] == ' ') //Cant begin with whitespace
		return 0;

	int ntok = 0;

	while(*buf && ntok < toksize){
		tokarr[ntok++] = buf;
		if(ntok > 1 && *buf == ':')
			break;
		while(*buf && *buf != ' ') buf++;
		if(!*buf)
			break;
		*buf++ = '\0';
		while(*buf && *buf == ' ') buf++;
	}

	return ntok;
}

struct irc_message util_irc_message_parse(char *msg)
{
	struct irc_message r;
	for(int i = 0; i < MAX_TOKENS; i++)
		r.tokarr[i] = NULL;

	r.ntok = util_tokenize(msg, r.tokarr, COUNT_OF(r.tokarr));

	if(r.tokarr[0][0] == ':')
		r.prefix = util_irc_prefix_parse(r.tokarr[0]);
	else{
		r.prefix.nick = NULL;
		r.prefix.user = NULL;
		r.prefix.host = NULL;
	}

	r.cmd = (r.prefix.nick) ? 1 : 0;
	r.middle = r.cmd + 1;
	r.trailing = (r.tokarr[r.ntok - 1][0] == ':') ? r.ntok - 1 : -1;

	if(r.trailing)
		r.tokarr[r.trailing]++;

	return r;
}

struct irc_prefix util_irc_prefix_parse(char *prefix)
{
	struct irc_prefix p;

	char *t = strchr(prefix, '@');
	if(t){
		*t = '\0';
		p.host = t + 1;
	}
	else
		p.host = NULL;

	t = strchr(prefix, '!');
	if(t){
		*t = '\0';
		p.user = t + 1;
	}
	else
		p.user = NULL;

	p.nick = prefix;

	return p;
}

void util_irc_prefix_construct(char *buf, size_t bufsz, struct irc_prefix p)
{
	snprintf(buf, bufsz, "%s!%s@%s", p.nick, p.user, p.host);
}

void util_parse_hostspec(char *host, size_t sz, int *port, bool *ssl, const char *hostspec)
{
	if(hostspec[0] == '!')
		*ssl = true;
	char *sep = strchr(hostspec, ':');
	if(sep)
		*sep = '\0';
	util_strncpy(host, hostspec, sz);
	if(sep)
		*port = strtol(sep + 1, NULL, 10);
	else
		*port = 6667;
}

void util_mkdir_r(char *path)
{
	char buf[2048];
	util_strncpy(buf, path, sizeof buf);
	for(char *p = buf + 1; *p != '\0'; p++)
	{
		if(*p == '/' && *(p+1) != '\0')
		{
			*p = '\0';
			mkdir(buf, 0755);
			*p = '/';
		}
	}
}