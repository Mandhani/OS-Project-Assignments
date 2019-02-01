/* policy.c = srpolicy*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>


extern int page_replace_policy;
extern int debug;
/*-------------------------------------------------------------------------
 * srpolicy - set page replace policy 
 *-------------------------------------------------------------------------
 */
SYSCALL srpolicy(int policy)
{
  /* sanity check ! */
	debug = 1;
	if(policy == AGING) {
		kprintf("AGING not implemented. Using SC and Returning syserr\n");
		return SYSERR;
	} else if(policy == SC) {
		page_replace_policy = SC;
		return OK;
	}
  //kprintf("To be implemented!\n");
	kprintf("srpolicy: Invalid input: %d\n",policy);
	return SYSERR;
}

/*-------------------------------------------------------------------------
 * grpolicy - get page replace policy 
 *-------------------------------------------------------------------------
 */
SYSCALL grpolicy()
{
  return page_replace_policy;
}
