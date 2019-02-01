#include<stdio.h>
#include <kernel.h>
#include <proc.h>

static unsigned long *esp;
void printprocstks(int priority) {
	printf("\nvoid printprocstks(int priority)\n");
	int num = 0;
	while(num < NPROC) {
		if(proctab[num].pprio > priority && proctab[num].pstate != PRFREE) {
			printf("Process\t[%s]\n",proctab[num].pname);
			printf("\tpid: %d\n",num);
			printf("\tpriority: %d\n",proctab[num].pprio);
			printf("\tbase: 0x%08x\n",proctab[num].pbase);
			printf("\tlimit: 0x%08x\n",proctab[num].plimit);
			printf("\tlen: %d\n",proctab[num].pstklen);
			if(num == currpid) {
				asm("movl %esp,esp");
                        	printf("\tpointer: 0x%08x\n",esp);
			} else {
                        	printf("\tpointer: 0x%08x\n",proctab[num].pesp);
			}
		}
		num++;
	}
}
