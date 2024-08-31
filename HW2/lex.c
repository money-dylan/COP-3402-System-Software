//Dylan Money & Ryan Richards

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h> 

#define MAX_PROGRAM_SIZE 1000
#define MAX_WORD_LENGTH 11
#define MAX_NUMBER_LENGTH 5
#define ERROR_BUFFER 50

typedef enum {
    skipsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5,
    multsym = 6, slashsym = 7, fisym = 8, eqlsym = 9, neqsym = 10, lessym = 11,
    leqsym = 12, gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16,
    commasym = 17, semicolonsym = 18, periodsym = 19, becomessym = 20,
    beginsym = 21, endsym = 22, ifsym = 23, thensym = 24, whilesym = 25,
    dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30,
    writesym = 31, readsym = 32, elsesym = 33
} TokenType;

/*Struct for lexeme*/
struct Lexeme {
    char ID[MAX_WORD_LENGTH];
    TokenType Token;
}program;

/*Returns the amount of chars in a file*/
int SizeOfFile(FILE *file){
    int fileSize = 0, c;
    while ((c = fgetc(file)) != EOF) {
        fileSize++;
    }
    return fileSize;
}


int main(int argc, char *argv[]) {
    /*Initialize Values*/
    struct Lexeme program[MAX_PROGRAM_SIZE]; // Array to store lexemes
    int programSize = 0, fileSize = 0;
    bool DontPrint = false, commentOn = false; 
    char c;
    char word[ERROR_BUFFER];
    char number[ERROR_BUFFER];
    

    /*Open File*/
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Error opening file\n");
        return 1;
    }

    /*Allocate memory for plaintext array*/
    fileSize = SizeOfFile(file);
    char *plaintext = (char *)malloc(sizeof(char) * fileSize);
    if (plaintext == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }

    fseek(file, 0, SEEK_SET); //file position indicator to the beginning of file
    fread(plaintext, sizeof(char), fileSize, file); //Copy the file into the plaintext array
    fclose(file);   // Close the file
    printf("\n%s\n", plaintext); //output the contents of the file

    /*Headers for output*/
    printf("\nLexeme Table:\n");
    printf("\nLexeme:\tToken Type:\n");

    /*MAIN LOOP FOR TOKENIZATION*/
    for(int i = 0; i < fileSize; i++) {

        /*Ignores Comments*/
        if (plaintext[i] == '/' && plaintext[i + 1] == '*'){
            commentOn = true;
            DontPrint = true;
            for (int j = i + 2; j < fileSize; j++){
                if (plaintext[j] == '*' && plaintext[j + 1] == '/'){
                    i = j + 2;
                    commentOn = 0;
                    break;
                }
            }
            if(commentOn){ 
                printf("\nERROR: Comment Never Closed\n"); 
                i = fileSize;
                break;
            }
        }

        /*Switch Cases for allocating sym Values*/
        switch(plaintext[i]){
            
            /*Whitespace Char*/
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                DontPrint = true;
                break;
        
            /*Operators*/
            case '+': //4
                strcpy(program[programSize].ID, "+");
                program[programSize].Token = plussym;
                programSize++;
                break;

            case '-': //5
                strcpy(program[programSize].ID, "-");
                program[programSize].Token = minussym;
                programSize++;
                break;
                
            case '*': //6
                strcpy(program[programSize].ID, "*");
                program[programSize].Token = multsym;
                programSize++;
                break;

            case '/': //7
                strcpy(program[programSize].ID, "+");
                program[programSize].Token = slashsym;
                programSize++;
                break;

            case '=': //9
                strcpy(program[programSize].ID, "=");
                program[programSize].Token = eqlsym;
                programSize++;
                break;

            case '!': //10
                if (plaintext[i + 1] == '='){
                    strcpy(program[programSize].ID, "!=");
                    program[programSize].Token = neqsym;
                    programSize++;
                    i += 1;
                }else { 
                    printf("%c\tERROR: Invalid Symbol\n", plaintext[i]); 
                    DontPrint = true;  
                }
                break;

            case '<': //11 & 12
                if (plaintext[i + 1] == '='){
                    strcpy(program[programSize].ID, "<=");
                    program[programSize].Token = leqsym;
                    programSize++;
                    i += 1;
                    break;
                }else{
                    strcpy(program[programSize].ID, "<");
                    program[programSize].Token = lessym;
                    programSize++;
                    break;
                }

            case '>': //13 & 14
                 if (plaintext[i + 1] == '='){
                    strcpy(program[programSize].ID, ">=");
                    program[programSize].Token = geqsym;
                    programSize++;
                    i += 1;
                    break;
                }else{
                    strcpy(program[programSize].ID, ">");
                    program[programSize].Token = gtrsym;
                    programSize++;
                    break;
                }

            case '(': //15
                strcpy(program[programSize].ID, "(");
                program[programSize].Token = lparentsym;
                programSize++;
                break;

            case ')': //16
                strcpy(program[programSize].ID, ")");
                program[programSize].Token = rparentsym;
                programSize++;
                break;

            case ',': //17
                strcpy(program[programSize].ID, ",");
                program[programSize].Token = commasym;
                programSize++;
                break;

            case ';': //18
                strcpy(program[programSize].ID, ";");
                program[programSize].Token = semicolonsym;
                programSize++;
                break;

            case '.': //19
                strcpy(program[programSize].ID, ".");
                program[programSize].Token = periodsym;
                programSize++;
                break;

            case ':': //20
                if (plaintext[i + 1] == '='){
                    strcpy(program[programSize].ID, ":=");
                    program[programSize].Token = becomessym;
                    programSize++;
                    i += 1;
                    break;
                }else { 
                    printf("%c\tERROR: Invalid Symbol\n", plaintext[i]); 
                    DontPrint = true; 
                }
                continue;

            default:
                /*Identifiers and Reserved Words*/
                if(isalpha(plaintext[i])){
                    memset(word, 0, sizeof(word)); //clear word string
                    /*Isolate Word*/
                    for(int j = 0; j < fileSize; j++){
                        if(isalnum(plaintext[i])){
                            word[j] = plaintext[i];
                            i++;
                        }else{ break; }                    
                    }          
                    
                    /*Reserved Words*/
                    if (strcmp(word, "begin") == 0) {
                        strcpy(program[programSize].ID, "begin");
                        program[programSize].Token = beginsym;
                        i--;
                        programSize++;
                    } 
                    else if (strcmp(word, "end") == 0) {
                        strcpy(program[programSize].ID, "end");
                        program[programSize].Token = endsym;
                        i--;
                        programSize++;
                    }
                    else if (strcmp(word, "if") == 0) {
                        strcpy(program[programSize].ID, "if");
                        program[programSize].Token = ifsym;
                        i--;
                        programSize++;
                    }
                    else if (strcmp(word, "then") == 0) {
                        strcpy(program[programSize].ID, "then");
                        program[programSize].Token = thensym;
                        i--;
                        programSize++;
                    } 
                    else if (strcmp(word, "while") == 0) {
                        strcpy(program[programSize].ID, "while");
                        program[programSize].Token = whilesym;
                        i--;
                        programSize++;
                    } 
                    else if (strcmp(word, "do") == 0) {
                        strcpy(program[programSize].ID, "do");
                        program[programSize].Token = dosym;
                        i--;
                        programSize++;
                    } 
                    else if (strcmp(word, "call") == 0) {
                        strcpy(program[programSize].ID, "call");
                        program[programSize].Token = callsym;
                        i--;
                        programSize++;
                    } 
                    else if (strcmp(word, "const") == 0) {
                        strcpy(program[programSize].ID, "const");
                        program[programSize].Token = constsym;
                        i--;
                        programSize++;
                    } 
                    else if (strcmp(word, "var") == 0) {
                        strcpy(program[programSize].ID, "var");
                        program[programSize].Token = varsym;
                        i--;
                        programSize++;
                    } 
                    else if (strcmp(word, "procedure") == 0) {
                        strcpy(program[programSize].ID, "proc");
                        program[programSize].Token = procsym;
                        i--;
                        programSize++;
                    } 
                    else if (strcmp(word, "write") == 0) {
                        strcpy(program[programSize].ID, "write");
                        program[programSize].Token = writesym;
                        i--;
                        programSize++;
                    } 
                    else if (strcmp(word, "read") == 0) {
                        strcpy(program[programSize].ID, "read");
                        program[programSize].Token = readsym;
                        i--;
                        programSize++;
                    } 
                    else if (strcmp(word, "else") == 0) {
                        strcpy(program[programSize].ID, "else");
                        program[programSize].Token = elsesym;
                        i--;
                        programSize++;
                    }
                    else if (strcmp(word, "fi") == 0) {
                        strcpy(program[programSize].ID, "fi");
                        program[programSize].Token = fisym;
                        i--;
                        programSize++;
                    }
                    
                    /*Identifiers*/
                    else if(strlen(word) > MAX_WORD_LENGTH){
                        printf("%s\tERROR: Identifier too long\n", word);
                        DontPrint = true;
                    }else{
                        strcpy(program[programSize].ID, word);
                        program[programSize].Token = identsym;
                        i--;
                        programSize++;
                        
                    }
                }
                /*Numbers*/
                else if(isdigit(plaintext[i])){
                    memset(number, 0, sizeof(number)); //clear number string
                    /*Isolate Number*/
                    for(int j = 0; j < fileSize; j++){
                        if(isdigit(plaintext[i])){
                            number[j] = plaintext[i];
                            i++;
                        }else{ break; }                       
                    }
                    /*Number*/
                    if(strlen(number) > MAX_NUMBER_LENGTH){
                        printf("%s\tERROR: Number too long\n", number);
                        DontPrint = true;
                    }else{
                        strcpy(program[programSize].ID, number);
                        program[programSize].Token = numbersym;
                        i--;
                        programSize++;
                    }
                }
                /*Invalid Symbol*/
                else{
                    printf("%c\tERROR: Invalid Symbol\n", plaintext[i]); 
                    DontPrint = true;
                }
                break;

        }
        
        /*Output for the Lexeme Table*/
        if(!DontPrint){
            printf("%s\t%d\n", program[programSize - 1].ID, program[programSize - 1].Token);
        }else{ DontPrint = false; }
    }

    /*Output for final token list*/
    printf("\nToken List:\n");
    for (int i = 0; i < programSize; i++) {
        printf("%d  ", program[i].Token);
        if(program[i].Token == 2 || program[i].Token == 3){
            printf("%s  ", program[i].ID);
        }
    }

    free(plaintext);
    return 0;
}
