#include "proc.h"
#include "clt.h"
#include "srv.h"
#include "util.h"

struct wqueue_entry
{
	int target;
	void *data;
	size_t datasz;
	struct wqueue_entry *next;
};

struct wqueue
{
	struct wqueue_entry *head;
	struct wqueue_entry *tail;
	size_t length;
};

static void *proc_util_dup(const void *src, size_t len);

struct wqueue g_wqueue = {0};

void proc_wqueue_add(int target, void *data, size_t datasz)
{
	struct wqueue_entry *e = malloc(sizeof *e);
	e->target = target;
	e->data = util_dup(data, datasz);
	e->datasz = datasz;
	e->next = NULL;

	if(!head)
		head = tail = e;
	else
		tail->next = e, tail = e;
}

void proc_wqueue_next()
{
	struct wqueue_entry *t = g_wqueue.head;
	g_wqueue.head = g_wqueue.head->next;
	g_wqueue.length--;

	if(!head)
		g_wqueue.tail = NULL;

	free(t->data);
	free(t);
}

size_t proc_wqueue_length(){ return g_wqueue.length; }
wqueue_entry_t *proc_wqueue_head(){ return g_wqueue.head; }
int proc_wqueue_entry_target(wqueue_entry_t d){ return d->target; }
void *proc_wqueue_entry_data(wqueue_entry_t d){ return d->data; }
size_t proc_wqueue_entry_datasz(wqueue_entry_t d){ return d->datasz; }

void proc_tick()
{

}

void proc_read(int fd, char *buf, size_t bufsize, int len)
{
	char *tokarr[64] = {NULL};
	int ntok = util_tokenize(buf, bufsize, tokarr, COUNT_OF(tokarr));

	int syntax[SYNELEM];
	util_msg_syntax(tokarr, ntok);

	if(fd == srv_socket()) //Msg from server
		srv_process_msg(syntax, tokarr, ntok);
	else //Msg from client
		clt_process_msg(fd, syntax, tokarr, ntok);
}