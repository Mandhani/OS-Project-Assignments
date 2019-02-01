/* getpid.c - getpid */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include<lab0.h>

/*------------------------------------------------------------------------
 * getpid  --  get the process id of currently executing process
 *------------------------------------------------------------------------
 */
SYSCALL getpid()
{
	int tracekey = KGETPID;
	unsigned long starttime = starttrace(tracekey);
	stoptrace(tracekey,starttime);
	return(currpid);
}
