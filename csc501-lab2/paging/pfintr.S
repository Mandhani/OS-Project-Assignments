    	   .text
pferrcode: .long 0
           .globl  pfintr,pferrcode 
pfintr:

    popl pferrcode		//get error code
    pushfl			//push flags
    cli				//clear interrupts
    pushal			//push all registers
    call pfint			//load page
    popal			//pop to ll registers
    popfl			//pop the flags
    iret			//return
