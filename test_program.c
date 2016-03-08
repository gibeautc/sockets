
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int ID;
char command[2048];//Max size of input
int main()
{
	int a,b;
	a=4;
	b=23;
	a=a-b;
	if(a<1){a+=25;}
	int c=a%26;
	printf("Output: %d\n\n\n",c);
return 0;
}//end main