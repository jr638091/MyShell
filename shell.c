 #include <stdio.h>
 #include <unistd.h>
 #include <stdlib.h>
 #include <sys/types.h>
 #include <sys/wait.h>
 #include <string.h>

int exitcount;
int c;
char *code;

void non_specific_user();
void handlerctrC(int a){if(exitcount++){ /*write(STDOUT_FILENO,"\n",1);*/ exit(0); } else { /*write(STDOUT_FILENO, "\n~:", 3);*/}}

void leer(){
    char t;
    c = 0;
    while (read(STDIN_FILENO,&t,1))
    {
        if(t != '\n'){
            code[c++] = t;
        }
        else{ code[c++] = '\0';
            break;
        }
    }
}

int execute_cmd(char *args[],int fd_source,int fd_destiny){
    int in[2] = {fd_source,0};
    int out[2] = {1,fd_destiny};
    pipe(in);
    pipe(out);
    int ret = execvp(args[0],args);
    return ret;
}

int main(int cargs, char *vargs[]){

    code = malloc(500);
    // leer();
    // write(STDOUT_FILENO,code,c);
        

    exitcount = 0;
    signal(SIGINT,handlerctrC);

    if(cargs == 1){
        non_specific_user();
    }

    return 0;
}



void non_specific_user(){
    while (1)
    {
        write(STDOUT_FILENO,"~:",2);
        leer();
        int count = 1;
        char *token = strtok(code, " ");
        char *b[500];
        b[0] = token;
        while(token = strtok(NULL, " ")) {
            b[count] = token;
            ++count;
        }    
        char *args[count+1];
        for (int i = 0; i < count; i++)
        {
            args[i] = b[i];
        }
        args[count] = NULL;

        int pid = fork();
        
        if (pid > 0)
        {
            int status = 0;
            int f = waitpid(pid, &status, 0);
        }
        else if (!pid)
        {
            if(strcmp(code,"\0") != 0){
                
                if(execute_cmd(args,0,1) == -1){
                    write(STDOUT_FILENO,"command not found\n",sizeof("command not found\n"));
                    return;
                }
            }
        }        
    }
}