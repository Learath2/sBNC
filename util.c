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