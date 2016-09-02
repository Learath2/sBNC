#ifndef STORE_H_
#define STORE_H_

void store_init();
void store_msg(char *msg);
bool store_store(char *path, char *msg);
bool store_format_parse(char *buf, size_t bufsz, const char *fmt, const char *channel);

#endif
