#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

char return_text[70000];



char* itoa(int i, char b[])
{
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do
	{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }
	while(shifter);
    *p = '\0';
    do
	{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }
	while(i);
    return b;
}

int enc(char* txt,char* keys,int num)
{
		int a=0,b=0;
		int x=0;
		
		for(x=0;x<num;x++)
		{
			a=(int) txt[x];
			b=(int) keys[x];
			
			if(a==32){a=0;}
			else{a-=64;}
			if(b==32){b=0;}
			else{b-=64;}
			a-=b;
			if(a<0){a+=27;}
			a=a%27;
			if(a==0){a=32;}
			else{a+=64;}
			if((a>90 || a<65) && a!=32){continue;}
			return_text[x]=a;
		}
	return 1;
	
}



int start_list(int portnum)
{
	int _sockfd;
	int portno;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
 
     
     _sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (_sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     //portno = atoi(argv[1]);
     portno=portnum;
	 serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(_sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     
	 listen(_sockfd,5);
     clilen = sizeof(cli_addr);
	 //This is where it waits for connection
	return _sockfd;
}

int main(int argc, char *argv[])
{	
	
	int max_con=5;
     int n;
     char buffer[140000];
	 char text[70000];
	 char key[70000];
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
	 int mainPortNum=atoi(argv[1]);//converts input portnumber into int
	 int running[max_con];//array for PID of concurent processes
	 int numrunning=0;//number of kids running around
	int index=0;//used in for loops
	int cur_index=0;//used for current index of child, used for calculating new portnumber
	for(index=0;index<max_con;index++){running[index]=0;}//zeros out array	
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	int sockfd=start_list(mainPortNum);
	int endID=0;
	int last_exit=-100;
	while(1)
	 {
		 
		//check on children and update array
		for(index=0;index<max_con;index++)
		{
			if(running[index]!=0)//found a processes
			{
				//check to see if its still running
				endID = waitpid(running[index], &last_exit, WNOHANG|WUNTRACED);
				if(endID==0){continue;}//if so, back to checking array
				if(endID==running[index])//if dead, remove from array and dec numrunning
				{
					running[index]=0;
					numrunning--;
					//printf("My Kid is Dead!!!\n");fflush(NULL);
				}
			}
			
		}
		while(numrunning<max_con)	//if max number is running, it just skips waiting for new conneciton
								//this will allow it to check on kids again without blocking
		{
			
			int newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
			if (newsockfd < 0) {error("ERROR on accept");}
			//new connection established
			//Fork here
			for(index=0;index<max_con;index++)
			{
				if(running[index]==0)
				{
					cur_index=index;
					break;
				}
			}
			int spawnpid=fork();
			int newnum;
			char port_char[100];
			switch(spawnpid)
			{
				case -1:	perror("Fork Failed....");
							exit(1);
							break;
				case 0:	
							//char port_char[100];
							newnum=mainPortNum+1+cur_index;
							itoa(newnum,port_char);
							n=write(newsockfd,port_char,strlen(port_char));
							close(newsockfd);//close old socket
							int child_socket=start_list(newnum);//start new connection on new port#
							int newsockfd = accept(child_socket,(struct sockaddr *) &cli_addr, &clilen);
							if (newsockfd < 0) {error("ERROR on accept");}
							//printf("New Connection Setup on: %d\n",newnum);fflush(NULL);
								//receive data
								//encrypt
								//send data back
								//close new
								
								
							bzero(buffer,140000);
							char buffer1[256];
							bzero(buffer1,256);
							 n = read(newsockfd,buffer1,256);
							 int total=0;
							 //printf("N is: %d\nBuffer1: %s\n",n,buffer1);fflush(NULL);
							 strcat(buffer,buffer1);
							 while(n==256)
							 {
								bzero(buffer1,256); 
								total+=256;
								 n = read(newsockfd,buffer1,256);
								 strcat(buffer,buffer1);
							 }
							 total+=n;
							 
							 //printf("Test Number is: %d\n",total);
							 if (n < 0) error("ERROR reading from socket");
							 //printf("Size: %d\n",total);
							 
							 int index=0;
							 int count1=0;
							 int count2=0;
							 //printf("Buffer: %s\n",buffer);fflush(NULL);
							 for(index=0;index<((total-1)/2);index++)
							 {
								text[index]=buffer[index];
								key[index]=buffer[index+(total+1)/2];
							 }
							 total=(total+1)/2;
							 
							 enc(text,key,total);
							 //printf("D-Text: %s\n",text);fflush(NULL);
							 //printf("D-Key: %s\n",key);fflush(NULL);
							 //printf("D-Return Text: %s\n",return_text);fflush(NULL);
							 
							 n = write(newsockfd,return_text,total);
							 if (n < 0) error("ERROR writing to socket");	
							exit(1);
				default:	//Parent
							//Check for first index that is free
							running[cur_index]=spawnpid;
							numrunning++;
							break;//go back into loop
								
			}//end switch
		}//end while(numrunning)
	 }//end while(1)
	 
 
	 
	 
	 /*
     
     close(newsockfd);
     
	 
	while(1){}  //used to prevent exiting for now (code should stay running for more connections)
	 return 0; 
*/
 }
