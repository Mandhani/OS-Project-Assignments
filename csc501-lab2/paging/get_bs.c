#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) {

  /* requests a new mapping of npages with ID map_id */
//    kprintf("To be implemented!\n");
	STATWORD ps;
	disable(ps);
	if(npages > 256 || npages <= 0 ) {
		kprintf("get_bs: number of pages should be between 0 to 256\n");
		restore(ps);
		return SYSERR;
	}
	if(bs_id < 0 || bs_id > 7) {
		kprintf("get_bs: Backing Store id should be between 0 to 7\n");
		restore(ps);
		return SYSERR;
	}
	if(bsm_tab[bs_id].bs_pheap == 1) {
		kprintf("get_bs: Backing store used by a process for private heap\n");
		restore(ps);
		return SYSERR;
	}
	if(bsm_tab[bs_id].bs_status == BSM_MAPPED) {
		restore(ps);
		return 256;
	}
	restore(ps);
	return npages;
}


