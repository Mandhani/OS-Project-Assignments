#include<kernel.h>
#include<proc.h>
typedef enum { false, true } bool;

struct	tracedata {
	int count;
	int avgtime;
};

typedef	enum	syscallnames{KFREEMEM,KCHPRIO,KGETPID,KGETPRIO,KGETTIME,KKILL,KRECEIVE,KRECVCLR,KRECVTIM,KRESUME,KSCOUNT,KSDELETE,KSEND,KSETDEV,KSETNOK,KSCREATE,KSIGNAL,KSIGNALN,KSLEEP,KSLEEP10,KSLEEP100,KSLEEP1000,KSRESET,KSTACKTRACE,KSUSPEND,KUNSLEEP,KWAIT} syscallnames;

extern  long    zfunction(long param);
extern	void	printsegaddress();
extern	void	printtos();
extern	void	printprocstks(int priority);
struct	tracedata	tracedatalist[NPROC][27];
extern	bool	trace;
extern	int	ctr1000;
extern	void	printsyscallsummary();
extern	void	syscallsummary_start();
extern	void	syscallsummary_stop();
extern	unsigned long	starttrace(int tracekey);
extern	void	stoptrace(int tracekey, unsigned long starttime);
