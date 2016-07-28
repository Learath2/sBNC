#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

struct irc_prefix{
	char *nick;
	char *user;
	char *host;
};

struct irc_msg{
	char *tokarr;
	int ntok;

	struct irc_prefix prefix;
	int middle;
	int cmd;
	int trailing;
};

void *util_dup(const void *src, size_t len);
int util_tokenize(char *buf, char **tokarr, size_t toksize);