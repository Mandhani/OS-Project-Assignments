/* getprio.c - getprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include<lab0.h>

/*------------------------------------------------------------------------
 * getprio -- return the scheduling priority of a given process
 *------------------------------------------------------------------------
 */
SYSCALL getprio(int pid)
{
	int tracekey = KGETPRIO;
	unsigned long starttime = starttrace(tracekey);
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		stoptrace(tracekey,starttime);
		return(SYSERR);
	}
	restore(ps);
	stoptrace(tracekey,starttime);
	return(pptr->pprio);
}
