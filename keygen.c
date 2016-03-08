/*
Chad Gibeaut 
Key generation program
takes int input from command line and produces key of that length
normall call is to redirect output to file for use in other code
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

int ID;
void error(const char *msg)
{
    perror(msg);
    exit(1);
}
int main(int argc, char *argv[])
{
	ID=getpid();//used in random number generation
	if (argc!=2){error("Invalid input parameters....Exiting");}//checks number of inputs, but not type
	int length=atoi(argv[1]); //converts string to int 
	int x;//index in loop
	int num=-1;//used for random number 
	for(x=0;x<length;x++)
	{
		num=random_int(); //gets random number (0-26)
		if(num==0){num=32;}//remaps 0 to a space(32)
		else{num=num+64;}//remaps remainding numbers to ascii uppercase
		printf("%c",num);
	}
	printf("\n");
	exit(0);
}

//random number function (uses procress ID to help with randomizing)
int random_int()
{
	int x=rand();
	if(ID%2==0){x=x*((ID+1)%100);} //had to make an adjustment incase ID is even
	if(ID%2!=0){x=x*((ID)%100);}
	x=abs(x%27);
	//printf("\n Random is: %d",x);
	return x;
	
}
