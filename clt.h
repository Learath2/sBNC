#ifndef CLT_H_
#define CLT_H_

#include <time.h>

typedef enum{
	CLIENT_STATE_EMPTY = -1,
	CLIENT_STATE_REGISTER = 0,
	CLIENT_STATE_REGISTERED,
	CLIENT_STATE_INIT,
	CLIENT_STATE_READY
} client_state_t;

struct client{
	int nid;
	client_state_t state;
	time_t lastact;
	bool auth;
	char *username;
	bool needs_playback;
	char regping[11];

	time_t lastping;
	time_t pingsent;
};

int clt_clients_nid2cid(int nid);
int clt_clients_add(int nid);
void clt_clients_remove_cid(int cid);
void clt_clients_remove_nid(int nid);
int clt_init(void);
void clt_tick(void);
int clt_accept(void);
void clt_message_process(int nid, char *buf);
void clt_message_sendf(int id, const char *format, ... );
void clt_message_send(int id, const char *data);

#endif
