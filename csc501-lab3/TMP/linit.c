#include<kernel.h>
#include<proc.h>
#include<lock.h>

struct ld_tab_entry ld_tab[NLOCKS];

void linit() {
	STATWORD ps;
	disable(ps);
	int i;
	for(i=0;i<NLOCKS;i++) {
		ld_tab[i].state = DELETED;
		ld_tab[i].lprio = -1;
		int j;
		for(j=0;j<NPROC;j++) {
			ld_tab[i].pids[j] = 0;
			ld_tab[i].wl_pids[j] = 0;
			ld_tab[i].wl_type[j] = 0;
			ld_tab[i].wl_wprio[j] = 0;
			ld_tab[i].wl_waitstarttime[j] = 0;
			ld_tab[i].wl_pids_old[j] = 0;
			struct pentry *pptr;
			pptr = &proctab[j];
			pptr->locks_held[1] = 0;
		}
	}
	restore(ps);
}
