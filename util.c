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
	dest[-1] = '\0';
}

int util_clamp(int val, int min, int max)
{
	if(val < min)
		return min;
	if(val > max)
		return max;
	return val;
}

int util_tokenize(char *buf, char **tokarr, size_t toksize)
{	
	if(buf[0] == ' ') //Cant begin with whitespace
		return 0;

	int ntok = 0;
	int len = strlen(buf);

	for(int i = 0; i < len; i++){
		while(i < len && buf[i] == ' ') i++; //Discard leading whitespace
		tokarr[ntok++] == &buf[i];
		if(i && buf[i] == ':') break; //Trailing
		while(i < len && buf[i] != ' ') i++;
		buffer[i++] = '\0';
	}
	return ntok;
}

struct irc_message util_irc_message_parse(char *msg)
{
	struct irc_message r;
	r.ntok = util_tokenize(msg, r.tokarr, COUNT_OF(r.tokarr));
	r.prefix = (r.tokarr[0][0] == ':') ? util_irc_prefix_parse(r.tokarr[0]) : {0};
	r.cmd = (r.prefix) ? 1 : 0;
	r.middle = r.cmd + 1;
	r.trailing = (r.tokarr[ntok - 1][0] == ':') ? ntok - 1 : -1;

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

void util_parse_hostspec(char *host, size_t sz, int *port, char *hostspec)
{
	char *sep = strchr(hostspec, ':');
	if(sep)
		*sep = '\0';
	util_strncpy(host, hostspec, sz);
	if(sep)
		*port = strtol(sep[1], NULL, 10);
	else
		*port = 6667;
}
