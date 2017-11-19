#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int special_character(char c){
    
    return (c == '|' || c == ';' || c == '>' || c == '<');
}

int next_token(char* token, int* index, char* str, int ichange){

    static int i = 0;
    if(ichange == 1){
        i = 0;
        return 0;
    }
    else if(ichange < 0){
        i+=ichange;
        return 0;
    }

    bzero(token, 100);

    int comillas = 0;
    char c;
    while(str[i] == ' ')
    	i++;
    
    if(str[i] == 0){
        *token = 0;
        return 0;
    }
    
    if(special_character(str[i]))
        if(str[i] == '>' && str[i+1] == '>'){
            token = ">>";
            i += 2;
            return 2;
        }
        else{
            token[0] = str[i++];
            return 2;
        }
    
    int j = 0;
    while( comillas || ( str[i] != ' ' && str[i] != 0 && !special_character(c)) ){
        
        if(str[i] == '"' || str[i] == 39) // 39 = '
            comillas = !comillas;
        else
            token[j++] = str[i++];
    }
    
    if(comillas)
        return -1;
    
    *index = i;
    return 1; 
}

int close_input(int fd){
    if(fd != 0)
        close(fd);
}

int close_output(int fd){
    if(fd != 1)
        close(fd);
}

int againer(char* str){

    int index;
    char token[100];
    int n = 0;
    next_token(NULL, NULL, 0, 1); // inializa el i del next_token

    int iscommand = 1;

    while( 1 ){
        n = next_token(token, &index, str, 0);

        if(iscommand && n != 1){
            write(1, "Orden no encontrada\n", strlen("Orden no encontrada\n"));
            return -1;
        }

        if(n == 1 && iscommand && !strncmp(token,"again", 5)){
            
            int number;
            int j ;
            if((j = isnumber(&number, str, index)) == -1){
                write(1, "Error en el comando again\n", strlen("Error en el comando again\n"));
                return -1;
            }
            char line[500];
            if(find_line_history(line, number) == -1){
                write(1, "La linea no existe\n", strlen("La linea no existe\n"));
                return -1;
            }
            expander(str, line, index, j);
            next_token(NULL, NULL, 0, -5);  
        }

        if(n == 1)
            iscommand = 0;
        if(n == 2 && ( !strncmp(token, "|", 1) || !strncmp(token, ";", 1)))
            iscommand = 1;
    }
}

int find_line_history(char* result, int line){

    int fd = open(history_dir, O_RDONLY, 0);
    int i = 0;
    char c;
    while(i < line){
        while(read(fd, &c, 1) && c != '\n');
        i++;
    }
    if(i < line)
        return -1;
    i = 0;
    while((c = read(fd, &c, 1)) != '\n')
        result[i++] = c;
    result[i] = 0;
    return 0;
}

int isnumber(int* number, char* str,int index){

    int i = index;
    int s = 0;
    while(str[i] == ' '){
        i++;
        s = 1;
    }
    if(!s)
        return -1;
    char n[100];
    int j = 0;
    s = 0;
    while(str[j + i] >= 48 && str[j + i] <= 57){
        s = 1;
        n[j] = str[i+j];
        j++;
    }
    if(!s)
        return -1;
    if(str[j] != 0 || str[j] != ' ' || !special_character(str[j]))
        return -1;
    *number = atoi(n);

    return i+j;
}

int expander(char* str, char* paster, int i, int j){
    
    char rest[100];
    strcpy(rest, str + j);
    strcpy(str + j, paster);
    strcpy(str + i - 5, rest);
    return 0;
}

int parser(char* str){

    char c;
    int i = 0;
    int index;
    char token[100];
    int ichange = 0;
    int n = 0;
    next_token(NULL, NULL, 0, 1); // inializa el i del next_token

    int iscommand = 1;
    char command[100];
    char* argv[10];
    int argc = 0;
    int input = 0;
    int output = 1;

    while( 1 ){
        n = next_token(token, str, &index, ichange);
        if(iscommand && n != 1){
            write(1, "Orden no encontrada\n", strlen("Orden no encontrada\n"));
            return -1;
        }

        switch (n){
            case 1:              //caso en que sea un comando o un parametro
                if(iscommand){
                        strcpy(command, token);
			            strcpy(argv[argc++], token);
                        iscommand = 0;
                }
                else
                    strcpy(argv[argc++], token);
                break;

            case 2:             //caso en que sea un caracter especial
                if(!strncmp(token, "|", 1)){ //si es un pipe

                    int p[2];
                    pipe(p);
                    argv[argc] = NULL;
                    execute(command, argv, input, p[1]);
                    close(p[1]);
                    close_input(input);

                    output = 1;
                    input = p[0];
                    iscommand = 1;
                    argc = 0;
                }

                else if(!strncmp(token, "<", 1)){

                    if(next_token(token, str, &index, ichange) != 1){
                        write(1, "Error en el archivo esperado\n", strlen("Error en el archivo esperado\n"));
                        return -1;
                    }
                    input = open(token, O_RDONLY, 0);
                    
                }

                else if(!strncmp(token, ">>", 2)){

                    if(next_token(token, str, &index, ichange) != 1){
                        write(1, "Error en el archivo esperado\n", strlen("Error en el archivo esperado\n"));
                        return -1;
                    }
                    output = open(token, O_WRONLY | O_CREAT | O_APPEND, 0666);
                }

                else if(!strncmp(token, ">", 1)){
                    
                   if(next_token(token, str, ichange) != 1){
                        write(1, "Error en el archivo esperado\n", strlen("Error en el archivo esperado\n"));
                            return -1;
                    }
                    output = open(token, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                }
                else if(!strncmp(token, ";", 1)){
                    argv[argc] = NULL;
                    execute(command, argv, input, output);
                    close_input(input);
                    close_output(output);
    
                    output = 1;
                    input = 0;
                    iscommand = 1;
                    argc = 0;    
                }
                break;

            case 0:
                argv[argc] = NULL;
                execute(command, argv, input, output);
                close_input(input);
                close_output(output);

                output = 1;
                input = 0;
                iscommand = 1;
                argc = 0;
                return 0;
                break;
            case -1:
            	return -1;
            	break;
        }

    }

}
