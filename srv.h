typedef enum {
	SRV_STATE_OFFLINE = -1,
	SRV_STATE_CONNECTED,
	SRV_STATE_ONLINE
} srv_state_t;

int srv_init(void);
int srv_connect(char *server);
srv_state_t srv_state(void);
int srv_socket(void);
void srv_message_process(char *buf);
void srv_message_sendf(const char *format, ... );
void srv_message_send(void *data, size_t datasz);
