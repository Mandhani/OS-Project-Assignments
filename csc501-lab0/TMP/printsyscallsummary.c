#include<stdio.h>
#include<conf.h>
#include<kernel.h>
#include<proc.h>
#include<lab0.h>

bool trace = false;
/*struct tracedata {
	int count = 0;
	int avgtime = 0;
};*/
//struct tracedata tracedatalist[NPROC][27];

const char* getSysCallName(enum syscallnames syscallname) {
	switch (syscallname) {
		case KFREEMEM: return "freemem";
		case KCHPRIO: return "chprio";
		case KGETPID: return "getpid";
		case KGETPRIO: return "getprio";
		case KGETTIME: return "gettime";
		case KKILL: return "kill";
		case KRECEIVE: return "receive";
		case KRECVCLR: return "recvclr";
		case KRECVTIM: return "recvtim";
		case KRESUME: return "resume";
		case KSCOUNT: return "scount";
		case KSDELETE: return "sdelete";
		case KSEND: return "send";
		case KSETDEV: return "setnok";
		case KSETNOK: return "setnok";
		case KSCREATE: return "screate";
		case KSIGNAL: return "signal";
		case KSIGNALN: return "signaln";
		case KSLEEP: return "sleep";
		case KSLEEP10: return "sleep10";
		case KSLEEP100: return "sleep100";
		case KSLEEP1000: return "sleep1000";
		case KSRESET: return "sreset";
		case KSTACKTRACE: return "stacktrace";
		case KSUSPEND: return "suspend";
		case KUNSLEEP: return "unsleep";
		case KWAIT: return "wait";
		default: return "";
	}
}


void syscallsummary_start() {
	if(trace == true) {
		printf("Already tracing system calls!");
		return;
	}
	trace = true;
	//clearing old data
	int i=0;
	while(i<NPROC) {
		int j=KFREEMEM;
		while(j<KWAIT) {
			tracedatalist[i][j].count=0;
			tracedatalist[i][j].avgtime=0;
			j++;
		}
		i++;
	}
	//memset(tracedatalist,0,sizeof(tracedatalist));
	return;
}

void syscallsummary_stop() {
	trace = false;
	return;
}

void printsyscallsummary() {
	printf("\nvoid printsyscallsummary()\n");
	int i=0;
	while(i<NPROC) {// && 
		int j=KFREEMEM;
		bool flag = false;
		while(j<=KWAIT) {// &&
			if( tracedatalist[i][j].count != 0) {
				//first time?
				if(!flag) {
					printf("Process [pid:%d]\n",i);
				}
				flag = true;
				printf("\tSyscall: sys_%s, count: %d, average execution time: %d (ms)\n",getSysCallName(j), tracedatalist[i][j].count, tracedatalist[i][j].avgtime);
			}
			j++;
		}
		i++;
	}
	return;
}

unsigned long starttrace(int tracekey) {
	if(trace) {
		//tracedatalist[currpid][tracekey].count+=1;
	}
	return ctr1000;
}

void stoptrace(int tracekey, unsigned long starttime) {
	if(trace == true) {
                unsigned long endtime = ctr1000;
                long totaltime = endtime - starttime;
                tracedatalist[currpid][tracekey].avgtime += ((totaltime - tracedatalist[currpid][tracekey].avgtime)/(++tracedatalist[currpid][tracekey].count));
        }
}
