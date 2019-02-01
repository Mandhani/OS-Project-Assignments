#include<kernel.h>
#include<proc.h>
#include<lock.h>

int ldelete (int lockdescriptor) {
	STATWORD ps;
	disable(ps);
	if(lockdescriptor<0 || lockdescriptor>=NLOCKS) {
		restore(ps);
		return SYSERR;
	}
	int i;
	for(i=0;i<NPROC;i++) {
		if(ld_tab[lockdescriptor].wl_pids[i] == 1) {
			ld_tab[lockdescriptor].wl_pids_old[i] = 1;
			ready(i,RESCHNO);
			ld_tab[lockdescriptor].wl_pids[i] = 0;
		}
		ld_tab[lockdescriptor].pids[i] = 0;
	}
	ld_tab[lockdescriptor].state = DELETED;
	ld_tab[lockdescriptor].lprio = -1;
	restore(ps);
	return OK;
}
