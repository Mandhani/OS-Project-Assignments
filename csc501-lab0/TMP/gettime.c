/* gettime.c - gettime */

#include <conf.h>
#include <kernel.h>
#include <date.h>
#include<lab0.h>

extern int getutim(unsigned long *);

/*------------------------------------------------------------------------
 *  gettime  -  get local time in seconds past Jan 1, 1970
 *------------------------------------------------------------------------
 */
SYSCALL	gettime(long *timvar)
{
    /* long	now; */
	int tracekey = KGETTIME;
	unsigned long starttime = starttrace(tracekey);
	stoptrace(tracekey,starttime);
	/* FIXME -- no getutim */

    return OK;
}
