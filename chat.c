#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<errno.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<netdb.h>

void exitWithError(){
	printf("Please provide valid arguments\n");
	exit(-1);
}
void exitSysWithError(const char *call){
	fprintf(stderr, "Syscall %s failed with errno= %d: %s\n", call, errno,
	 strerror(errno));
	exit(-1);
}
int main(int argc, char** argv){

	char *type;
	int portnumber;
	char *peer;
	struct sockaddr_in ina;
	struct message_type{
	char opcode;
	short length;
	char text[80];
	}finalmsg;
	peer = NULL;
	if (argc != 4 && argc != 6){
		exitWithError();
	}
	for(int i=0; i<argc; i++){
		if(strcmp(argv[i],"--port") == 0)
			portnumber =(int)atol(argv[i+1]);
	}
	for(int i=0; i<argc; i++){
		if(strcmp(argv[i],"--peer") == 0)
			peer = argv[i+1];
	}
	for(int i=0; i<argc; i++){
		if(strcmp(argv[i],"--active") == 0)
			type = "active";
	}
	for(int i=0; i<argc; i++){
		if(strcmp(argv[i],"--passive") == 0)
			type = "passive";
	}
	ina.sin_family = AF_INET;
	ina.sin_port = htons(portnumber);
	if (peer != NULL){
		struct hostent *hp;
		hp = gethostbyname(peer);
		int r = inet_pton(AF_INET,peer, &(ina.sin_addr));
		if (r == 0){
			char *IPBuffer = inet_ntoa(*((struct in_addr*)hp->h_addr_list[0]));
			//printf("Invalid conversion from text to IP address");
			ina.sin_addr.s_addr=inet_addr(IPBuffer);
		}
		else
			ina.sin_addr.s_addr=inet_addr(peer);
	}
	else
		ina.sin_addr.s_addr=htonl(INADDR_ANY);

	int s = socket(AF_INET, SOCK_STREAM, 0);
	if(s ==-1){
		exitSysWithError("socket()");
	}
	if (type == "active"){
		int opcode;
		char name[32];
		int c = connect (s, (struct sockaddr*)&ina, sizeof(ina));
		if (c==-1)
			exitSysWithError("connect()");
		printf("Welcome to the Chat..\n");
		char buff[80] = {'\0'};
		char buffer[100] = {'\0'};
		char message[80] = {'\0'};
		char peername[20];
    		int n;
		n= 0;
		for(;;){
			bzero(buff, sizeof(buff));
			bzero(buffer, sizeof(buffer));
			if (n == 0){
				printf("%s","Enter your name>");
				fflush(stdout);
                                fgets(buff, sizeof(buff), stdin);
				strcpy(name,buff);
				name[strcspn(name,"\n")] = 0;
			}
			else{
				printf("%s>",name);
				fflush(stdout);
				fgets(buff, sizeof(buff), stdin);
			}
			if(n==0){
                                finalmsg.opcode=='1';
                                finalmsg.length = sizeof(buff) + 1;
                                strcpy(finalmsg.text, buff);
                                send(s, (void *)&finalmsg,
                                sizeof(finalmsg),0);}
                        //sprintf(buffer,"%s:%s",peername,message);
                        if (n>0 && (strstr(buff,"bye") != NULL)){
                                finalmsg.opcode == '3';
                                //send(sock, message, sizeof(message),0);}
                                 finalmsg.length = sizeof(buff) + 1;
                                strcpy(finalmsg.text, buff);
                                send(s, (void *)&finalmsg,
                                sizeof(finalmsg),0);
				//printf("Executed\n");
				}

                        if (n>0 && (strstr(buff,"bye") == NULL)){
                                finalmsg.opcode == '2';
                                //send(sock, message, sizeof(message),0);}
                                 finalmsg.length = sizeof(buff) + 1;
                                strcpy(finalmsg.text, buff);
                                send(s, (void *)&finalmsg,
                                sizeof(finalmsg),0);
				//printf("Executed this for text\n");
			}
			if (strstr(buff,"bye") != NULL) {
                                printf("Exit...\n");
                                close(s);
                                break;
                        }
			bzero(message, sizeof(message));
			int r = recv(s, (struct message_type *)&finalmsg,sizeof(finalmsg),0);
			n++;
			if (n == 1){
				strcpy(peername,finalmsg.text);
				peername[strcspn(peername,"\n")] = 0;
                                //printf("Peername:%s\n",peername);
                                printf("Peer:%s",finalmsg.text);
			}
			if(n > 1){
                               sprintf(buffer, "%s>%s", peername,
						finalmsg.text);
                               printf("%s",buffer);
			}
        		//printf("%s",message);
			//printf("%s",">");
                        //fflush(stdout);
			if (strstr(finalmsg.text,"bye") != NULL) {
                                printf("Exit...\n");
                                close(s);
                                break;
                        }
		}
	}
	if (type == "passive"){
		int opcode;
		char name[32]={'\0'};
		char peername[20] = {'\0'};
		char buffer[100] = {'\0'};
                char message[80] = {'\0'};
		int b = bind(s, (struct sockaddr *)&ina , sizeof(ina));
		if(b == -1){
			printf("binding fails\n");
			exitSysWithError("bind()");
		}
		int l = listen(s,5);
		if (l==-1)
			printf("listen fails\n");
		struct sockaddr_in cli;
		int len = sizeof(cli);
		int sock = accept(s, (struct sockaddr *)&cli , &len);
		if (sock <0)
			printf("server accept failed\n");
		printf("Welcome to the Chat..\n");
		char buff[80];
		int n;
		n=0;
		for(;;){
			bzero(buff,sizeof(buff));
			bzero(buffer, sizeof(buffer));
			int r = recv(sock,(struct message_type *)&finalmsg,sizeof(finalmsg),0);
			n++;
			if(n == 1){
                                strcpy(peername,finalmsg.text);
                                //printf("Peername:%s\n",peername);
				peername[strcspn(peername,"\n")] = 0;
				printf("Peer:%s", finalmsg.text);
                        }
			if(n>1)
				{sprintf(buffer, "%s>%s", peername,
						finalmsg.text);
				printf("%s",buffer);}
			if (strstr(finalmsg.text,"bye") != NULL) {
                                printf(" Exit...\n");
                                close(sock);
                                break;
                        }
			bzero(message,sizeof(message));
			if (n==1){
				printf("%s","Enter your Name>");
				fflush(stdout);
                                fgets(message,sizeof(message),stdin);
				strcpy(name,message);
				name[strcspn(name,"\n")] = 0;
			}
			else{
				printf("%s>",name);
				fflush(stdout);
				fgets(message,sizeof(message),stdin);
			}
			if(n==1){
				finalmsg.opcode=='1';
				finalmsg.length = sizeof(message) + 1;
				strcpy(finalmsg.text, message);
				send(sock, (void *)&finalmsg,
				sizeof(finalmsg),0);}
			//sprintf(buffer,"%s:%s",peername,message);
			if (n>1 && (strstr(message,"bye") != NULL)){
				finalmsg.opcode == '3';
				//send(sock, message, sizeof(message),0);}
				 finalmsg.length = sizeof(message) + 1;
                                strcpy(finalmsg.text, message);
                                send(sock, (void *)&finalmsg,
                                sizeof(finalmsg),0);}

			if (n>1 && (strstr(message,"bye") == NULL)){
                                finalmsg.opcode == '2';
                                //send(sock, message, sizeof(message),0);}
                    		 finalmsg.length = sizeof(message) + 1;
                                strcpy(finalmsg.text, message);
                                send(sock, (void *)&finalmsg,
                                sizeof(finalmsg),0);}
			if (strstr(message,"bye") != NULL) {
                                printf("Exit...\n");
                                close(sock);
                                break;
                        }
		
		}
	}

}

struct message_type{
                char Opcode;
                char NameLength;
                short TextLength;
                char Text[80];
                char Name[20];
        }finalmsg;

