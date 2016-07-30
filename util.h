#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

struct irc_prefix{
	char *nick;
	char *user;
	char *host;
};

struct irc_msg{
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
int util_tokenize(char *buf, char **tokarr, size_t toksize);
struct irc_message util_irc_message_parse(char *msg);
struct irc_prefix util_irc_prefix_parse(char *prefix);