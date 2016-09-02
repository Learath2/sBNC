#ifndef UTIL_H_
#define UTIL_H_

#include <stddef.h>
#include <stdbool.h>

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#define XSTR(s) STR(s)
#define STR(s) #s

#define MAX_TOKENS 20
#define VERSION "0.0.1" //Definately doesn't belong here

struct irc_prefix{
	char *nick;
	char *user;
	char *host;
};

struct irc_message{
	char *tokarr[MAX_TOKENS];
	int ntok;

	struct irc_prefix prefix;
	int middle;
	int cmd;
	int trailing;
};

void *util_dup(const void *src, size_t len);
char *util_strdup(const char *src);
void util_strncpy(char *dst, const char *src, size_t dstsz);
int util_clamp(int val, int min, int max);
int util_tokenize(char *buf, char **tokarr, size_t toksize);
struct irc_message util_irc_message_parse(char *msg);
struct irc_prefix util_irc_prefix_parse(char *prefix);
void util_irc_prefix_construct(char *buf, size_t bufsz, struct irc_prefix p);
void util_parse_hostspec(char *host, size_t sz, int *port, bool *ssl, char *hostspec);

#endif
