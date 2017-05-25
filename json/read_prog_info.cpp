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

    int socket_descriptor; //�׽ӿ�������  
    int iter=0;  
    char buf[80];  
    int port=6789;  
  if (argc != 2){
	printf("%s  message\n",argv[0]);
	return -1;
  }
	
    struct sockaddr_in address;//��������ͨ�ŵĵ�ַ  
  
    bzero(&address,sizeof(address));  
    address.sin_family=AF_INET;  
    address.sin_addr.s_addr=inet_addr("192.168.1.50");//���ﲻһ��  
    address.sin_port=htons(port);  
  
    //����һ�� UDP socket  
  
    socket_descriptor=socket(AF_INET,SOCK_DGRAM,0);//IPV4  SOCK_DGRAM ���ݱ��׽��֣�UDPЭ�飩  
  
   
    
      
         
        /*int PASCAL FAR sendto( SOCKET s, const char FAR* buf, int len, int flags,const struct sockaddr FAR* to, int tolen);���� 
         * s��һ����ʶ�׽ӿڵ������֡��� 
         * buf���������������ݵĻ����������� 
         * len��buf�����������ݵĳ��ȡ��� 
         * flags�����÷�ʽ��־λ������ 
         * to������ѡ��ָ�룬ָ��Ŀ���׽ӿڵĵ�ַ���� 
         * tolen��to��ָ��ַ�ĳ��ȡ�   
����      */  
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

	/* ��дsockaddr_in*/
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(UDP_TEST_PORT);
	addr.sin_addr.s_addr = inet_addr(UDP_SERVER_IP);

	 struct timeval timeout={3,0};//3

	setsockopt(sockfd,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));
   	setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));

	
	bzero(buffer, sizeof(buffer));



	/* �ӱ�׼�����豸ȡ���ַ���*/
	const char *string="{ \"ordertype\": \"netinfo\" }";

	/* ���ַ������͸�server��*/
	sendto(sockfd, string, strlen(string)+1, 0, (struct sockaddr *)&addr, addr_len);

	/* ����server�˷��ص��ַ���*/
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




