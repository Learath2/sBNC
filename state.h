bool state_nick_set(char *nick);
const char *state_nick();
const char *state_server_umodes();
const char *state_server_cmodes();
void state_server_umodes_set(char *s);
void state_server_cmodes_set(char *s);
void state_server_005_store(char *s);
void state_server_005(int id);
void state_channel_join(char *chan);
int state_channel_joined(char *chan);
void state_channel_part(char *chan);
void state_channel_client_init(int id);