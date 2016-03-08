#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

//Starts connection, returns 0 on success
//returns -1 on error
int start_con(int portnum)
{
	int _sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
    //portno = atoi(argv[3]);
    portno=portnum;
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd < 0) {error("ERROR opening socket");return -1;}
	server=gethostbyname("localhost");
	if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        return -1;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(_sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
	{
		error("ERROR connecting");
		return -1;
	}
	
	return _sockfd;	
}

int main(int argc, char *argv[])   //program text key port#
{	
	// read in all text into buffer [70000]
	// read in all key into another buffer [70000] max
	int key_fd;
	int text_fd;
	int count=0;
	char text[70000];
	char key[70000];
	char text_char[1];
	char key_char[1];
	key_fd=open(argv[2],O_RDONLY);//opens key for readonly
	if(key_fd<0){error("Error Opening Key file...exiting");}
	text_fd=open(argv[1],O_RDONLY);//opens text file for readonly
	if(text_fd<0){error("Error Opening Key file...exiting");}
	int rv=read(text_fd,text_char,1);
	int ky=0;
	while(rv!=0)//loop till end of file
	{
		ky=read(key_fd,key_char,1);
		if(ky==0){error("Key Not Long enough...EXITING");}
		text[count]=text_char[0];
		key[count]=key_char[0];
		count++;
		rv=read(text_fd,text_char,1);
	}
	close(key_fd);
	close(text_fd);
	
	char buffer[70000];
	int n;
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       return -1;
    }
	
	
	// error if key is smaller then text
    //establish connection
	
	//send text+[42-File seperator]+key(append length)
	//receive back text (check for same length)
	//print text (will be redirected)
	
	
	
	int sockfd=start_con(atoi(argv[3]));//start conneciton on input port#
	if (sockfd<0){error("Start_Con Failed");exit(1);}
	//recieve new port number
	//close connection
	//open new connection on new port
	char new_port_char[100];
	n=read(sockfd,new_port_char,100);//receives new port number
	int new_port=atoi(new_port_char);
	//printf("New Port Number: %d\n",new_port);
	close(sockfd); //close old connection
	sockfd=start_con(new_port);//start new connection 
	//Sends text
	n = write(sockfd,text,count);
	if (n < 0){error("ERROR writing to socket");}
	 
	 text_char[0]=42;
	//Sends file seperator 
	 n = write(sockfd,text_char,1);
	if (n < 0){error("ERROR writing to socket");}
	 
	 //sends key
	n = write(sockfd,key,count);
	if (n < 0) {error("ERROR writing to socket");}
	 
	 
    bzero(buffer,count+1);
    int r=0;
	r = read(sockfd,buffer,count);
    //printf("Received Size: %d\nN size: %d\n\n",strlen(buffer),r);
	if (r < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer);
    
	close(sockfd);
    return 0;
}
