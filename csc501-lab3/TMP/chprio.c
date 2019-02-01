/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
SYSCALL chprio(int pid, int newprio)
{
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	pptr->pprio = newprio;
	kill_releaseall(pid);
/*	if(pptr->waiting_on_lock != -1) {
		int ldes = pptr->waiting_on_lock;
		int newlprio = -1,n;
		if(ld_tab[ldes].lprio > 
                for(n=0;n<NPROC;n++) {
                        if(ld_tab[ldes].wl_pids[n] == 1) {
                                if(newlprio > &proctab[n].pprio) {
                                        newlprio = &proctab[n].pprio;
                                }
                        }
                }
                ld_tab[ldes].lprio = newlprio;
	}*/
	restore(ps);
	return(newprio);
}
