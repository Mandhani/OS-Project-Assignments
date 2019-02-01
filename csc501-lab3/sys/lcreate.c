#include<kernel.h>
#include<proc.h>
#include<lock.h>

int lcreate (void) {
	STATWORD ps;
	disable(ps);
	int i;
	for(i=0;i<NLOCKS;i++) {
		if(ld_tab[i].state == DELETED) {
			ld_tab[i].state = FREE;
			ld_tab[i].lprio = -1;
//			kprintf("\nReturning %d with debug %d\n", i, ld_tab[i].wl_pids_old[currpid]);
			restore(ps);
			return i;
		}
	}
	restore(ps);
	return SYSERR;
}
