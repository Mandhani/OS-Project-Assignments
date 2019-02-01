#ifndef LIST_H_
#define LIST_H_

// List.h code
//
#define FREE 1
#define READ 2
#define WRITE 3

#define NLOCKS 50

struct ld_tab_entry {

int state;
int lprio;
int pids[NPROC]; /*currently holding lock */
int wl_pids[NPROC]; /* Wait list */
int wl_type[NPROC];
int wl_wprio[NPROC];
unsigned long wl_waitstarttime[NPROC];
int wl_pids_old[NPROC]; /* Handles SYSERR on lock delete */

};

extern struct ld_tab_entry ld_tab[];

extern void linit();
extern int ldelete(int);
extern unsigned long ctr1000;
extern int lock(int, int, int);
extern int releaseall(int,int,...);
extern void task1();
#endif
