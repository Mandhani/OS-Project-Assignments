/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include <paging.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;

	disable(ps);
	//kprintf("kill: entered\n");
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
	//	kprintf("Badpid\n");
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

	case PRCURR:	cleanup(pid);
			pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
	cleanup(pid);
	restore(ps);
	return(OK);
}
void cleanup(int pid) {
	int a;
	//kprintf("kill: celanup entered\n");
	//kprintf("cleanup: DEBUG: Current Frames:\n");
	for(a=0;a<NFRAMES;a++) {
		unsigned long pa = frm_tab[a].fr_vpno*NBPG;
	//	kprintf("DEBUG: Frame pID: %d, Type: %d, Status: %d addr: 0x%x\n",frm_tab[a].fr_pid, frm_tab[a].fr_type, frm_tab[a].fr_status, pa);
		if(frm_tab[a].fr_pid == pid) {
		/*	if(frm_tab[a].fr_type == FR_PAGE) {
				unsigned long pa = frm_tab[a].fr_vpno*NBPG;
				int store,pageth;
				bsm_lookup(pid,pa,&store,&pageth);
				kprintf("DEBUG: lookup done!\n");
				write_bs((FRAME0+a)*NBPG,store,pageth);
				kprintf("kill: removied mapping for page addr: 0x%x\n",pa);*/
	//		kprintf("DEBUG: Removing Frame pID: %d, Type: %d, Status: %d addr: 0x%x\n",frm_tab[a].fr_pid, frm_tab[a].fr_type, frm_tab[a].fr_status, pa);
			frm_tab[a].fr_status = FRM_UNMAPPED;
			/*} else {
				kprintf("kill: removied mapping for PT or PD\n");
			}
			//frm_tab[a].fr_status = FRM_UNMAPPED;*/
		}
	//	kprintf("DEBUG: frame done!\n");
	}
	//kprintf("cleanup: DEBUG: Releasing bs: %d\n", proctab[pid].store);
	if(proctab[pid].store != -1) {
		release_bs(proctab[pid].store);
	}
}
