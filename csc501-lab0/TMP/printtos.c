#include<stdio.h>
#include<kernel.h>
#include<proc.h>

static unsigned long *ebp,*esp;

void printtos() {
        asm("movl %esp,esp");
	asm("movl %ebp,ebp");
	printf("\nvoid printtos()\n");
	printf("Before [0x%08x]: 0x%08x\n",ebp+1,*(ebp+1));
	//printf("From Table: Base [0x%08x], Top [0x%08x]\nEBP Now: [0x%08x]: 0x%08x\n",proctab[currpid].pbase,proctab[currpid].pesp,ebp,*ebp);
	printf("After [0x%08x]: 0x%08x\n",esp,*esp);
	int n=1;
	while(n < 5 && ebp >= (esp+n)) {
		printf("\tElement[0x%08x]: 0x%08x\n",esp+n,*(esp+n));
		n++;
	}
}
