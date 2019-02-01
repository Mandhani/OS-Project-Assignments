#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

SYSCALL release_bs(bsd_t bs_id) {

  /* release the backing store with ID bs_id */
//    kprintf("To be implemented!\n");
	STATWORD ps;
	disable(ps);
	if( bs_id < 0 || bs_id > 7) {
		kprintf("release_bs: Error in bs_id. Should be between 0 to 7\n");
		restore(ps);
		return SYSERR;
	}
	free_bsm(bs_id, currpid);
	restore(ps);
	return OK;

}

