#define MODULE_NAME "proc"

#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "proc.h"
#include "log.h"
#include "clt.h"
#include "srv.h"

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

static struct wqueue g_wqueue = {0};

void proc_wqueue_add(int target, void *data, size_t datasz)
{
	struct wqueue_entry *e = malloc(sizeof *e);
	e->target = target;
	e->data = util_dup(data, datasz);
	e->datasz = datasz;
	e->next = NULL;

	if(!g_wqueue.head)
		g_wqueue.head = g_wqueue.tail = e;
	else
		g_wqueue.tail->next = e, g_wqueue.tail = e;

	g_wqueue.length++;
}

void proc_wqueue_next(void)
{
	struct wqueue_entry *t = g_wqueue.head;
	g_wqueue.head = g_wqueue.head->next;
	g_wqueue.length--;

	if(!g_wqueue.head)
		g_wqueue.tail = NULL;

	free(t->data);
	free(t);
}

size_t proc_wqueue_length(void){ return g_wqueue.length; }
wqueue_entry_t proc_wqueue_head(void){ return g_wqueue.head; }
int proc_wqueue_entry_target(wqueue_entry_t d){ return d->target; }
void *proc_wqueue_entry_data(wqueue_entry_t d){ return d->data; }
size_t proc_wqueue_entry_datasz(wqueue_entry_t d){ return d->datasz; }

void proc_tick(void)
{
	clt_tick();
}

void proc_read(int fd, char *rbuf, size_t len)
{
	char msg[513] = "";
	size_t sz = sizeof msg;

	while(len){
		char *a = msg;
		char *b = rbuf;
		while(sz--){
			char c = (*a++ = *b++);
			if(!c || c == '\n')
				break;
		}

		if(a[-1]){
			a[0] = '\0'; //Ensure NULL termination
			len -= (a - &msg[0]);
			memmove(rbuf, b, len + 1);

			if(fd == srv_socket())
				srv_message_process(msg);
			else
				clt_message_process(fd, msg);
		}
		else{
			util_strncpy(rbuf, msg, 513);
			break;
		}

	}
}

#undef MODULE_NAME
