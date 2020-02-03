//phase 3 be shekle sadetar



#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

///ye tabe ke miad chaharta chiz migire va ye error message ya success mide
char * qTjson_twinMessage(char name1[],char content1[] , char name2[] , char content2[]){
    char * output;
    output = (char*)malloc(2000 * sizeof(char));
    sprintf(output,"{\"%s\":\"%s\",\"%s\":\"%s\"}",name1,content1,name2,content2);
    return output;
}

char * qTjson_twinMessageWithArray(char name1[],char content1[] , char name2[] , char array[]){
    char * output;
    output = (char*)malloc(2000 * sizeof(char));
    sprintf(output,"{\"%s\":\"%s\",\"%s\":%s}",name1,content1,name2,array);
    return output;
}

//ye item ke miad ye array dorost mikone

char * qTjson_createArray(){
    char * output;
    output = (char*)malloc(2000 * sizeof(char));
    output = "[]";
    return output;
}

// ye tabe ke miad element migire va michasbone tahe array
char * qTjson_appendToArray(char array[],char item[]){
    char * output;
    output = (char*)malloc(2000 * sizeof(char));
    strcpy(output,array);

    int n = strlen(output);
    int commaFlag = 1;
    if(n==2) commaFlag = 0;

    output[n-1] = 0;
    if(commaFlag == 1) strcat(output,",");
    strcat(output,"\"");
    strcat(output,item);
    strcat(output,"\"");
    strcat(output,"]");

    return output;
}

//baraye parse kardan 3 halat darim , ya inke json e mamoli 2 iteme darim
//ke bayad mohtava ro kharej konim va bedim biron
//ya json e members list darim , ke bayad liste member haro done done chap konim
//ya json e messages darim ke bayad message haye mellat ro be hamrahe sender bedast biarim

//parser e ma miad baraye har halat joda joda tabe mizare

/*
type:
0 => error
1 => success
2 => token
3 => etc
*/
char * qTjson_parseTwin(char array[] , int * type){
    for(int i=0;array[i]!=0;i++){
        if(array[i]=='\"') array[i] = ' ';
        if(array[i]=='{') array[i] = ' ';
        if(array[i]=='}') array[i] = ' ';
        if(array[i]==':') array[i] = ' ';
        if(array[i]==',') array[i] = ' ';
    }

    char * output = (char*)malloc(40*sizeof(char));
    char type_c[40];

    sscanf(array,"%*s %s %*s %[^\n]s",type_c,output);

    if(strcmp(type_c,"Success")==0) *type = 1;
    if(strcmp(type_c,"Error")==0) *type = 0;
    if(strcmp(type_c,"AuthToken")==0) *type = 2;

    return output;

}

void qTjson_printMemList(char array[]){
    //bayad member haro done done joda konim va harkodom ro print konim

    int tmp_flg;
    for(int i=26;i<200;i++){
        //if(array[i]==',' && array[i-1]=='\"' && array[i+1]=='\"') array[i] = '\n';
		if(array[i]==']') {
            tmp_flg = 0;
		}
		if(tmp_flg){
			printf("%c",array[i]);
		}
		if(array[i]=='[') {
            tmp_flg = 1;

		}
    }



}

void qTjson_printRefresh(char array[]){
    int tmp_flg=1;
	//printf("ssss:%s\n",recv_buffer);
	for(int i=0;i<8000;i++){
		if(array[i]=='{' && array[i+2]=='s'){
			//we have a sender!
			printf("* ");
			for(i+1;tmp_flg==1;i++){//prints the username
				if(array[i+11]=='"' || array[i+12]==']') break;//end of username
				printf("%c",array[i+11]);
			}
			printf(": ");
		}
		if(array[i]=='t' && array[i+2]==':'){
			//we have the message!
			for(i+1;tmp_flg==1;i++){//prints the message
				if(array[i+4]=='"' || array[i+4]==']') break;//end of message
				printf("%c",array[i+4]);
			}
			printf("\n");
		}

		if(array[i]==']') break;//end of JSON content
	}
}





