/* setdev.c - setdev */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include<lab0.h>

/*------------------------------------------------------------------------
 *  setdev  -  set the two device entries in the process table entry
 *------------------------------------------------------------------------
 */
SYSCALL	setdev(int pid, int dev1, int dev2)
{
	int tracekey = KSETDEV;
	unsigned long starttime = starttrace(tracekey);
	short	*nxtdev;

	if (isbadpid(pid)) {
		stoptrace(tracekey,starttime);
		return(SYSERR);
	}
	nxtdev = (short *) proctab[pid].pdevs;
	*nxtdev++ = dev1;
	*nxtdev = dev2;
	stoptrace(tracekey,starttime);
	return(OK);
}
