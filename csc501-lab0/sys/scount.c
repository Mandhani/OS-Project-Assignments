/* scount.c - scount */

#include <conf.h>
#include <kernel.h>
#include <sem.h>
#include<lab0.h>

/*------------------------------------------------------------------------
 *  scount  --  return a semaphore count
 *------------------------------------------------------------------------
 */
SYSCALL scount(int sem)
{
	int tracekey = KSCOUNT;
	unsigned long starttime = starttrace(tracekey);
extern	struct	sentry	semaph[];

	if (isbadsem(sem) || semaph[sem].sstate==SFREE) {
		stoptrace(tracekey,starttime);
		return(SYSERR);
	}
	stoptrace(tracekey,starttime);
	return(semaph[sem].semcnt);
}
