/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmmap(int virtpage, bsd_t source, int npages)
{
//  kprintf("xmmap - to be implemented!\n");
    STATWORD ps;
    disable(ps);
    if(virtpage >= 4096) {
        if(source >= 0 && source <= 7) {
            if(npages >= 0 && npages <= 256) {
                if(bsm_map(currpid,virtpage,source,npages) == SYSERR) {
			kprintf("xmmap: Error occured in bsm_map\n");
			restore(ps);
			return SYSERR;
		}
                restore(ps);
                return OK;
            }
        }
    }
    kprintf("Invalid arguments to xmmap: virtpage: %d, source: %d, npages: %d\n", virtpage, source, npages);
    restore(ps);
    return SYSERR;
}



/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage)
{
    STATWORD ps;
    disable(ps);
    if( virtpage >= 4096) {
        bsm_unmap(currpid, virtpage);
        restore(ps);
        return OK;
    }
    kprintf("Invalid argument to xmunmap: virtpage: %d", virtpage);
    restore(ps);
    return SYSERR;
}
