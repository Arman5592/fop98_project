//phase2 new appro

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "cJSON.h"
#include "cJSON_Utils.h"
#include "cJSON.c"
#include <dir.h>
#include <process.h>
#include "qTjson.c"

#define MAX 100
#define PORT 12345
#define SA struct sockaddr

cJSON *current_users;
cJSON *current_tokens;
cJSON *tokens_username;
cJSON *tokens_channel;
cJSON *tokens_msg_cnt;

char token[33];
char users_username[31];

int online_user_cnt = 0;
int client_socket,server_socket;


void response(char buffer[]){
    printf("server:%s\n",buffer);
    send(client_socket,buffer,strlen(buffer),0);
}


void initialize()
{
    closesocket(client_socket);
    closesocket(server_socket);
    struct sockaddr_in server, client;
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
// Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h
    wVersionRequested = MAKEWORD(2, 2);
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        // Tell the user that we could not find a usable Winsock DLL.
        printf("WSAStartup failed with error: %d", err);
        return;
    }
    // Create and verify socket
    server_socket = socket(AF_INET, SOCK_STREAM, 6);
if (server_socket == INVALID_SOCKET)
        wprintf(L"socket function failed with error = %d", WSAGetLastError() );
    else
        printf("Socket successfully created.");

    // Assign IP and port
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);
    // Bind newly created socket to given IP and verify
    if ((bind(server_socket, (SA *)&server, sizeof(server))) != 0)
    {
        printf("Socket binding failed.");
        exit(0);
    }
    else
        printf("Socket successfully bound.");

    // Now server is ready to listen and verify
    if ((listen(server_socket, 5)) != 0)
    {
        printf("Listen failed.");
        exit(0);
    }
    else
        printf("Server listening.");

    // Accept the data packet from client and verify
    int len = sizeof(client);
    client_socket = accept(server_socket, (SA *)&client, &len);
    if (client_socket < 0)
    {
        printf("Server accceptance failed.");
        exit(0);
    }
    else
        printf("Server acccepted the client.\n");
}

void read_command(){
    char buffer[MAX];
    char command[30];

    while(1){

        initialize();

        memset(buffer,0,sizeof(buffer));

        recv(client_socket,buffer,sizeof(buffer),0);

        sscanf(buffer,"%s",command);

        printf("client:%s\n",buffer);

        if(strcmp(command,"register")==0) {
            register_user(buffer);
        }
        if(strcmp(command,"login")==0) {
            login_user(buffer);
        }
        if(strcmp(command,"create")==0) {
            create_channel(buffer);
        }
        if(strcmp(command,"join")==0) {
            join_channel(buffer);
        }
        if(strcmp(command,"send")==0) {
            send_msg(buffer);
        }
        if(strcmp(command,"refresh")==0) {
            refresh(buffer);
        }
        if(strcmp(command,"channel")==0) {
            members_list(buffer);
        }
        if(strcmp(command,"leave")==0) {
            leave(buffer);
        }
        if(strcmp(command,"logout")==0) {
            logout(buffer);
        }
        if(strcmp(command,"show")==0) {
            char *output_str = cJSON_PrintUnformatted(tokens_channel);
            printf("%s\n",output_str);

            output_str = cJSON_PrintUnformatted(tokens_username);
            printf("%s\n",output_str);

            output_str = cJSON_PrintUnformatted(current_tokens);
            printf("%s\n",output_str);

            output_str = cJSON_PrintUnformatted(current_users);
            printf("%s\n",output_str);
        }

    }
}

void register_user(char buffer[]){
    char username[31];
    char password[31];
    char address[56];

    char *output_str;
    char *save_str;

    sscanf(buffer,"%*s %s %s",username,password);
    for(int i=30;i>=0;i--){
        if(username[i]==','){
            username[i] = '\0';
            break;
        }
    }

    sprintf(address,"users/%s.json",username);

    //age vojod dasht ke hichi , age vojod nadasht misazim
    FILE *registry;
    if((registry = fopen(address,"r")) != NULL){
        output_str = qTjson_twinMessage("type","Error","content","User Already Exists");
        fclose(registry);
    }
    else{

        output_str = qTjson_twinMessage("type","Success","content","User Created Successfully");
        //bayad besazim!
        save_str = qTjson_twinMessage("username",username,"password",password);

        registry = fopen(address,"w");
        fprintf(registry,"%s",save_str);
        fclose(registry);

    }

    response(output_str);

}

void login_user(char buffer[]){
    char username[31];
    char password[31];
    char address[56];
    char content[100];
    char *output_str;

    //bayad tahe file e user,login status bashe!
    sscanf(buffer,"%*s %s %s",username,password);
    for(int i=30;i>=0;i--){
        if(username[i]==','){
            username[i] = '\0';
            break;
        }
    }

    sprintf(address,"users/%s.json",username);
    //inja mirim tush,agge login bod error midim, agge pw ghalat bod error midim
    FILE *registry;


    if((registry = fopen(address,"r")) == NULL){
        //yani user vojod nadare
        output_str = qTjson_twinMessage("type","Error","content","User Doesnt Exist");
        response(output_str);//ino bayad  bedim be client
        fclose(registry);
        return;
    }

    fscanf(registry,"%s",content);
    fclose(registry);
    //printf("%s\n",content);

    cJSON *tmp_un_j = cJSON_CreateObject();


    int type;
    char *password_correct = qTjson_parseTwin(content,&type);
    for(int i=0;password_correct[i]!=0;i++){
        if(password_correct[i]==' ') password_correct[i]=0;
    }
    char *tmp_un;
    char tmp_un_2[31]="\"";

    strcat(tmp_un_2,username);
    strcat(tmp_un_2,"\"");


    for(int i=0;i<online_user_cnt;i++){
        tmp_un_j = cJSON_GetArrayItem(current_users,i);
        tmp_un = cJSON_PrintUnformatted(tmp_un_j);
        if(strcmp(tmp_un_2,tmp_un)==0){
            //yani online e
            output_str = qTjson_twinMessage("type","Error","content","User Already Logged In");
            response(output_str);//ino bayad  bedim be client
            return;
        }
    }


    if(strcmp(password_correct,password)!=0){
        output_str = qTjson_twinMessage("type","Error","content","Wrong Password");
        printf("%sa\n",password);
        printf("%sa\n",password_correct);
        response(output_str);//ino bayad  bedim be client
        return;
    }
    //yani mikhad login beshe inja
    make_auth_token();

    //bayad  be karbar auth token ro bedim va isLoggedIn ro bokonim 1
    output_str = qTjson_twinMessage("type","AuthToken","content",token);
    response(output_str);//ino bayad  bedim be client

    //alan user login shode
    cJSON_AddItemToArray(current_users,cJSON_CreateString(username));
    output_str = cJSON_PrintUnformatted(current_users);

    cJSON_AddItemToArray(current_tokens,cJSON_CreateString(token));
    output_str = cJSON_PrintUnformatted(current_tokens);

    cJSON_AddItemToObject(tokens_username,token,cJSON_CreateString(username));
    output_str = cJSON_PrintUnformatted(tokens_username);

    online_user_cnt++;
}

void create_channel(char buffer[]){
    //inja taraf miad ye channele jadid dorst mikone,
    //vaghti channelo sakht bayad current channelesh beshe esme channel
    char channel_name[31];
    char auth_token[33];
    FILE *registry;
    char address[56];
    sscanf(buffer,"%*s %*s %s %s",channel_name,auth_token);

    for(int i=strlen(channel_name)+1;i>=0;i--){
        if(channel_name[i]==','){
            channel_name[i] = '\0';
            break;
        }
    }
    sprintf(address,"channels/%s.json",channel_name);

    char *output_str;//chizi ke midim behesh

    cJSON *content;
    content = cJSON_CreateObject();
    char *content_str;//chizi ke mirizim tu channel

    char *tmp_au;
    char tmp_au_2[36]="\"";

    strcat(tmp_au_2,auth_token);
    strcat(tmp_au_2,"\"");
    cJSON *tmp_au_j = cJSON_CreateObject();

    int tmp_flag=0;
    for(int i=0;i<online_user_cnt;i++){
        tmp_au_j = cJSON_GetArrayItem(current_tokens,i);
        tmp_au = cJSON_PrintUnformatted(tmp_au_j);
        if(strcmp(tmp_au_2,tmp_au)==0){
            //yani hast
            tmp_flag=1;
            break;
        }
    }
    if(tmp_flag == 0){
        output_str = qTjson_twinMessage("type","Error","content","Wrong Auth Token");
        response(output_str);//ino bayad  bedim be client
        return;
    }

    //agge channel vojod dasht bayad error bedim
    if((registry = fopen(address,"r")) != NULL){
        //channel vojod dare!
        output_str = qTjson_twinMessage("type","Error","content","Channel Already Exists");
        response(output_str);// be client
        fclose(registry);
        return;
    }

    //ta inja channel vojod nadare va token ham ok e pas mitonim channel ro dorost konim
    registry = fopen(address,"w");
    //in miad channel ro dorost mikone
    output_str = qTjson_twinMessage("type","Success","content","Channel Created Successfully");
    response(output_str);// be client


    //users usrname bayad malom beshe az authtoken
    cJSON *un;

    un = cJSON_GetObjectItemCaseSensitive(tokens_username,auth_token);
    char *un_str_tmp = cJSON_Print(un);
    char un_str[31];
    for(int i=1; i<strlen(un_str_tmp)-1;i++){
        un_str[i-1] = un_str_tmp[i];
    }
    un_str[strlen(un_str_tmp)-2]=0;

    char tmp_msg[51];
    sprintf(tmp_msg,"%s created %s.",un_str,channel_name);

    content_str = qTjson_twinMessage("sender","server","content",tmp_msg);
    fprintf(registry,"%s\n",content_str);//message e sakhte shodane server



    for(int i=0;i<51;i++) tmp_msg[i] = 0;

    sprintf(tmp_msg,"%s joined.",un_str);
    content_str = qTjson_twinMessage("sender","server","content",tmp_msg);

    fprintf(registry,"%s\n",content_str);//msg e avalin joine user be kanal

    fclose(registry);

    cJSON_AddItemToObject(tokens_channel,auth_token,cJSON_CreateString(channel_name));
    cJSON_AddItemToObject(tokens_msg_cnt,auth_token,cJSON_CreateNumber(0));



}

void join_channel(char buffer[]){
    //bayad ye karbar betone join bede be channel
    //age channel nabod error bedim
    //age channel bod join beshe va esme channel biad tu stringe globalema
    //va msg e "joined" barash biad
    char channel_name[31];
    char auth_token[33];
    FILE *registry;
    char address[56];
    sscanf(buffer,"%*s %*s %s %s",channel_name,auth_token);
    for(int i=strlen(channel_name)+1;i>=0;i--){
        if(channel_name[i]==','){
            channel_name[i] = '\0';
            break;
        }
    }
    cJSON *un;

    un = cJSON_GetObjectItemCaseSensitive(tokens_username,auth_token);
    char *un_str_tmp = cJSON_Print(un);
    char un_str[31];
    for(int i=1; i<strlen(un_str_tmp)-1;i++){
        un_str[i-1] = un_str_tmp[i];
    }
    un_str[strlen(un_str_tmp)-2]=0;

    char *output_str;//chizi ke midim behesh

    char *content_str;//chizi ke mirizim tu channel

    char *tmp_au;
    char tmp_au_2[36]="\"";

    strcat(tmp_au_2,auth_token);
    strcat(tmp_au_2,"\"");
    cJSON *tmp_au_j = cJSON_CreateObject();

    int tmp_flag=0;
    for(int i=0;i<online_user_cnt;i++){
        tmp_au_j = cJSON_GetArrayItem(current_tokens,i);
        tmp_au = cJSON_PrintUnformatted(tmp_au_j);
        if(strcmp(tmp_au_2,tmp_au)==0){
            //yani hast
            tmp_flag=1;
            break;
        }

    }
    if(tmp_flag == 0){

        output_str = qTjson_twinMessage("type","Error","content","Wrong Auth Token");

        response(output_str);//ino bayad  bedim be client
        return;
    }

    for(int i=strlen(channel_name)+1;i>=0;i--){
        if(channel_name[i]==','){
            channel_name[i] = '\0';
            break;
        }
    }
    sprintf(address,"channels/%s.json",channel_name);

    if((registry = fopen(address,"r")) == NULL){
        //channel vojod dare!

        output_str = qTjson_twinMessage("type","Error","content","Channel Doesnt Exist");

        response(output_str);// be client
        fclose(registry);
        return;
    }

    //yani null nabode pas bayad baz konim va msg e joined berizim tush
    registry = fopen(address,"a");

    char tmp_msg[51];
    sprintf(tmp_msg,"%s joined.",un_str);

    content_str = qTjson_twinMessage("sender","server","content",tmp_msg);
    fprintf(registry,"%s\n",content_str);//joined!

    fclose(registry);

    output_str = qTjson_twinMessage("type","Success","content","Joined Successfully");
    response(output_str);//ino bayad  bedim be client

    cJSON_AddItemToObject(tokens_channel,auth_token,cJSON_CreateString(channel_name));
    cJSON_AddItemToObject(tokens_msg_cnt,auth_token,cJSON_CreateNumber(0));


}

void send_msg(char buffer[]){
    //msg ro migirim va ba sender va mohtava zakhire mikonim
    char auth_token[33];
    char msg[80];

    sscanf(buffer,"%*s %[^,]s %s",msg);

    int n=strlen(msg) + 7;
    for(int i=n;i<n+32;i++){
        auth_token[i-n] = buffer[i];
    }
    //msg ro darim, tokene yaro ro darim, miaim channelesh ro dar miarim

    cJSON *un;

    un = cJSON_GetObjectItemCaseSensitive(tokens_username,auth_token);
    char *un_str_tmp = cJSON_Print(un);
    char un_str[31];
    for(int i=1; i<strlen(un_str_tmp)-1;i++){
        un_str[i-1] = un_str_tmp[i];
    }
    un_str[strlen(un_str_tmp)-2]=0;

    cJSON *ch;

    ch = cJSON_GetObjectItemCaseSensitive(tokens_channel,auth_token);
    char *ch_str_tmp = cJSON_Print(ch);
    char ch_str[31];
    for(int i=1; i<strlen(ch_str_tmp)-1;i++){
        ch_str[i-1] = ch_str_tmp[i];
    }
    ch_str[strlen(ch_str_tmp)-2]=0;

    //alan username va channelesh ro darim, miaim msg ro chap mikonim
    char address[56];
    sprintf(address,"channels/%s.json",ch_str);

    FILE *registry;
    char *output_str;
    char *content_str;

    if((registry = fopen(address,"a")) == NULL){

        output_str = qTjson_twinMessage("type","Error","content","Internal Error");
        response(output_str);//ino bayad  bedim be client
        fclose(registry);
        return;
    }

    registry = fopen(address,"a");

    content_str = qTjson_twinMessage("sender",un_str,"content",msg);
    fprintf(registry,"%s\n",content_str);

    fclose(registry);
}

void refresh(char buffer[]){
//hammaro array konim az jaii ke nadide bdim be client chonke client ba array kar mikone
//===================================================================================================
    char auth_token[33];
    sscanf(buffer,"%*s %s",auth_token);


    char * msg_array = qTjson_createArray();
    int n;//tedad payami ke dide
    cJSON *num_tmp = cJSON_GetObjectItem(tokens_msg_cnt,auth_token);
    n = num_tmp -> valueint;

    cJSON *channel_j;
    channel_j = cJSON_GetObjectItem(tokens_channel,auth_token);

    char address[56];
    sprintf(address,"channels/%s.json",channel_j -> valuestring);

    //hala inja midonim chanta msg ro dide, miaim shoro mikonim be jaroob e msg ha va
    //chap mikonim, harja i>=n shod msg haro chap mikonim

    FILE * reader;
    reader = fopen(address,"r");

    int i=1;
    char msg[80];
    while(!feof(reader)){
        fgets(msg,79,reader);
        for(int i=79;i>=0;i--){
            if(msg[i] == '\n'){
                msg[i] = 0;
                break;
            }
        }
        //inja msg ha amadean , bayad age i az n bozorgtar mosavi bod chap konim
        if(i>n){
            printf("%s\n",msg);

            msg_array = qTjson_appendToArray(msg_array,msg);
        }
        i++;
    }
    n=i;
    //bayad adadi ke tokens_msg_cnt dare ro delete konim va n ro berizim jash
    cJSON_DeleteItemFromObject(tokens_msg_cnt,auth_token);
    cJSON_AddItemToObject(tokens_msg_cnt,auth_token,cJSON_CreateNumber(n));

    char *output_str = qTjson_twinMessageWithArray("type","List","content",msg_array);
    printf("%s\n",output_str);

    response(output_str);//ino  midim be client
}

void members_list(char buffer[]){
    char auth_token[33];
    sscanf(buffer,"%*s %*s %s",auth_token);

    //bayad bebinim yaroo tu kodom kanale!
    char * mem_array = qTjson_createArray();

    char *channel_name;
    cJSON *channel_name_json;

    channel_name_json = cJSON_GetObjectItemCaseSensitive(tokens_channel,auth_token);
    channel_name = cJSON_PrintUnformatted(channel_name_json);

    char *tmp_ch;
    cJSON *tmp_ch_j;
    char *tmp_at;
    char *tmp_un;

    //alan esme kanal ro darim , miaim done done member haro check mikonim
    //agge kanaleshon in bod esme member ro add mikonim be arrayi ke avale tabe sakhtim

    for(int i=0;i<online_user_cnt;i++){
        //bayad kanale yaro ro dar biarim - aval token e yaroo mikhaim
        tmp_at = cJSON_PrintUnformatted(cJSON_GetArrayItem(current_tokens,i));
        char at[31];
        for(int i=1; i<strlen(tmp_at)-1;i++){
            at[i-1] = tmp_at[i];
        }
        at[strlen(tmp_at)-2]=0;

        tmp_ch_j = cJSON_GetObjectItem(tokens_channel,at);
        if(tmp_ch_j == NULL)    continue;
        tmp_ch = cJSON_PrintUnformatted(tmp_ch_j);
        if(strcmp(channel_name,tmp_ch)==0){
            //yani in yaroo ke peida kardim kanalesh mese mae!
            tmp_un = cJSON_PrintUnformatted(cJSON_GetObjectItem(tokens_username,at));
            char un[31];
            for(int i=1; i<strlen(tmp_un)-1;i++){
                un[i-1] = tmp_un[i];
            }
            un[strlen(tmp_un)-2]=0;
            //check shod, un username e dorostie , harbar un ro ezafe mikonim be array,
            //badesh ye object misazim array ro midim be object badesh pass midim be client
            mem_array = qTjson_appendToArray(mem_array,un);
        }
    }
    char *output_str;
    output_str = qTjson_twinMessageWithArray("type","List","content",mem_array);
    response(output_str); // be client

}

void leave(char buffer[]){
    //miad onsori ke neshon mide in taraf tu che kanalie ro kollan pak mikone
    //ye payame leave chap mikone tu kanal ke baraye list bekar mire
    //
    FILE *registry;
    char *output_str;
    char *content_str;

    char auth_token[33];
    sscanf(buffer,"%*s %s",auth_token);

    char *tmp_au;
    char tmp_au_2[36]="\"";

    strcat(tmp_au_2,auth_token);
    strcat(tmp_au_2,"\"");
    cJSON *tmp_au_j = cJSON_CreateObject();


    int tmp_flag=0;
    for(int i=0;i<online_user_cnt;i++){
        tmp_au_j = cJSON_GetArrayItem(current_tokens,i);
        tmp_au = cJSON_PrintUnformatted(tmp_au_j);
        if(strcmp(tmp_au_2,tmp_au)==0){
            //yani hast
            tmp_flag=1;
            break;
        }

    }
    if(tmp_flag == 0){

        output_str = qTjson_twinMessage("type","Error","content","Wrong Auth Token");
        response(output_str);//ino bayad  bedim be client
        return;
    }


    cJSON *un;

    un = cJSON_GetObjectItemCaseSensitive(tokens_username,auth_token);
    char *un_str_tmp = cJSON_Print(un);
    char un_str[31];
    for(int i=1; i<strlen(un_str_tmp)-1;i++){
        un_str[i-1] = un_str_tmp[i];
    }
    un_str[strlen(un_str_tmp)-2]=0;

    cJSON *ch;

    ch = cJSON_GetObjectItemCaseSensitive(tokens_channel,auth_token);
    char *ch_str_tmp = cJSON_Print(ch);
    char ch_str[31];
    for(int i=1; i<strlen(ch_str_tmp)-1;i++){
        ch_str[i-1] = ch_str_tmp[i];
    }
    ch_str[strlen(ch_str_tmp)-2]=0;

    //aval peyghame leave channel ro chap mikonim tu channel
    //badesh az array hazf mikonim yarroo

    char address[56];
    sprintf(address,"channels/%s.json",ch_str);

    char tmp_msg[56];
    sprintf(tmp_msg,"%s left.",un_str);

    content_str = qTjson_twinMessage("sender","server","content",tmp_msg);

    if((registry = fopen(address,"a")) == NULL){
        output_str = qTjson_twinMessage("type","Error","content","Internal Error");
        response(output_str);//ino bayad  bedim be client
        fclose(registry);
        return;
    }

    registry = fopen(address,"a");
    fprintf(registry,"%s\n",content_str);

    fclose(registry);//message e leave ro dorost kard

    cJSON_DeleteItemFromObjectCaseSensitive(tokens_channel,auth_token);
    cJSON_DeleteItemFromObject(tokens_msg_cnt,auth_token);


    output_str = qTjson_twinMessage("type","Success","content","");
    response(output_str);//ino bayad  bedim be client

}

void logout(char buffer[]){
    //az array ha o ina pakesh mikonim
    //az arraye tokens channel , tokens username va online users
    char auth_token[33];
    sscanf(buffer,"%*s %s",auth_token);

    cJSON *un;

    un = cJSON_GetObjectItemCaseSensitive(tokens_username,auth_token);
    char *un_str = cJSON_Print(un);

    int n=-1;
    cJSON * tmpj;
    char * tmpc;

    for(int i=0;i<online_user_cnt;i++){
        tmpj = cJSON_GetArrayItem(current_users,i);
        tmpc = cJSON_PrintUnformatted(tmpj);

        if(strcmp(tmpc,un_str)==0){
            n=i;
            break;
        }
    }
    if(n==-1) return;
    cJSON_DeleteItemFromArray(current_users,n);

    for(int i=0;i<online_user_cnt;i++){
        tmpj = cJSON_GetArrayItem(current_tokens,i);
        tmpc = cJSON_PrintUnformatted(tmpj);

        if(strcmp(tmpc,auth_token)==0){
            n=i;
            break;
        }
    }
    if(n==-1) return;

    cJSON_DeleteItemFromArray(current_tokens,n);
    cJSON_DeleteItemFromObject(tokens_username,auth_token);
    cJSON_DeleteItemFromObject(tokens_channel,auth_token);
    cJSON_DeleteItemFromObject(tokens_msg_cnt,auth_token);


    online_user_cnt --;
    //az har 4 array pakesh kardim
    char *output_str;

    output_str = qTjson_twinMessage("type","Success","content","");
    response(output_str);//ino bayad  bedim be client

}

void make_auth_token(){
    token[32] = 0;
    char tmp = 'a';
    int tmp_int = 3;
    for(int i=0;i<=31;i++){
        tmp_int = rand()%3;
        if(tmp_int == 0){
            tmp = (rand()%10 + 48);
        }
        else if(tmp_int == 1){
            tmp = (rand()%26 + 65);
        }
        else{
            tmp = (rand()%26 + 97);
        }

        token[i] = tmp;
    }
}

int main(){

    char *dirname1 = "users";
    char *dirname2 = "channels";
    char *dirname3 = "data";

    mkdir(dirname1);
    mkdir(dirname2);
    mkdir(dirname3);

    FILE *tmp = fopen("data/README.txt","w");
    fputs("Prozhe mabani , server e chat application\n",tmp);
    fputs("phase2\n",tmp);
    fputs("Mohammad Arman Soleimani\n",tmp);
    fputs("98105835\n",tmp);
    fclose(tmp);

    current_users = cJSON_CreateArray();
    current_tokens = cJSON_CreateArray();
    tokens_username = cJSON_CreateObject();
    tokens_channel = cJSON_CreateObject();
    tokens_msg_cnt = cJSON_CreateObject();

    read_command();

    return 0;
}
