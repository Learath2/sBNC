typedef enum{

} clt_state_t;

typedef enum{
	CLIENT_STATE_EMPTY = -1,
	CLIENT_STATE_INIT = 0
} client_state_t;

struct client{
	int fd;
	client_state_t state;
	int lastping;
};

int clt_clients_get_id(int fd);
int clt_clients_add(int fd);
void clt_clients_remove_id(int id);
void clt_clients_remove_fd(int fd);
int clt_init(int port);
int clt_accept();
void clt_message_process(int fd, char *buf);
void clt_message_send(int fd, void *data, size_t datasz);