#ifndef STATE_H_
#define STATE_H_

bool state_nick_set(char *nick);
const char *state_nick();
const char *state_server_umodes();
const char *state_server_cmodes();
void state_server_umodes_set(char *s);
void state_server_cmodes_set(char *s);
void state_server_005_store(char *s);
void state_server_005(int id);
void state_channel_join(char *chan);
int state_channel_id_get(char *chan);
void state_channel_part(char *chan);
void state_channel_client_init(int id);
bool state_is_away();
void state_mark_away();
void state_unmark_away();
void state_buffer(char *channel, char* msg);
void state_buffer_play(int clid);

#endif
