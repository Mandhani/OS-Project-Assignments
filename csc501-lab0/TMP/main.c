/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
int prX;
void halt();
prch(c)
char c;
{
	int i;
	sleep(5);	
}

int main()
{
	kprintf("\n\nHello World, Xinu lives\n\n");
	kprintf("\n\nzfunction output: %x\n",zfunction(0xaabbccdd));
	printsegaddress();
	//asm("movl %esp,esp");
	//printf("Before printos: 0x%08x", esp);
	printtos();
	printprocstks(-1);
	kprintf("Task 5 (printsyscallsummary)\n");
	syscallsummary_start();        
	resume(prX = create(prch,2000,20,"proc X",1,'A'));
	sleep(10);
	syscallsummary_stop();
	printsyscallsummary();
	/*syscallsummary_start();
	sleep(0);
	getpid();
	getprio(currpid);
	stacktrace(currpid);
	sleep(3);
	sleep(2);
	syscallsummary_stop();
	printsyscallsummary();*/
	return 0;
}
