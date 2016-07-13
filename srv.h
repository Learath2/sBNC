typedef enum {
	SRV_STATE_OFFLINE = -1,
	SRV_STATE_ONLINE
} srv_state_t;

int srv_init(void);
int srv_connect(char *server);
srv_state_t srv_state(void);
int srv_socket(void);
void srv_process_message(int *syn, char **tokarr, int ntok);