typedef enum {

} clt_state_t;

int clt_init(int port);
int clt_accept();
void clt_process_msg(char **tokarr, int ntok);