#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

enum //Syntax
{
	PREFIX = 0,
	CMD,
	TRAILING,
	SYNELEM
};

void *util_dup(const void *src, size_t len);
int util_tokenize(char *buffer, size_t bufsize, char **tokarr, size_t toksize);
void util_msg_syntax(int *syntax, char **tokarr, size_t ntok);