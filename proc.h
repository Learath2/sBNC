#ifndef PROC_H_
#define PROC_H_

typedef struct wqueue_entry *wqueue_entry_t;

#define ENT_GET(ENTRY, FIELD) proc_wqueue_entry_##FIELD(ENTRY)

void proc_wqueue_add(int target, void *data, size_t datasz);
void proc_wqueue_next(void);
size_t proc_wqueue_length(void);
wqueue_entry_t proc_wqueue_head(void);
int proc_wqueue_entry_target(wqueue_entry_t d);
void *proc_wqueue_entry_data(wqueue_entry_t d);
size_t proc_wqueue_entry_datasz(wqueue_entry_t d);

void proc_tick(void);
void proc_read(int fd, char *buf, size_t len);

#endif
