typedef enum{

} clt_state_t;

typedef enum{
	CLIENT_STATE_EMPTY = -1,
	CLIENT_STATE_REGISTER = 0,
	CLIENT_STATE_INIT,
	CLIENT_STATE_READY
} client_state_t;

struct client{
	int fd;
	client_state_t state;
	int lastping;
	bool auth;
	char *username;
};

int clt_clients_get_id(int fd);
int clt_clients_add(int fd);
void clt_clients_remove_id(int id);
void clt_clients_remove_fd(int fd);
int clt_init(int port);
int clt_tick(void);
int clt_accept(void);
void clt_message_process(int fd, char *buf);
void clt_message_sendf(int id, const char *format, ... )
void clt_message_send(int fd, void *data, size_t datasz);