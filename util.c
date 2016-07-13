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

size_t util_tokenize(char *buffer, size_t bufsize, char **tokarr, size_t toksize)
{
	size_t ntok = 0;
	for(int i = 0;i < bufsize && ntok < toksize;){
		while(i < bufsize && buffer[i] == ' ') i++; //Leading whitespace
		tokarr[ntok++] = &buffer[i];
		if(i && buffer[i] == ':') break; //Trailing
		while(i < bufsize && buffer[i] != ' ') i++;
		buffer[i++] = '\0';
	}
	return ntok;
}

void util_msg_syntax(int *syntax, char **tokarr, size_t ntok)
{
	syntax[PREFIX] = (tokarr[0][0] == ':') ? 0 : -1;
	syntax[CMD] = syntax[PREFIX] + 1;
	syntax[TRAILING] = (tokarr[ntok - 1][0] == ':') ? ntok - 1 : -1;
}