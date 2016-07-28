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

int util_tokenize(char *buf, char **tokarr, size_t toksize)
{
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