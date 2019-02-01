/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
bs_map_t bsm_tab[MAX_NUM_BS];
SYSCALL init_bsm() {
	STATWORD(ps);
	disable(ps);
	int a;
	for(a=0;a<MAX_NUM_BS;a++) {
		int tempArray[NPROC] = { 0 };
		int tempArray2[NPROC] = { 0 };
		int tempArray3[NPROC] = { 0 };
		bsm_tab[a].bs_pid = tempArray;
		bsm_tab[a].bs_vpno = tempArray2;
		bsm_tab[a].bs_npages = tempArray3;
		bsm_tab[a].bs_status = BSM_UNMAPPED;
	}
	for (a=0; a<NPROC;a++) {
		proctab[a].store = -1;
	}
	restore(ps);
	return OK;
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail) {
	STATWORD(ps);
	disable(ps);
	int a=0;
	while(a<MAX_NUM_BS) {
		if(bsm_tab[a].bs_status == BSM_UNMAPPED) {
			*avail = a;
			restore(ps);
			return OK;
		}
		a++;
	}
	restore(ps);
	return SYSERR;
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int bs_id, int pid) {
	STATWORD(ps);
	disable(ps);
	if(bs_id>=MAX_NUM_BS || bs_id<0) {
		restore(ps);
		return SYSERR;
	}
	bsm_tab[bs_id].bs_pid[pid] = 0;
	bsm_tab[bs_id].bs_vpno[pid] = 0;
	bsm_tab[bs_id].bs_npages[pid] = 0;
	if(bsm_tab[bs_id].bs_pheap == 0) {
		int flag = 0, i;
		for (i=0;i<NPROC;i++) {
			if(bsm_tab[bs_id].bs_pid[i] == 1) {
				flag = 1;
				break;
			}
		}
		if(flag == 0) {
			bsm_tab[bs_id].bs_status = BSM_UNMAPPED;
		}
        } else {
		bsm_tab[bs_id].bs_status = BSM_UNMAPPED;
	}
	restore(ps);
	return OK;
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth) {
	STATWORD ps;
	disable(ps);
	int a;
	for(a=0;a<MAX_NUM_BS;a++) {
		if(bsm_tab[a].bs_pid[pid] == 1) {
			*pageth = vaddr/NBPG;
			*pageth = *pageth - bsm_tab[a].bs_vpno[pid];
			*store = a;
			restore(ps);
			return OK;
		}
	}
	restore(ps);
	return SYSERR;
}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages) {
	STATWORD ps;
	disable(ps);
	if(source>=MAX_NUM_BS || source < 0) {
		kprintf("bsm_map: Error! invalid backing store. Should be between 0 and 7\n");
		restore(ps);
		return SYSERR;
	}
	if(npages > 256 || npages < 1) {
		kprintf("bsm_map: Error! Number of pages should be between 1 to 256\n");
		restore(ps);
		return SYSERR;
	}
	if(bsm_tab[source].bs_pheap == 1) {
		kprintf("bsm_map: Error! BS used by private heap.\n");
		restore(ps);
		return SYSERR;
	}
	//kprintf("pid : %d, store: %d, source: %d", pid, proctab[pid].store, source);
	if(proctab[pid].store != -1 && proctab[pid].store != source) {
		kprintf("bsm_map: Error! Process is already mapped to a different BS\n");
		restore(ps);
		return SYSERR;
	}
	bsm_tab[source].bs_pid[pid] = 1;
	bsm_tab[source].bs_vpno[pid] = vpno;
	bsm_tab[source].bs_npages[pid] = npages;
	bsm_tab[source].bs_status = BSM_MAPPED;
	proctab[pid].store = source;
	restore(ps);
	return OK;
}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno) {
	STATWORD ps;
	disable(ps);
	int bs_id = proctab[pid].store;
	free_bsm(bs_id, pid);
	proctab[pid].store = -1;
	restore(ps);
	return OK;
}


