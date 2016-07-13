typedef enum {

} clt_state_t;

int clt_init(int port);
int clt_accept(void);
void clt_process_msg(int fd, int *syn, char **tokarr, int ntok);