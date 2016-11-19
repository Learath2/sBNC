#ifndef NET_H_
#define NET_H_

int net_nfds(void);
int net_init(void);
int net_poll_add_listener(int fd, short events);
int net_poll_add(int fd, short events);
int net_poll_remove(int id);
int net_poll_remove_fd(int fd);
int net_poll(int timeout);
int net_socket_create(void);
int net_fd2id(int fd);
int net_id2fd(int id);
int net_socket_read(int id);
int net_socket_read_fd(int fd);
int net_socket_write(int fd, void *data, size_t sz);
bool net_socket_avail(int id);
bool net_socket_avail_fd(int fd);
void net_socket_msg(int id, char *buf, size_t sz);
void net_socket_msg_fd(int fd, char *buf, size_t sz);

#endif