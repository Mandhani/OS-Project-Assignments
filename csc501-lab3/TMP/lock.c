#include<kernel.h>
#include<proc.h>
#include<lock.h>
#include<sleep.h>

int lock (int ldes1, int type, int priority) {
	STATWORD ps;
	disable(ps);
	struct pentry *pptr;
	pptr = &proctab[currpid];
	if(ldes1<0 || ldes1>=NLOCKS) {
		restore(ps);
		return SYSERR;
	}
	if(type != READ && type != WRITE) {
		restore(ps);
		return SYSERR;
	}
	if(ld_tab[ldes1].state == DELETED ) {
		kprintf("Lock was Deleted.");
		restore(ps);
		return SYSERR;
	}
/*	int z;
	for(z=0;z<NLOCKS;z++){
		kprintf("Pid: %d, locks_held: %d\n", currpid, pptr->locks_held[z]);
	}*/
	//kprintf("Entered: %d\n\n",ld_tab[ldes1].wl_pids[currpid]);
	if(ld_tab[ldes1].wl_pids_old[currpid] == 1) {
		//kprintf("Lock was deleted while this process waited on it. Someone else has acquired it now.");
		restore(ps);
		return SYSERR;
	}
	/*int tempprio;
	if(pptr->pinh == 0) {
		tempprio = pptr->pprio;
	} else {
		tempprio = pptr->pinh;
	}*/
	if(ld_tab[ldes1].state == FREE) { //free lock.
		ld_tab[ldes1].state = type;
		ld_tab[ldes1].pids[currpid] = 1;
		pptr->locks_held[ldes1] = 1;
		restore(ps);
		return OK;
	}
	if(ld_tab[ldes1].state == READ) {
		//kprintf("Entering READ");
		if(type == READ) { //shared read
		//	kprintf("Entering shared READ\n");
			//check for other writers with higher priority.
			int i,found=0;
			for(i=0;i<NPROC;i++) {
		//		kprintf("check for other writers with higher priority,Valid: %d Current %d, Writers: %d\n",ld_tab[ldes1].wl_pids[i],priority,ld_tab[ldes1].wl_wprio[i]);
				if(ld_tab[ldes1].wl_pids[i] == 1 && priority < ld_tab[ldes1].wl_wprio[i]) {//finding a writer with not less than reader priority
					found = 1;
					break;
				}
			}
			if(!found) {
//				kprintf("Adding another reader\n");
				ld_tab[ldes1].pids[currpid] = 1;
				pptr->locks_held[ldes1] = 1;
//				kprintf("LocksHeld Value: %d, Lock Value: %d, currpid: %d\n",pptr->locks_held[ldes1], ldes1, currpid);
				restore(ps);
				return OK;
			}
		}
	} //found a writer or need an exclusive write. sleep baby!!!
	if(ld_tab[ldes1].lprio < pptr->pprio ) {
		ld_tab[ldes1].lprio = pptr->pprio;
	}
//	kprintf("This process will wait for lock\n");
	//TBD: check for priority inversion - done! Risky, but done :P
//	kprintf("Calling recursive for pinv. currpid: %d\n",currpid);
	recursivePinv(pptr, ldes1);
	ld_tab[ldes1].wl_pids[currpid] = 1;
	ld_tab[ldes1].wl_type[currpid] = READ;
	ld_tab[ldes1].wl_wprio[currpid] = priority;
	ld_tab[ldes1].wl_waitstarttime[currpid] = ctr1000;
	pptr->waiting_on_lock = ldes1;
	pptr->pstate = PRLWAIT;
	resched();
	if(ld_tab[ldes1].wl_pids_old[currpid] == 1) {
		kprintf("Lock was deleted while this process waited on it.");
		restore(ps);
		return SYSERR;
	}
	restore(ps);	
	return OK;
}

void recursivePinv(struct pentry *pptr, int ldes1) {
//	kprintf("Welcome to recursion\n");
	int i;
	struct pentry *tptr;
	int tmpprio;
	if(pptr->pinh == 0) {
		tmpprio = pptr->pprio;
	} else {
		tmpprio = pptr->pinh;
	}
	for(i=0;i<NPROC;i++) {
		if(ld_tab[ldes1].pids[i] == 1) {
			tptr = &proctab[i];
			if(tptr->pinh == 0) {
				if(tptr->pprio < tmpprio) {
					tptr->pinh = tmpprio;
				//	dequeue(i);
				//	insert(i, rdyhead, tptr->pinh);
//					kprintf("FOund an entry to modify, changing pinh for process: %d, to : %d\n",i,tmpprio);
				}
			} else {
				if(tptr->pinh < tmpprio) {
					tptr->pinh = tmpprio;
				//	dequeue(i);
				//	insert(i, rdyhead, tptr->pinh);
//					kprintf("FOund an entry to modify, I think\n");
				}
			}
			if(tptr->waiting_on_lock !=-1) {
				recursivePinv(tptr, tptr->waiting_on_lock);
			}
		}
	}

}
