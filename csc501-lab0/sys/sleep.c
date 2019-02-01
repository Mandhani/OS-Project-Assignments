/* sleep.c - sleep */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include<lab0.h>

/*------------------------------------------------------------------------
 * sleep  --  delay the calling process n seconds
 *------------------------------------------------------------------------
 */
SYSCALL	sleep(int n)
{
	int tracekey = KSLEEP;
	unsigned long starttime = starttrace(tracekey);
	STATWORD ps;    
	if (n<0 || clkruns==0) {
		stoptrace(tracekey,starttime);
		return(SYSERR);
	}
	if (n == 0) {
	        disable(ps);
		resched();
		restore(ps);
		stoptrace(tracekey,starttime);
		return(OK);
	}
	while (n >= 1000) {
		sleep10(10000);
		n -= 1000;
	}
	if (n > 0)
		sleep10(10*n);
	stoptrace(tracekey,starttime);
	return(OK);
}
