/* vcreate.c - vcreate */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/

/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	hsize;			/* virtual heap size in pages	*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{
	STATWORD ps;
	disable(ps);
	if(hsize<0 || hsize>256) {
		kprintf("vcreate: Error! wrong hsize specified");
		restore(ps);
		return SYSERR;
	}
	int bs_id;
	if(get_bsm(&bs_id)==SYSERR) {
		kprintf("vcreate: Error! No backing stores available for private heap");
		restore(ps);
		return SYSERR;
	}
	int newpid = create(procaddr, ssize, hsize, priority, name, nargs, args);
	int vpno = 4096;
	if(bsm_map(newpid, vpno, bs_id, hsize) == SYSERR) {
		kprintf("vcreate: Error! Failed during bsm_map");
		restore(ps);
		return SYSERR;
	}
	bsm_tab[bs_id].bs_pheap = 1;
	proctab[newpid].vhpno = vpno;
	proctab[newpid].vhpnpages = hsize;
	proctab[newpid].vmemlist->mnext = vpno*NBPG;
	struct mblock *php;
	php = bs_id * BACKING_STORE_UNIT_SIZE + BACKING_STORE_BASE;
	php->mlen = hsize*NBPG;
	php->mnext = NULL; 
	restore(ps);
	return newpid;
//	kprintf("To be implemented!\n");
//	return OK;
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
