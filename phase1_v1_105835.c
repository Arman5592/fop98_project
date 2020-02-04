//Mohammad Arman Soleimani 98105835

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ctype.h>
#include <time.h>
#include "qTjson.c"

#define MAX 80
#define PORT 12345
#define SA struct sockaddr

int client_socket;
char auth_token[33];
char current_channel_name[31];
//auth_token[32]='\0'; //gives an error





void initialize(){

	struct sockaddr_in servaddr, cli;

	WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        printf("WSAStartup failed with error: %d\n", err);
        return 1;
    }

	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1) {
		printf("Socket creation failed...\n");
		exit(0);
	}
	else
		printf("loading...\n");

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("Connection to the server failed...\n");
		exit(0);
	}
	else
		printf("loading......\n");

}//void initialize

void account_menu(){
	system("cls");
	system("color B0");
	puts("Hi!");
	puts("A:A: , L: , R: , ... specifies your current menu.");

	puts("A:type L to login , R to register .");
	char entrance;
	scanf("%c",&entrance);
	//scanf("%c",&entrance);

	if(entrance == 'L'){
		login();
	}
	else if(entrance == 'R'){
		register_user();
	}
	//else if(entrance == 'E'){
	//	return ;
	//}
	else{
		account_menu();
	}
}//VOID ACCOUNT MENU

void main_menu(){
	system("cls");
	system("color 30");
	puts("L: you are now logged in.");
	printf("L: for the record, your auth token is:%s .\n",auth_token);

	while(true){
	puts("M: type C to create a channel , J to join one , X to search for a user , L to log out or Q for a quote.");

	char entrance;
	scanf("%c",&entrance);
	scanf("%c",&entrance);


	if(entrance == 'C'){
		char buffer[MAX];
		char channel_name[31];
		memset(buffer, 0, sizeof(buffer));
		memset(channel_name, 0, sizeof(channel_name));

		puts("M: enter a name for your new channel , max 30 chars , no spaces.");
		scanf("%s",channel_name);

		strcat(buffer,"create channel ");
		strcat(buffer,channel_name);
		strcat(buffer,", ");
		strcat(buffer,auth_token);
		strcat(buffer,"\n");

		initialize();
		send(client_socket,buffer , strlen(buffer), 0);

		memset(buffer, 0, sizeof(buffer));
		recv(client_socket, buffer, sizeof(buffer), 0);
		closesocket(client_socket);

		int type;
		char * result = qTjson_parseTwin(buffer,&type);

		printf("M:%s\n",result);
		if(type==1){
			puts("M: channel joined!");
			for(int i=0;i<31;i++) current_channel_name[i] = channel_name[i];
			chat_menu();
		}



	}

	else if(entrance == 'J'){
		char buffer[MAX];
		char channel_name[31];
		memset(buffer, 0, sizeof(buffer));
		memset(channel_name, 0, sizeof(channel_name));

		puts("M: enter a valid channel name.");
		scanf("%s",channel_name);

		strcat(buffer,"join channel ");
		strcat(buffer,channel_name);
		strcat(buffer, ", ");
		strcat(buffer, auth_token);
		strcat(buffer, "\n");

		initialize();
		send(client_socket,buffer , strlen(buffer), 0);

		memset(buffer, 0, sizeof(buffer));
		recv(client_socket, buffer, sizeof(buffer), 0);
		closesocket(client_socket);

		int type;
		char * result = qTjson_parseTwin(buffer,&type);

		if(type==1){
			printf("M:%s\n",result);
			for(int i=0;i<31;i++) current_channel_name[i] = channel_name[i];
			chat_menu();
		}
		else if(type==0){
			printf("M:%s\n",result);
			//printf("%s\n",buffer);
		}

	}
	else if(entrance == 'X'){
		char buffer[8000];
		char query[32];
		char medium;
		memset(buffer, 0, sizeof(buffer));
		puts("M: enter your search query... (max 30 chars, no space)");
		scanf("%c",&medium);
		scanf("%[^\n]s",query);


		strcat(buffer,"searchusr ");
		strcat(buffer,query);
		strcat(buffer," ");
		strcat(buffer,auth_token);
		strcat(buffer,"\n");

		initialize();
		send(client_socket,buffer , strlen(buffer), 0);

		memset(buffer, 0, sizeof(buffer));
		recv(client_socket, buffer, sizeof(buffer), 0);

		closesocket(client_socket);
		puts("M:online usernames that include your query:");
		qTjson_printMemList(buffer);
		puts("");
	}

	else if(entrance == 'L'){
		char buffer[MAX];
		memset(buffer, 0, sizeof(buffer));

		strcat(buffer,"logout ");
		strcat(buffer,auth_token);
		strcat(buffer,"\n");

		initialize();
		send(client_socket,buffer , strlen(buffer), 0);

		memset(buffer, 0, sizeof(buffer));
		recv(client_socket, buffer, sizeof(buffer), 0);
		closesocket(client_socket);

		int type;
		char * result = qTjson_parseTwin(buffer,&type);

		if(type==1){
			puts("M: logged out successfully!");
			account_menu();
		}
	}

	else if(entrance == 'Q'){
		clock_t time;
		int remainder;
		time = clock();
		remainder = time % 6;
		if(remainder==0){
			puts("\n* It is ordinary to love the beautiful ,");
			puts("  but it is beautiful to love the ordinary. *");
			puts(" -Unknown\n");
		}
		else if(remainder==1){
			puts("\n* If you wont go for that seemingly impossible gap,");
			puts("  then you might as well go home. *");
			puts(" -Sebastian Vettel\n");
		}
		else if(remainder==2){
			puts("\n* When you get to the end of your rope ,");
			puts("  tie a knot and hang on.");
			puts(" -Theodore Roosevelt\n");
		}
		else if(remainder==3){
			puts("\n* In war , truth is the first casualty. *");
			puts(" -Aeschylus\n");
		}
		else if(remainder==4){
			puts("\n* Courage is fear holding on a minute longer. *");
			puts(" -George S. Patton\n");
		}
		else{
			puts("\n* An eye for an eye makes the whole world blind. *");
			puts(" -Gandhi\n");
		}
	}


	}//while true
}//VOID MAIN MENU

void chat_menu(){
	system("cls");
	system("color 90");
	while(true){
	printf("C: you are in %s.\n",current_channel_name);
	puts("C: type S to send a message , R to refresh ,X to search messages , C to view members list and L to leave.");

	char entrance;
	scanf("%c",&entrance);
	scanf("%c",&entrance);

	if(entrance == 'S'){
		//send
		int flag=1;
		char medium;
		char buffer[MAX];
		char message[41];
		memset(buffer, 0, sizeof(buffer));
		memset(message, 0, sizeof(message));

		puts("C: enter your message... (max 40 chars)");
		scanf("%c",&medium);
		scanf("%[^\n]s",message);

		strcat(buffer,"send ");
		strcat(buffer,message);
		strcat(buffer,", ");
		strcat(buffer,auth_token);
		strcat(buffer,"\n");

		printf("%s\n",buffer);
		initialize();
		send(client_socket,buffer , strlen(buffer), 0);

		memset(buffer, 0, sizeof(buffer));
		recv(client_socket, buffer, sizeof(buffer), 0);
		closesocket(client_socket);


	}
	else if(entrance == 'R'){
		//refresh
		refresh();
	}
	else if(entrance == 'C'){
		//members list
		members_list();
	}
	else if(entrance == 'L'){
		//leave
		char buffer[MAX];
		memset(buffer, 0, sizeof(buffer));


		strcat(buffer,"leave ");
		strcat(buffer,auth_token);
		strcat(buffer,"\n");

		initialize();
		send(client_socket,buffer , strlen(buffer), 0);

		memset(buffer, 0, sizeof(buffer));
		recv(client_socket, buffer, sizeof(buffer), 0);

		closesocket(client_socket);

		int type;
		char * result = qTjson_parseTwin(buffer,&type);

		if(type==1) printf("C: you left %s\n",current_channel_name);

		main_menu();
	}
	else if(entrance == 'X'){
		//search messages
		char buffer[8000];
		char query[42];
		char medium;
		memset(buffer, 0, sizeof(buffer));
		puts("C: enter your search query... (max 40 chars, no space)");
		scanf("%c",&medium);
		scanf("%[^\n]s",query);


		strcat(buffer,"searchmsg ");
		strcat(buffer,query);
		strcat(buffer," ");
		strcat(buffer,auth_token);
		strcat(buffer,"\n");

		initialize();
		send(client_socket,buffer , strlen(buffer), 0);

		memset(buffer, 0, sizeof(buffer));
		recv(client_socket, buffer, sizeof(buffer), 0);

		closesocket(client_socket);
		puts("C: messages that include your query:");
		qTjson_printRefresh(buffer);
	}

	}
}//VOID CHAT MENU

void refresh(){
	char buffer[50];
	char recv_buffer[8000];
	memset(buffer, 0, sizeof(buffer));
	memset(recv_buffer, 0, sizeof(recv_buffer));

	strcat(buffer,"refresh ");
	strcat(buffer,auth_token);
	strcat(buffer,"\n");

	initialize();
	send(client_socket,buffer , strlen(buffer), 0);

	recv(client_socket, recv_buffer, sizeof(recv_buffer), 0);
	closesocket(client_socket);

	qTjson_printRefresh(recv_buffer);


}// VOID REFRESH

void members_list(){
	char buffer[200];
	memset(buffer, 0, sizeof(buffer));

	strcat(buffer,"channel members ");
	strcat(buffer,auth_token);
	strcat(buffer,"\n");

	initialize();
	send(client_socket,buffer , strlen(buffer), 0);

	memset(buffer, 0, sizeof(buffer));
	recv(client_socket, buffer, sizeof(buffer), 0);
	closesocket(client_socket);

	//printf("sv:%s\n",buffer);

	printf("C: members in %s :",current_channel_name);

	qTjson_printMemList(buffer);
	puts(".");
}// VOID MEMBERS LIST

void register_user() {
	char buffer[MAX];
	char username[30];
	char password[30];

	memset(buffer, 0, sizeof(buffer));
	memset(username, 0, sizeof(username));
	memset(password, 0, sizeof(password));

	puts("R: Enter a username , max 30 chars , no spaces");
	scanf("%s",username);
	puts("R: Enter a password , max 30 chars , no spaces");
	scanf("%s",password);

	strcat(buffer,"register ");
	strcat(buffer,username);
	strcat(buffer,", ");
	strcat(buffer,password);
	strcat(buffer,"\n");

	initialize();
	send(client_socket,buffer , strlen(buffer), 0);

	memset(buffer, 0, sizeof(buffer));
	recv(client_socket, buffer, sizeof(buffer), 0);
	closesocket(client_socket);

	int type;
	char * result = qTjson_parseTwin(buffer,&type);


	if(type==0){
		printf("Error:%s\n",result);
		account_menu();
		return;
	}

	if(type==1) printf("R:%s\n",result);
	account_menu();

}//VOID REGISTER USER

void login(){
	char buffer[MAX];
	char username[31];
	char password[31];

	memset(buffer, 0, sizeof(buffer));
	memset(username, 0, sizeof(username));
	memset(password, 0, sizeof(password));

	puts("L:Enter your username...");
	scanf("%s",username);
	puts("L:Enter your password...");
	scanf("%s",password);

	strcat(buffer,"login ");
	strcat(buffer,username);
	strcat(buffer,", ");
	strcat(buffer,password);
	strcat(buffer,"\n");

	initialize();
	send(client_socket,buffer , strlen(buffer), 0);

	memset(buffer, 0, sizeof(buffer));
	recv(client_socket, buffer, sizeof(buffer), 0);
	closesocket(client_socket);

	int type;
	char * result = qTjson_parseTwin(buffer,&type);

	if(type==0){
		printf("L:Error:%s\n",result);

		//printf("server:%s\n",buffer);

		account_menu();
		return;
	}

	for(int i=31;i<63;i++){
		auth_token[i-31]=buffer[i];
	}
	auth_token[32]='\0';

	

	main_menu();

}//VOID LOGIN

int main()
{
	char language;

	/*while(true){
		puts("baraye finglish F ra type konid ,");
		puts("type E for English.");
		scanf("%c",&language);
		if(language == 'E' || language =='F') break;
	}
	if(language == 'E'){
		//strings are set to English
	}
	if(language == 'F'){
		//strings are set to Finglish
	}*/





	//initialize();


	account_menu();



	// Close the socket
	closesocket(client_socket);
	return 0;
}//INT MAIN
