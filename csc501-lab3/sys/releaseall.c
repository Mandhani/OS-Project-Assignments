#include<kernel.h>
#include<proc.h>
#include<lock.h>
#include<sleep.h>

int releaseall (int numlocks, int ldes1, ...) {
	STATWORD ps;
	disable(ps);
	int *i;
	i = (int*) &ldes1;
	int locks[numlocks];
	int k;
	int returnValue = OK;
	struct pentry *pptr;
	pptr = &proctab[currpid];
	for(k=0;k<numlocks;k++) {//use *i to access the ldes
//		kprintf("releaseall called with %d numlocks\n", numlocks);
		int ldes = *i; //Yay!!
		if(pptr->locks_held[ldes] != 1) {
			returnValue = SYSERR;
//			kprintf("Lock not held. CurrPID: %d,LockHeld value: %d, Lock Value: %d\n",currpid,pptr->locks_held[ldes],ldes);
			continue;
		}
		//update self priority
		int l,max_prio = pptr->pprio;
		for(l=0;l<NLOCKS;l++) {
			if(pptr->locks_held[l] == 1) {
				if(max_prio < ld_tab[l].lprio) {
					max_prio = ld_tab[l].lprio;
				}
			}
		}
		if (max_prio > pptr->pprio) {
			pptr->pinh = max_prio;
		} else {
			pptr->pinh = 0;
		}
		//check last reader
		ld_tab[ldes].pids[currpid] = 0;
		pptr->locks_held[ldes] = 0;
//		kprintf("CHanged locksHeld for pid: %d. Set to %d\n", currpid, pptr->locks_held[ldes]);
		int readersleft = 0;
		if(ld_tab[ldes].state == READ) {
//			kprintf("Check");
			int j;
			for(j=0;j<NPROC;j++){
				if(ld_tab[ldes].pids[j] == 1) {
					readersleft = 1;
				}
			}
		}
//		kprintf("Reader left: %d\n", readersleft);
		if(readersleft) {
			restore(ps);
			return returnValue;
		}//need to schedule new reader/writer here...
//		kprintf("No more readers left or writer completed. Scheduling someone new\n");
		int foundsamewp = 0,newpid=0,maxwp = -1;
		int m;
		for(m=0;m<NPROC;m++) {
			if(ld_tab[ldes].wl_pids[m] == 1) {
				if(maxwp == ld_tab[ldes].wl_wprio[m]) {
					unsigned long wt_newpid = ctr1000 - ld_tab[ldes].wl_waitstarttime[newpid];
					unsigned long wt_m = ctr1000 - ld_tab[ldes].wl_waitstarttime[m];
					if(abs(wt_newpid - wt_m) >= 1000) {
						if(wt_newpid < wt_m) {
							newpid = m;
						}
					} else {
						if(ld_tab[ldes].wl_type[newpid] == READ && ld_tab[ldes].wl_type[m] == WRITE) {
							newpid = m;
						}//same wt and wp
					}
				} else if(maxwp < ld_tab[ldes].wl_wprio[m]) {
					newpid = m;
					maxwp = ld_tab[ldes].wl_wprio[m];
					foundsamewp = 0;
				}
//				kprintf("Looking at pid %d. Newpid: %d\n", m, newpid);
			}
		}
//		kprintf("Found that someone! %d\n", newpid);	
		//remove from wl
		ld_tab[ldes].wl_pids[newpid] = 0;
		struct pentry *nptr;
		nptr = &proctab[newpid];
		nptr->locks_held[ldes] = 1;
		ready(newpid, RESCHNO);
		//find other readers if type is read.
		if(ld_tab[ldes].wl_type[newpid] == READ) {
			//find highest writer
			int o, max_writer_prio = -1;
			for(o=0;o<NPROC;o++) {
				if(ld_tab[ldes].wl_pids[o] == 1 && ld_tab[ldes].wl_type[o] == WRITE) {
					if(max_writer_prio < ld_tab[ldes].wl_wprio[o]) {
						max_writer_prio = ld_tab[ldes].wl_wprio[o];
					}
				}
			}
			if(max_writer_prio != -1) {
				//release readers with greater or equal wprio
				for(o=0;o<NPROC;o++) {
					if(ld_tab[ldes].wl_pids[o] == 1 && ld_tab[ldes].wl_type[o] == READ) {
						if(ld_tab[ldes].wl_wprio[o] >= max_writer_prio) {
							ld_tab[ldes].wl_pids[o] = 0;
							ready(o, RESCHNO);
							nptr->locks_held[ldes] = 1;
						}
					}
				}
			}
		}
		int newlprio = -1,n;
		for(n=0;n<NPROC;n++) {
			if(ld_tab[ldes].wl_pids[n] == 1) {
				if(newlprio > &proctab[n].pprio) {
					newlprio = &proctab[n].pprio;
				}
			}
		}
		ld_tab[ldes].lprio = newlprio;
		//update the lprio after scheduling...	
		*i++;
	}
	restore(ps);
	return returnValue;
}
