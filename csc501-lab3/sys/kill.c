/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
void kill_releaseall(int);
void kill_releaselocks(int);
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			kill_releaseall(pid);
			kill_releaselocks(pid);
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;
	case PRLWAIT:	kill_releaseall(pid);
	//		kill_releaselocks(pid); //can't release locks not owned by this process.

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
	restore(ps);
	return(OK);
}

void kill_releaseall(int pid) {
	STATWORD ps;
	disable(ps);
	struct pentry *pptr;
	pptr = &proctab[pid];
//	kprintf("Updating lprio and process priorities\n");
	if(pptr->waiting_on_lock != -1) {
                int ldes = pptr->waiting_on_lock;
		ld_tab[ldes].wl_pids[pid] = 0;
                int newlprio = -1,n;
                for(n=0;n<NPROC;n++) {
			struct pentry *tptr;
			tptr = &proctab[n];
                        if(ld_tab[ldes].wl_pids[n] == 1) {
                                if(newlprio < tptr->pprio) {
                                        newlprio = tptr->pprio;
                                }
                        }
                }
//		kprintf("Updating lprio for lock %d to %d\n", ldes,newlprio);
                ld_tab[ldes].lprio = newlprio;
		for(n=0;n<NPROC;n++) {
			struct pentry *tptr;
			tptr = &proctab[n];
			if(newlprio == -1 && ld_tab[ldes].pids[n] == 1) {
				tptr->pinh = 0;
				continue;
			}
			if((ld_tab[ldes].wl_pids[n] == 1 || ld_tab[ldes].pids[n] == 1) && tptr->pinh > newlprio) {
				tptr->pinh = newlprio;
//				kprintf("Updating pinh prio for process %d to %d\n", n,newlprio);
			}
		}
        }

	restore(ps);
//	kprintf("Returning\n");
}
void kill_releaselocks(int pid) {
	STATWORD ps;
	disable(ps);
//	kprintf("Releaseing all locks for %d\n",pid);
	struct pentry *pptr;
	pptr = &proctab[pid];
	int i;
	for(i=0;i<NLOCKS;i++) {
//		kprintf("Locksheld value for lock %d is %d\n", i, pptr->locks_held[i]);
		if(pptr->locks_held[i] == 1) {
//			kprintf("releaseing");
			releaseall(1, i);
		}
	}
	restore(ps);
}
