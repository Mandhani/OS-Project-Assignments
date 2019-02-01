/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
extern int page_replace_policy;
extern int debug;
fr_map_t frm_tab[NFRAMES];
int sc_head = 5;
SYSCALL init_frm()
{
	STATWORD ps;
	disable(ps);
	int a = 0;
	for (a = 0; a < NFRAMES; a++) {
		frm_tab[a].fr_status = FRM_UNMAPPED;
	}
	restore(ps);
//  kprintf("To be implemented!\n");
	return OK;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* avail)
{
	STATWORD ps;
	disable(ps);
	int a;
	for(a=0;a<NFRAMES;a++){
		if(frm_tab[a].fr_status == FRM_MAPPED) {
			continue;
		}
		*avail = a;
		restore(ps);
		return OK;
	}
	//page fault
	if(page_replace_policy == SC) {
		if((sc_head + 1) == NFRAMES) {
			sc_head = 5;
		}
		int count = 0;
		a = sc_head;
		while(count < (NFRAMES-5)*2) {//NFRAMES minus first five
			if(frm_tab[a].fr_type != FR_PAGE) {
				count++;
				a++;
				if(a == NFRAMES) {
					a = 5;
				}
				continue;
			}
			unsigned long pdbr;
			unsigned long tempva;
			pdbr = proctab[frm_tab[a].fr_pid].pdbr;
			tempva = frm_tab[a].fr_vpno*NBPG;
			virt_addr_t *va;
			va = (virt_addr_t *)&tempva;
			pd_t *pd;
			pt_t *pt;
			int pdo,pto;
			pdo = va->pd_offset;
			pto = va->pt_offset;
			pd = (pd_t *)(pdbr + pdo*sizeof(pd_t));
			pt = (pd_t *)(pd->pd_base*NBPG + pto*sizeof(pt_t));
			if(pt->pt_acc == 0) {
				free_frm(a);
				*avail = a;
				if(debug == 1) {
					kprintf("Replacing Frame Number: %d\n", a);
				}
				sc_head = a;
				restore(ps);
				return OK;
			} else {
				pt->pt_acc = 0;
				count ++;
				a++;
				if(a == NFRAMES) {
					a = 5;
				}
			}
		}
		kprintf("get_frm: could not find a page to replace\n");
		restore(ps);
		return SYSERR;
	}
	kprintf("get_frm: Page Replacement policy is not SC!\n");
	restore(ps);
	return SYSERR;
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{
	STATWORD ps;
	disable(ps);
	unsigned long pdbr;
	unsigned long tempva;
	pdbr = proctab[frm_tab[i].fr_pid].pdbr;
	tempva = frm_tab[i].fr_vpno*NBPG;
	virt_addr_t *va;
	va = (virt_addr_t *)&tempva;
	pd_t *pd;
	pt_t *pt;
	int pdo,pto;
	pdo = va->pd_offset;
	pto = va->pt_offset;
	pd = (pd_t *)(pdbr + pdo*sizeof(pd_t));
	pt = (pd_t *)(pd->pd_base*NBPG + pto*sizeof(pt_t));
	pt->pt_pres = 0;
	if(frm_tab[i].fr_pid == currpid) {
		asm volatile("invlpg (%0)"
				:
				:"r"(pt)
				:"memory"
				);
	}
	frm_tab[pd->pd_base - FRAME0].fr_refcnt--;
	if(frm_tab[pd->pd_base - FRAME0].fr_refcnt == 0) {
	//	kprintf("free_frm: No more entries in this page table. Removing it");
		pd->pd_pres = 0;
		frm_tab[pd->pd_base - FRAME0].fr_status = FRM_UNMAPPED;
	}
	int store,pageth;
	if(bsm_lookup(frm_tab[i].fr_pid,tempva,&store,&pageth)==SYSERR) {
		kprintf("free_frm: Error! Lookup failed for addr: 0x%x\n", tempva);
		kill((int)frm_tab[i].fr_pid);
	}
	//kprintf("free_frm: removing and writing frame: PID: %d, vpno: %d, addr: 0x%x\n",frm_tab[i].fr_pid, frm_tab[i].fr_vpno, tempva);
	write_bs((FRAME0 + i)*NBPG,store,pageth);
	frm_tab[i].fr_status = FRM_UNMAPPED;
	restore(ps);
	return OK;
//  kprintf("To be implemented!\n");
//  return OK;
}



