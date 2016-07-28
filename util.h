void *util_dup(const void *src, size_t len);
int util_tokenize(char *buffer, size_t bufsize, char **tokarr, size_t toksize);
struct ircmessage util_msg_syntax(char **tokarr, size_t ntok);