/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include<sched.h>
#include<math.h>

unsigned long currSP;	/* REAL sp of counter process */
extern int ctxsw(int, int, int, int);
int expdistsched();
int linuxsched();
int rrqueue[NPROC] = {0};
int rrqlength = 0;
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives counter process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			counter process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int resched()
{
	switch(schedclass) {
		case EXPDISTSCHED: return expdistsched();
		case LINUXSCHED: return linuxsched();
		default: break;//Default RR Scheduler
	}
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	/* no switch needed if counter process priority higher than next*/

	if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
	   (lastkey(rdytail)<optr->pprio)) {
		return(OK);
	}
	
	/* force context switch */
	if (optr->pstate == PRCURR) {
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->pprio);
	}

	/* remove highest priority process at end of ready list */

	nptr = &proctab[ (currpid = getlast(rdytail)) ];
	nptr->pstate = PRCURR;		/* mark it counterly running	*/
#ifdef	RTCLOCK
	preempt = QUANTUM;		/* reset preemption counter	*/
#endif
	
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
	/* The OLD process returns here when resumed. */
	return OK;
}

int expdistsched() {
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	int prev = currpid;
	if ( rrqlength == 0 ) {
		int randomnumber = expdev(0.1);
	// mkushal: proctab iteration implementation (more complexity)
	/* 	int i,max=0;
		int counter = 0;
		for(i=0;i<NPROC;i++) {
			if(max < proctab[i].pprio) {
				max = proctab[i].pprio;
			}
		}
		int minafterrandom = 100;
		for (i=0;i<NPROC;i++) {
			if(proctab[i].pstate==PRREADY) {
				if ( proctab[i].pprio==max && randomnumber > proctab[i].pprio ) {
					prev = i;
					break;
				} else if (proctab[i].pprio<minafterrandom && proctab[i].pprio>randomnumber) {
					minafterrandom = proctab[i].pprio;
					prev = i;
				} 
			}
		}
		
		if ( (proctab[currpid].pprio > randomnumber && proctab[currpid].pprio < proctab[prev].pprio) || 
				( randomnumber > proctab[prev].pprio && proctab[currpid].pprio > proctab[prev].pprio )  ) { 
			//no switch required
			prev = currpid;
		}
		for(i=0;(i<NPROC && proctab[i].pstate==PRREADY && prev!=i);i++) {
			if (proctab[i].pprio == proctab[prev].pprio) {
				rrqueue[rrqlength++]=i;
			}
		}
		
	} else {
		prev = rrqueue[--rrqlength];
	}
	optr = &proctab[currpid];
	if (optr->pstate == PRCURR) {
                optr->pstate = PRREADY;
                insert(currpid,rdyhead,optr->pprio);
        }
	nptr = &proctab[(currpid = dequeue(prev))];
	nptr->pstate = PRCURR;		
	preempt = QUANTUM;		
	
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
	return OK;
	*/
	// QUEUE Implementation for exponential scheduler
	
		prev = q[rdytail].qprev;
		int firstproc = q[rdyhead].qnext;
		while((q[q[prev].qprev].qkey > randomnumber) ) {
			prev = q[prev].qprev;
			if(prev==firstproc) {
				break;
			}
			if(q[q[prev].qprev].qkey == q[prev].qkey){
					int ksmtemp = q[prev].qprev;
				if(ksmtemp == firstproc) {
					break;
				}
				while(q[q[ksmtemp].qprev].qkey == q[ksmtemp].qkey) {
					ksmtemp = q[ksmtemp].qprev;
					if(ksmtemp==firstproc) {
						break;
					}
				}
				if(ksmtemp!=firstproc){
					if(randomnumber < q[q[ksmtemp].qprev].qkey) {
						prev = q[ksmtemp].qprev;
					} else {
						break;
					}
				} else {
					break;
				}
			}
		}
		if(   ( (optr=&proctab[currpid])->pstate == PRCURR ) && 
			(  ( (optr->pprio > randomnumber) && (optr->pprio < q[prev].qkey) ) || ( (optr->pprio >q[prev].qkey) && (q[prev].qkey < randomnumber) )  )   ) {
			prev=currpid;
		}
		if ( q[prev].qkey == q[q[prev].qprev].qkey) {
			int rrtemp = q[prev].qprev;
			rrqueue[rrqlength++]=rrtemp;
			if(rrtemp != firstproc) {
				while(q[rrtemp].qkey==q[q[rrtemp].qprev].qkey) {
					rrtemp = q[rrtemp].qprev;
					rrqueue[rrqlength++]=rrtemp;
					if(rrtemp == firstproc) {break;}
				}
			}
		}
	} else {
		prev = rrqueue[--rrqlength];
		optr = &proctab[currpid];
	}
	if (optr->pstate == PRCURR) {
                optr->pstate = PRREADY;
                insert(currpid,rdyhead,optr->pprio);
        }
	nptr = &proctab[(currpid = dequeue(prev))];
	nptr->pstate = PRCURR;		
	preempt = QUANTUM;		
	
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
	return OK;

}

int linuxsched() {
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	int i;
	int refresh = 1;
	int debug=0;
	proctab[currpid].counter = preempt;
	if(preempt <= 0) {
		proctab[currpid].goodness = 0;
	} else {
		proctab[currpid].goodness = proctab[currpid].counter + proctab[currpid].pprio;
	}
	for(i=0;i<NPROC;i++) {
		if(proctab[i].counter > 0) {
			if(proctab[i].pstate == PRREADY || proctab[i].pstate == PRCURR) {
				refresh = 0;
				if(debug) kprintf("\nNot Refreshing now...");
				break;
			}
		}
	}
	if(refresh ==1) {
		if(debug) kprintf("\nRefreshing Now...");
		for(i=1;i<NPROC;i++) {
			if(proctab[i].pstate != PRFREE) {
				proctab[i].goodness = proctab[i].counter + proctab[i].pprio;;
				if(proctab[i].counter == proctab[i].timequantum || (proctab[i].counter == 0 && proctab[i].timequantum > proctab[i].counter)) {
					proctab[i].timequantum = proctab[i].pprio;
					proctab[i].counter = proctab[i].pprio;
				} else {
					proctab[i].timequantum = ((int)proctab[i].counter/2) + proctab[i].pprio;
					proctab[i].counter = ((int)proctab[i].counter/2) + proctab[i].pprio;
				}
			}
		}
	}
	//find max goodness in all processes
	int maxgoodness = 0;
	int nextproc = 0;
	for(i=0;i<NPROC;i++) {
		if((proctab[i].pstate == PRREADY || proctab[i].pstate == PRCURR ) && proctab[i].counter > 0 && maxgoodness <= proctab[i].goodness) {
			if(debug) kprintf("Setting tempmax for PID: %d, val: %d...",i,proctab[i].goodness);
			maxgoodness = proctab[i].goodness;
			nextproc = i;
		}
	}
	if(debug) kprintf("Found Max Goodness for PID: %d, val: %d",nextproc,maxgoodness);
	if ((optr=&proctab[currpid])->pstate == PRCURR) {
                optr->pstate = PRREADY;
                insert(currpid,rdyhead,optr->pprio);
        }
	int flag=0;
	for(i=1;i<NPROC;i++) {
		if(proctab[i].pstate == PRREADY || proctab[i].pstate == PRCURR) {
			flag=1;
			break;
		}
	}
	if(!flag) {
		if(debug) kprintf("Nothing Running. So running NULL");
		nextproc = 0;
	}
	if(debug) kprintf("Switching from PID: %d, To PID: %d",currpid,nextproc);
        nptr = &proctab[(currpid = dequeue(nextproc))];
        nptr->pstate = PRCURR;
        preempt = nptr->counter; 

        ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

        return OK;	
}
