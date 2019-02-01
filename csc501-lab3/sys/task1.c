#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lock.h>

#define DEFAULT_LOCK_PRIO 20

#define assert(x,error) if(!(x)){ \
            kprintf(error);\
            return;\
            }
/*----------------------------------Test---------------------------*/
void B (char *msg, int lck)
{
        int ret;
        kprintf ("  %s: to acquire lock \"%d\"\n", msg, lck);
        lock (lck, WRITE, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock \"%d\", sleep 8s\n", msg, lck);
	sleep(8);
        kprintf ("  %s: to release lock \"%d\"\n", msg, lck);
        releaseall (1, lck);
}
void A (char *msg, int lck1, int lck2)
{
        int ret;
        kprintf ("  %s: to acquire lock \"%d\"\n", msg, lck1);
        lock (lck1, WRITE, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock \"%d\"\n", msg, lck1);
        kprintf ("  %s: to acquire lock \"%d\"\n", msg, lck2);
        lock (lck2, WRITE, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock \"%d\"\n", msg, lck2);
        kprintf ("  %s: acquired locks \"%d\" \"%d\"\n", msg, lck1,lck2);
        kprintf ("  %s: to release locks \"%d\" \"%d\"\n", msg,lck1, lck2);
        releaseall (2, lck1, lck2);
}

void C (char *msg, int lck)
{
        kprintf ("  %s: to acquire lock \"%d\"\n", msg, lck);
        lock (lck, WRITE, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock \"%d\"\n", msg, lck);
        kprintf ("  %s: to release lock \"%d\"\n", msg,lck);
        releaseall (1, lck);
}

void testtask1locks()
{
        int     lck1, lck2;
        int     wr1, wr2, wr3;

        kprintf("\nTest : test the basic priority inheritence and transitive property\n");
        lck1 = lcreate();
	lck2 = lcreate();

	kprintf("Locks created: %d %d\n", lck1, lck2);
        assert (lck1 != SYSERR, "Test 3 failed");
        assert (lck2 != SYSERR, "Test 3 failed");

        wr2 = create(B, 2000, 30, "B", 2, "writer B", lck2); //Acquires lck2
        wr1 = create(A, 2000, 20, "A", 3, "writer A", lck1, lck2);//Acquires lck1 and waits on lck2
        wr3 = create(C, 2000, 40, "C", 2, "writer C", lck1);

        kprintf("-start writer B, then sleep 1s. lock : \"%d\" granted to write (prio 30)\n",lck2);
        resume(wr2);
        sleep (2);

        kprintf("-start writer A, then sleep 1s. writer A(prio 20) granted lock : \"%d\", blocked on the lock : \"%d\"\n",lck1,lck2);
        resume(wr1);
        sleep (2);

        kprintf("-start writer C, then sleep 1s. writer C(prio 40) blocked on the lock : \"%d\"\n", lck1);
        resume (wr3);
	sleep (2);

	kprintf("Checking Priorities of Writer A and B. Should be equal to C's priority.\n");
	assert (getprio(wr1) == 40, "Test 3 failed. A's priority not equal to C's\n");
	assert (getprio(wr2) == 40, "Test 3 failed. B's priority not equal to C's\n");
	kprintf("Priority of A: %d, B: %d, C: %d.\n",getprio(wr1),getprio(wr2),getprio(wr3));
	kprintf("Checked Priorities of Writer A and B. They're equal to C's priority.\n");

        sleep (4);
        kprintf ("Test 3 OK\n");
}

void Bs (char *msg, int s)
{
        int ret;
        kprintf ("  %s: to wait on sem \"%d\"\n", msg, s);
	wait(s);
        kprintf ("  %s: done waiting on sem \"%d\", sleep 8s\n", msg, s);
	sleep(8);
        kprintf ("  %s: to signal sem \"%d\"\n", msg, s);
	signal(s);
}
void As (char *msg, int s1, int s2)
{
        int ret;
        kprintf ("  %s: to wait on sem \"%d\"\n", msg, s1);
	wait(s1);
        kprintf ("  %s: done waiting on sem \"%d\"\n", msg, s1);
        kprintf ("  %s: to wait on sem \"%d\"\n", msg, s2);
	wait(s2);
        kprintf ("  %s: done waiting on sem \"%d\"\n", msg, s2);
        kprintf ("  %s: to signal sems \"%d\" \"%d\"\n", msg,s1, s2);
	signal(s1);
	signal(s2);
}

void Cs (char *msg, int s)
{
        kprintf ("  %s: to wait on sem \"%d\"\n", msg, s);
	wait(s);
        kprintf ("  %s: done waiting on sem \"%d\"\n", msg, s);
        kprintf ("  %s: to signal lock \"%d\"\n", msg,s);
	signal(s);
}

void testtask1sem()
{
        int     s1, s2;
        int     wr1, wr2, wr3;

        kprintf("\nTest : test the basic priority inheritence and transitive property\n");
        s1 = screate(1);
	s2 = screate(1);

	kprintf("Sems created: %d %d\n", s1, s2);
        assert (s1 != SYSERR, "Test 3 failed");
        assert (s2 != SYSERR, "Test 3 failed");

        wr2 = create(Bs, 2000, 30, "Bs", 2, "writer Bsem", s2); //Acquires lck2
        wr1 = create(As, 2000, 20, "As", 3, "writer Asem", s1, s2);//Acquires lck1 and waits on lck2
        wr3 = create(Cs, 2000, 40, "Cs", 2, "writer Csem", s1);

        kprintf("-start writer B, then sleep 1s. sem : \"%d\" granted to write (prio 30)\n",s2);
        resume(wr2);
        sleep (2);

        kprintf("-start writer A, then sleep 1s. writer A(prio 20) granted sem : \"%d\", blocked on the sem : \"%d\"\n",s1,s2);
        resume(wr1);
        sleep (2);

        kprintf("-start writer C, then sleep 1s. writer C(prio 40) blocked on the sem : \"%d\"\n", s1);
        resume (wr3);
	sleep (2);

	kprintf("Checking Priorities of Writer A and B. Should be equal to C's priority.\n");
	assert (getprio(wr1) == 40, "Test 3 failed. A's priority not equal to C's\n");
	assert (getprio(wr2) == 40, "Test 3 failed. B's priority not equal to C's\n");
	kprintf("Priority of A: %d, B: %d, C: %d.\n",getprio(wr1),getprio(wr2),getprio(wr3));
	kprintf("Checked Priorities of Writer A and B. They're equal to C's priority.\n");

        sleep (4);
        kprintf ("Test 3 OK\n");
}



void task1() {
	kprintf("Check Locks\n");
	testtask1locks();
	kprintf("Checked Locks\n");
	kprintf("Check Semaphores\n");
	testtask1sem();
	kprintf("Checked Semaphores\n");
}
