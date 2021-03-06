/* getprio.c - getprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * getprio -- return the scheduling priority of a given process
 *------------------------------------------------------------------------
 */
SYSCALL getprio(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || (pptr = &proctab[pid])->pstate == PRFREE) {
		kprintf("Getprio failed for pid : %d, PSTATE: %d\n", pid, pptr->pstate);
		restore(ps);
		return(SYSERR);
	}
	if(pptr->pinh == 0) {
		restore(ps);
		return(pptr->pprio);
	} else {
		restore(ps);
		return(pptr->pinh);
	}
}
