/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>


/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */
SYSCALL pfint()
{
	STATWORD ps;
	disable(ps);
	unsigned long tempva = read_cr2();
	//kprintf("pfint: INFO: started page fault for address: 0x%x\n", tempva);
	virt_addr_t *va;
	va = (virt_addr_t *)&tempva;
	unsigned int pdo,pto;
	pdo = va->pd_offset;
	pto = va->pt_offset;
	unsigned long pdbr = proctab[currpid].pdbr;
	pd_t *pd;
	pt_t *pt;
	pd = (pd_t *)(pdbr + pdo*sizeof(pd_t));
	if(pd->pd_pres == 0) {
		//create a new frame for PT
		int f_number_pt;
		get_frm(&f_number_pt);
		frm_tab[f_number_pt].fr_status = FRM_MAPPED;
		frm_tab[f_number_pt].fr_type = FR_TBL;
		frm_tab[f_number_pt].fr_refcnt = 4;//first four frames
		frm_tab[f_number_pt].fr_pid = currpid;
		pt = (pt_t *)((f_number_pt + FRAME0)*NBPG);
		int i;
		for(i=0;i<PTD_SIZE;i++) {
			pt->pt_pres = 0;
			pt++;	
		}
		pd->pd_pres = 1;
		pd->pd_write = 1;
		pd->pd_base = f_number_pt + FRAME0;
	}
	pt = (pt_t *) (pd->pd_base*NBPG + pto*sizeof(pt_t));
	int f_number;
	if(pt->pt_pres == 0) {
		get_frm(&f_number);
		frm_tab[f_number].fr_status = FRM_MAPPED;
		frm_tab[f_number].fr_type = FR_PAGE;
		frm_tab[f_number].fr_pid = currpid;
		frm_tab[f_number].fr_vpno = tempva/NBPG;
		pt->pt_pres = 1;
		pt->pt_write = 1;
		pt->pt_acc = 1;
		pt->pt_base = f_number + FRAME0;
		frm_tab[pd->pd_base - FRAME0].fr_refcnt++;
	}
	int store,pageth;
	if(bsm_lookup(currpid,tempva,&store,&pageth) == SYSERR) {
		kprintf("pfint: Error! Lookup failed!!! PID: %d, Address: 0x%x\n", currpid, tempva);
		restore(ps);
		kill(currpid);
		//return SYSERR;
	}
	read_bs((char *)((f_number + FRAME0)*NBPG),store,pageth);
	write_cr3(pdbr);
	restore(ps);
	return OK;

//  kprintf("To be implemented!\n");
//  return OK;
}


