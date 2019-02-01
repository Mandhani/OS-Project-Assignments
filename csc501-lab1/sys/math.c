#include<math.h>
#include<stdio.h>

double expdev(double lambda) {
	double dummy;
	do {
		dummy= (double) rand() / 077777;
	} while (dummy == 0.0);
	return -log(dummy) / lambda;
}

double log(double x) {
    if(x<0) { // x is negative > NAN
        return 0.0/0.0;
    }
    if(x==0) { // x is zero > -inf
        return -(1.0/0.0);
    }
    if(x==(1.0/0.0)) { // x is inf > inf
        return x;
    }
    if(x!=x) { // x is NAN > NAN
        return x;
    }
    //mkushal: log(1+x) = x - pow(x,2)/2 + pow(x,3)/3 - .... for -1<x<1
    //     ==> log(x) = (x-1) - pow(x-1,2)/2 + pow(x-1,3)/3 - .... for 0<x<2
    //     Making x less than 2 for x>2 cases and using:
    //         log(x) = a + log(x/pow(e,a)) where a=1,2,3,...
    double e = 2.7182818284;
    int count = 0; //this is 'a'
    while(x>2){
        count++;
        x=x/e;
    }
    double xminus1 = x - 1 ;
    double answerlog = 0;
    int i=0;
    for (i=1;i<=20;i++) {
        answerlog = answerlog + (pow(xminus1,i)*pow(-1,i+1)/i);
    }
    return count + answerlog;
}

double recursivepower(double x, int y) 
{ 
	double temp; 
	if( y == 0) 
		return 1; 
	temp = recursivepower(x, y/2); 
	if (y%2 == 0) 
		return temp*temp; 
	else
		return x*temp*temp; 
}

double pow(double x, int y) {
    if (y == 0) {
        return 1;
    }
    int flag = 0;
    if(y<0) {
        flag = 1;
        y=-y;
    }
    double answer = recursivepower(x,y);
    if(flag == 1) {
        return 1/answer;
    }
    return answer;
}

