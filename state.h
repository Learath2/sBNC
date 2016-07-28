bool state_nick_set(char *nick);
const char *state_nick_get();
void state_channel_join(char *chan);
int state_channel_joined(char *chan);
void state_channel_part(char *chan);
void state_channel_new_client(int id);