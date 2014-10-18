/*
 ============================================================================
 Name        : second_deamon.c
 Author      : Ivan
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <errno.h>
#include <sys/stat.h>
#include <syslog.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#define PIDFILENAME "/tmp/second_deamon.pid"

void SetPidFile(char* Filename)
{
    FILE* f;

    f = fopen(Filename, "w+");
    if (f)
    {
        fprintf(f, "%u",getpid());
        fclose(f);
    }
}
void clean_and_kill(){
	unlink(PIDFILENAME);
	exit(0);


}

int take_info(char *array,int *fd,char *pass,char *adr){
	char *stok=strtok(array," ");
	if(stok != NULL)
	    		{
			    	*fd=atoi(stok);
			    	stok = strtok (NULL, " ");
	    		}
	    	if(stok != NULL)
	    	   {
	    			stok = strtok (NULL, " ");
	    	   }
	    	if(stok != NULL)
	    		    	   {
	    		    			stok = strtok (NULL, " ");

	    		    	   }
	    	if(stok != NULL)
	    		    	   {
	    						strcpy(adr,stok);
	    		    			stok = strtok (NULL, " ");
	    		    	   }
	    	if(stok != NULL)
	    		    	   {
	    		    			stok = strtok (NULL, " \r\n");
	    		    	   }
	    	if(stok != NULL)
	    		    		{
	    		    		    strcpy(pass,stok);
	    						stok = strtok (NULL, " ");
	    						return 0;
	    		    		}
	    	return 1;
}


#define MSGSZ     128

typedef struct msg {
         long    mtype;
         char    mtext[MSGSZ];
         }msgbuf;
int main(int argc, char** argv)
{
	//Работа по создании очереди и демона
	key_t key;
	key=5555;
    int pid,sid,msgid;
    // создаем потомка
    pid = fork();
    if (pid < 0) {
                   exit(EXIT_FAILURE);
           	   	   }
    if (pid > 0) {
                   exit(EXIT_SUCCESS);

           	   	   }

     umask(0);
     sid=setsid();
     chdir("/");
    close(STDIN_FILENO);
     close(STDOUT_FILENO);
     close(STDERR_FILENO);
     if ((msgid = msgget(key,IPC_CREAT |0666)) < 0) {
    	 perror("werfw");
                         exit(1);
                     }
    SetPidFile(PIDFILENAME);
    msgbuf sbuf;
    msgbuf rbuf;
    while(1){
    	msgrcv(msgid, &sbuf, MSGSZ, 1, 0);
    	int fd;
    	char query[1000];
    	char pass[100];
    	char adr[20];
    	char path[100];
    	fprintf(stderr,"%s",sbuf.mtext);
    	if(!take_info(sbuf.mtext,&fd,pass,adr)){
    		sprintf(query,"sshpass -p %s ssh -o StrictHostKeychecking=no %s \"C:/Windows/System32/wbem/WMIC.exe product get name\" > /tmp/%s.txt",pass,adr,adr);
    		fprintf(stderr,"%s",query);
    		system(query);
    		sleep(4);
    		sprintf(path,"/tmp/%s.txt",adr);
    		FILE *open=fopen(path,"a");
    		fseek(open, 0L, SEEK_END);
    		if(!ftell(open)){
    			rbuf.mtype=fd;
    			strcpy(rbuf.mtext,"Error");
    			msgsnd(msgid, &rbuf,strlen(rbuf.mtext) , IPC_NOWAIT);
    		}
    		rbuf.mtype=fd;
    		strcpy(rbuf.mtext,path);
    		msgsnd(msgid, &rbuf,strlen(rbuf.mtext) , IPC_NOWAIT);

    	}
    	rbuf.mtype=fd;
    	strcpy(rbuf.mtext,"Error");
    	msgsnd(msgid, &rbuf,strlen(rbuf.mtext) , IPC_NOWAIT);

     sleep(10);
    }

}

