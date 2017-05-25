#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>


#include "rapidjson/reader.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"


using namespace std;
using namespace rapidjson;

#define	UDP_TEST_PORT		61000
#define UDP_SERVER_IP 		"127.0.0.1"


#if 0

int   read_prog_info(  string file_name ,  string  &prog_name , string   &release_info )
{

	void *shm = NULL;
	struct shared_info *shared;
	int shmid;

	string  path_name = "/etc";

	path   path_file_name  = path_name;

	cout <<"path_file_name"<<path_file_name<<endl;
	
	if (exists(path_file_name)){
	

	key_t key=ftok(path_file_name.c_str(),1);
	
	shmid = shmget(key, sizeof(struct shared_info), 0666|IPC_CREAT);
	if(shmid == -1)
	{
		fprintf(stderr, "shmget failed\n");
		exit(EXIT_FAILURE);
	}

	shm = shmat(shmid, 0, 0);
	if(shm == (void*)-1)
	{
		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}
	printf("\nMemory attached at %X\n", (int)shm);

	shared = (struct shared_info*)shm;

	printf("prog_name=%s",shared->prog_name);

	prog_name = shared->prog_name;
	release_info = shared->prog_name;

	if(shmdt(shm) == -1)
	{
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}

	if(shmctl(shmid, IPC_RMID, 0) == -1)
	{
		fprintf(stderr, "shmctl(IPC_RMID) failed\n");
		exit(EXIT_FAILURE);
	}

		return 0;

	}else  {

		return -1;


	}

	
}


#endif 


int   read_prog_info(  string file_name ,  string  &prog_name , string   &release_info )
{

#if 0

    int socket_descriptor; //套接口描述字  
    int iter=0;  
    char buf[80];  
    int port=6789;  
  if (argc != 2){
	printf("%s  message\n",argv[0]);
	return -1;
  }
	
    struct sockaddr_in address;//处理网络通信的地址  
  
    bzero(&address,sizeof(address));  
    address.sin_family=AF_INET;  
    address.sin_addr.s_addr=inet_addr("192.168.1.50");//这里不一样  
    address.sin_port=htons(port);  
  
    //创建一个 UDP socket  
  
    socket_descriptor=socket(AF_INET,SOCK_DGRAM,0);//IPV4  SOCK_DGRAM 数据报套接字（UDP协议）  
  
   
    
      
         
        /*int PASCAL FAR sendto( SOCKET s, const char FAR* buf, int len, int flags,const struct sockaddr FAR* to, int tolen);　　 
         * s：一个标识套接口的描述字。　 
         * buf：包含待发送数据的缓冲区。　　 
         * len：buf缓冲区中数据的长度。　 
         * flags：调用方式标志位。　　 
         * to：（可选）指针，指向目的套接口的地址。　 
         * tolen：to所指地址的长度。   
　　      */  
    sendto(socket_descriptor,argv[1],strlen(argv[1])+1,0,(struct sockaddr *)&address,sizeof(address));  

 
    close(socket_descriptor);  

#endif 


	return 0;

}




int   read_prog_info(   string   &release_info )
{
	struct sockaddr_in addr;
	int sockfd, len = 0;	
	int addr_len = sizeof(struct sockaddr_in);		
	char buffer[256];		
	
	
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	/* 填写sockaddr_in*/
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(UDP_TEST_PORT);
	addr.sin_addr.s_addr = inet_addr(UDP_SERVER_IP);

	 struct timeval timeout={3,0};//3

	setsockopt(sockfd,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));
   	setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));

	
	bzero(buffer, sizeof(buffer));



	/* 从标准输入设备取得字符串*/
	const char *string="{ \"ordertype\": \"netinfo\" }";

	/* 将字符串传送给server端*/
	sendto(sockfd, string, strlen(string)+1, 0, (struct sockaddr *)&addr, addr_len);

	/* 接收server端返回的字符串*/
	len = recvfrom(sockfd, buffer, sizeof(buffer), 0,  (struct sockaddr *)&addr, (socklen_t*)&addr_len);
	

	  Document d;
	  d.Parse(buffer);
	if (d.IsObject()){

	 if (  d.HasMember("ver")  ){
		 release_info = d["ver"].GetString();
	 	}

	}else {
			
		release_info="not exist";
	}

	return 0;
}




