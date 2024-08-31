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
#define MAX_SYMBOL_TABLE_SIZE 500

typedef enum {
    oddsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5,
    multsym = 6, slashsym = 7, fisym = 8, eqlsym = 9, neqsym = 10, lessym = 11,
    leqsym = 12, gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16,
    commasym = 17, semicolonsym = 18, periodsym = 19, becomessym = 20,
    beginsym = 21, endsym = 22, ifsym = 23, thensym = 24, whilesym = 25,
    dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30,
    writesym = 31, readsym = 32, elsesym = 33
} TokenType;

/*Struct for lexeme*/
typedef struct Lexeme {
    char ID[MAX_WORD_LENGTH];
    TokenType Token;
}Lexeme;

/*Struct for Symbol Table*/
typedef struct Symbol{
    int kind;                   // const = 1, var = 2
    char name[MAX_WORD_LENGTH]; // name up to 11 chars
    int val;                    // number (ASCII value)
    int level;                  // L level
    int addr;                   // M address
    int mark;                   // to indicate unavailable or deleted
} Symbol;

/*Struct for Code Gen*/
typedef struct Code{
    int OP;
    int L;
    int M;
} Code;

void Program();
void Block();
void ConstDec();
int VarDec();
void Statement();
int SymbolTableSearch();
void SymbolTableAdd();
void Expression();
void Condition();
void Term();
void Factor();
void CodeGenerator();
void ProcDec();

/*Function to Take a Numerical OP and find the abreviated counterpart*/
void GetOpCode(int OP, char opCode[]){
    switch(OP){
        case 1:
            strcpy(opCode, "LIT");
            break;
        case 2:
            strcpy(opCode, "OPR");
            break;
        case 3:
            strcpy(opCode, "LOD");
            break;
        case 4:
            strcpy(opCode, "STO");
            break;
        case 5:
            strcpy(opCode, "CAL");
            break;
        case 6:
            strcpy(opCode, "INC");
            break;
        case 7:
            strcpy(opCode, "JMP");
            break;
        case 8:
            strcpy(opCode, "JPC");
            break;
        case 9:
            strcpy(opCode, "SYS");
            break;
        default:
            strcpy(opCode, "IDK");
            break;
    }
}

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
    struct Symbol SymbolTable[MAX_SYMBOL_TABLE_SIZE]; //Array to store symbols
    struct Code CodeGen[MAX_PROGRAM_SIZE]; //Array to store code generation
    int programSize = 0, fileSize = 0;
    bool DontPrint = true, commentOn = false; 
    char c;
    char word[ERROR_BUFFER];
    char number[ERROR_BUFFER];
    

    /*Open File*/
    FILE *file = fopen(argv[1], "r");
    //FILE *file = fopen("input.txt", "r");
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

    fseek(file, 0, SEEK_SET);                       //file position indicator to the beginning of file
    fread(plaintext, sizeof(char), fileSize, file); //Copy the file into the plaintext array
    fclose(file);                                   // Close the file
    printf("\n%s\n", plaintext);                    //output the contents of the file

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
                printf("\nError: Comment Never Closed\n");
                exit(0);
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
                strcpy(program[programSize].ID, "/");
                program[programSize].Token = slashsym;
                programSize++;
                break;

            case '=': //9
                strcpy(program[programSize].ID, "=");
                program[programSize].Token = eqlsym;
                programSize++;
                break;

            case '<': //10, 11, & 12
                if (plaintext[i + 1] == '='){
                    strcpy(program[programSize].ID, "<=");
                    program[programSize].Token = leqsym;
                    programSize++;
                    i += 1;
                    break;
                }else if(plaintext[i + 1] == '>'){
                    strcpy(program[programSize].ID, "<>");
                    program[programSize].Token = neqsym;
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
                    printf("%c\tError: Invalid Symbol\n", plaintext[i]); 
                    exit(0);
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
                    else if (strcmp(word, "fi") == 0) {
                        strcpy(program[programSize].ID, "fi");
                        program[programSize].Token = fisym;
                        i--;
                        programSize++;
                    }
                    
                    /*Identifiers*/
                    else if(strlen(word) > MAX_WORD_LENGTH){
                        printf("%s\tError: Identifier too long\n", word);
                        exit(0);
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
                        printf("%s\tError: Number too long\n", number);
                        exit(0);
                    }else{
                        strcpy(program[programSize].ID, number);
                        program[programSize].Token = numbersym;
                        i--;
                        programSize++;
                    }
                }
                /*Invalid Symbol*/
                else{
                    //printf("%c\tError: Invalid Symbol\n", plaintext[i]); 
                    exit(0);
                }
                break;

        }
        
        /*Output for the Lexeme Table*/
        if(!DontPrint){
            //printf("%d\t%s\t%d\n", programSize - 1, program[programSize - 1].ID, program[programSize - 1].Token);
        }else{ DontPrint = false; }
    }
    printf("\n");
    
    /*Main Parsing Loop*/
    Program(program, programSize, CodeGen, SymbolTable);
    
    free(plaintext);
    return 0;
}

/*LAYER 1: PROGRAM*/
    void Program(Lexeme *program, int programSize, Code *CodeGen, Symbol *SymbolTable){
        /*Initialize Values*/
        int Pindex = 0, PSymbolTableSize = 0, PSymbolTableIndex = 0, PGenIndex = 0, Plevel = -1;
        int *index = &Pindex;                       //Index of the program
        int *SymbolTableSize = &PSymbolTableSize;   //Total size of the symbol table
        int *SymbolTableIndex = &PSymbolTableIndex; //Index position for the M addr
        int *GenIndex = &PGenIndex;                 //Index for the Code Generator
        int *level = &Plevel;                       //Lexographical level

        /*Run Tokens through recursive system*/
        while(*index < programSize - 1){ 
            Block(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level); 
        }

        /*Syntax Check: Program Ends in Period*/
        if(program[*index].Token != periodsym){
            printf("Error: program must end with period\n");
            exit(0);
        }
        
        printf("\n\nNo Errors, Program is syntactically correct\n\n");

        /*HALT*/
        CodeGenerator(CodeGen, GenIndex, 9, 0, 3);

        /*Make mark values in Symbol Table 1 after Execution*/
        for(int i = 0; i < *SymbolTableSize; i++){
            SymbolTable[i].mark = 1;
        }
        
        FILE *filePointer;
        
        filePointer = fopen("elf.txt", "w");

        // Check if file was opened successfully
        if (filePointer == NULL) {
            printf("Unable to create file.\n");
            return; // Exit program with error
        }
    

        /*Printing Assembly Code*/
        char opCode[4];
        printf("Line\t OP\t L\t M\n---------------------------------------------------\n");
        for (int i = 0; i < *GenIndex; i++) {
            GetOpCode(CodeGen[i].OP, opCode);
            printf("%d\t %s\t %d\t %d\n", i, opCode, CodeGen[i].L, CodeGen[i].M);
            //Output to elf.txt
            fprintf(filePointer, "%d\t %d\t %d\n", CodeGen[i].OP, CodeGen[i].L, CodeGen[i].M);
        }
        
        fclose(filePointer);

        return;
    }

/*RECURSION LAYER 2: DETERMINES IF TOKENS CONSTANT/VARIABLE DECLARATIONS OR STATEMENTS */
    void Block(Code *CodeGen, int *GenIndex, int *index ,struct Lexeme program[], struct Symbol SymbolTable[], int *SymbolTableSize, int *SymbolTableIndex, int *level){
        (*level)++; //Increment lex level
        int space = 3; //Space needed for a new AR
        int JMPindx = *GenIndex; //Save index of JMP instruction
        int SymbolTablePrev = *SymbolTableSize; //Save index of SymbolTable of the new level
        int SymbolTableAddr = *SymbolTableIndex; //Save index of AR
        *SymbolTableIndex = 3; //new AR symbol index

        CodeGenerator(CodeGen, GenIndex, 07, 0, 1969); //JMP
        
        /*If current token is a constant declaration*/
        if(program[*index].Token == constsym) { 
            ConstDec(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level);
        } 
        
        /*If current token is a variable declaration*/
        if(program[*index].Token == varsym) { 
            space += VarDec(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level); 
        } 
        
        /*If current token is a procedure declaration*/
        if(program[*index].Token == procsym){
            ProcDec(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level);
            CodeGenerator(CodeGen, GenIndex, 02, 0, 0); //RTN
        }

        //correct JMP address
        CodeGen[JMPindx].M = 3 * *GenIndex;
        CodeGenerator(CodeGen, GenIndex, 06, 0, space); //INC

        /*If current token is not a declaration, it must be a statement of some type*/
        Statement(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level);
        (*index)++;

        
        *SymbolTableSize = SymbolTablePrev; //delete symbols of higher level
        *SymbolTableIndex = SymbolTableAddr; //restore symbol index of previous AR
        (*level)--; //decrement the lexical level
        return;
    }

/*RECURSION LAYER 3: HANDLES CASES OF CONSTANT & VARIABLE DECLARATIONS AND STATEMENTS*/
    /*Constant Declarations*/
    void ConstDec(Code *CodeGen, int *GenIndex, int *index ,struct Lexeme program[], struct Symbol SymbolTable[], int *SymbolTableSize, int *SymbolTableIndex, int *level){
        do {
            (*index)++;
            /*Syntax Check: Is there an identifier after the constant declaration*/
            if(program[*index].Token != identsym){
                printf("Error: const declarations must be followed by identifier\n");
                exit(0);
            }

            /*Syntax Check: Has the Identifier already be declared*/
            int search = SymbolTableSearch(program[*index].ID, SymbolTable, *SymbolTableSize, SymbolTableIndex, level);
            if(search != -1 && SymbolTable[search].level == *level){
                printf("Error: Symbol name has already been declared\n");
                exit(0);
            }
            
            /*Saving Identifier name for SymbolTable addition*/
            char name[MAX_WORD_LENGTH];
            strcpy(name, program[*index].ID);
            (*index)++;

            /*Syntax Check: Is there a '=' after the identifier in the constant declaration*/
            if(program[*index].Token != eqlsym){
                printf("Error: constants must be assigned with '='\n");
                exit(0);
            }
            (*index)++;

            /*Syntax Check: Constant declarations must be assigned with integer values*/
            if(program[*index].Token != numbersym || program[(*index) + 1].Token == periodsym){
                printf("Error: constants must be assigned an integer value\n");
                exit(0);
            }

            /*Add constant to symbol table*/
            SymbolTableAdd(SymbolTable, 1, name, atoi(program[*index].ID), level, SymbolTableSize,  0, SymbolTableIndex);
            (*index)++;
        } while(program[*index].Token == commasym); //loop for multiple declarations

        /*Syntax Check: Constant declarations must end with semicolons*/
        if(program[*index].Token != semicolonsym){
            printf("Error: constant declarations must be followed by a semicolon\n");
            exit(0);
        }
        (*index)++;
        return;
    }

    /*Variable Declarations*/
    int VarDec(Code *CodeGen, int *GenIndex, int *index ,struct Lexeme program[], struct Symbol SymbolTable[], int *SymbolTableSize, int *SymbolTableIndex, int *level){
        int NumVars = 0; //returns number of variables
        do {
            NumVars++; 
            (*index)++;

            /*Syntax Check: Is there an identifier after the variable declaration*/
            if(program[*index].Token != identsym){
                printf("Error: variable declarations must be followed by identifier\n");
                exit(0);
            }
            
            /*Syntax Check: Has the Identifier already be declared*/
            int search = SymbolTableSearch(program[*index].ID, SymbolTable, *SymbolTableSize, SymbolTableIndex, level);
            if(search != -1 && SymbolTable[search].level == *level){
                printf("Error: Symbol name has already been declared\n");
                exit(0);
            }

            /*Add variable to symbol table*/
            SymbolTableAdd(SymbolTable, 2, program[*index].ID, 0, level, SymbolTableSize, 0, SymbolTableIndex);
            (*index)++;

        } while(program[*index].Token == commasym); //loop for multiple declarations

        /*Syntax Check: Variable declarations must end with semicolons*/
        if(program[*index].Token != semicolonsym){
            printf("Error: variable declarations must be followed by a semicolon\n");
            exit(0);
        }
        (*index)++;
        return NumVars;
    }

    /*Procedure Declarations*/
    void ProcDec(Code *CodeGen, int *GenIndex, int *index ,struct Lexeme program[], struct Symbol SymbolTable[], int *SymbolTableSize, int *SymbolTableIndex, int *level){
        (*index)++;
       
        /*Syntax Check: Is there an identifier after the procedure declaration*/
        if(program[*index].Token != identsym){
            printf("Error: procedure declarations must be followed by identifier\n");
            exit(0);
        }

        /*Add procedure to symbol table*/
        SymbolTableAdd(SymbolTable, 3, program[*index].ID, 3 * (*GenIndex), level, SymbolTableSize, 0, SymbolTableIndex);
        (*index)++;

        /*Syntax Check: procedure declarations must end with semicolons*/
        if(program[*index].Token != semicolonsym){
            printf("Error: procedure declarations must be followed by a semicolon\n");
            exit(0);
        }
        (*index)++;

        Block(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level);

        //*(index)--;
        /*Syntax Check: procedures  must end with semicolons*/
        if(program[*index].Token != semicolonsym){
            printf("Error: procedure end statements must be followed by a semicolon\n");
            exit(0);
        }
        (*index)++;
        return;
    }

    /*Statements*/
    void Statement(Code *CodeGen, int *GenIndex, int *index ,struct Lexeme program[], struct Symbol SymbolTable[], int *SymbolTableSize, int *SymbolTableIndex, int *level){
        /*Identifier*/
        if (program[*index].Token == identsym){
            /*Syntax Check: Has the Identifier already be declared*/
            int idntaddr = SymbolTableSearch(program[*index].ID, SymbolTable, *SymbolTableSize, SymbolTableIndex, level);           
            if(idntaddr == -1){
                printf("Error: undeclared identifier\n");
                exit(0);
            }
            /*Syntax Check: is the identifier a variable*/
            if(SymbolTable[idntaddr].kind != 2){
                printf("Error: only variable values may be altered\n");
                exit(0);
            }
            (*index)++;
            
            /*Syntax Check: Variable assignment statements must use ':='*/
            if(program[*index].Token != becomessym){
                printf("Error: assignment statements must use ':='\n");
                exit(0);
            }
            (*index)++;
            Expression(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level);
            CodeGenerator(CodeGen, GenIndex, 04,*level - SymbolTable[idntaddr].level, SymbolTable[idntaddr].addr); //STO
            return;
        }
        
        /*Call*/
        if(program[*index].Token == callsym){
            (*index)++;
            /*Syntax Check: Check if call is followed by an identifier*/
            if(program[*index].Token != identsym){
                printf("Error: call must be followed by an identifier\n");
                exit(0);
            }
            
            /*Syntax Check: is this a declared Identifier*/
            int idntaddr = SymbolTableSearch(program[*index].ID, SymbolTable, *SymbolTableSize, SymbolTableIndex, level);           
            if(idntaddr == -1){
                printf("Error: undeclared identifier\n");
                exit(0);
            }

            /*Syntax Check: is the identifier a procedure*/
            if (SymbolTable[idntaddr].kind != 3){
                printf("Error: call must be followed by a procedure identifier\n");
                exit(0);
            }
            else{
                CodeGenerator(CodeGen, GenIndex, 05, *level - SymbolTable[idntaddr].level, SymbolTable[idntaddr].addr); //CAL
            }
            (*index)++;
            return;
        }

        /*Begin*/
        if(program[*index].Token == beginsym){
            do{
                (*index)++;
                Statement(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level);
            } while(program[*index].Token == semicolonsym); //Loop for statements inside of Begin

            /*Syntax Check: Check if begin is followed by an end*/
            if(program[*index].Token != endsym){
                printf("Error: begin must be followed by end\n");
                exit(0);
            }
            return;
        }

        /*IF*/
        if(program[*index].Token == ifsym){
            (*index)++;

            /*Condition for an If statement*/
            Condition(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level);
            
            /*Creating a reference to the index of this JPC to add correct M addr later*/
            int jpcIdx = *GenIndex;
            CodeGenerator(CodeGen, GenIndex, 8, 0, 0); //JPC
            
            /*Syntax Check: Check if an IF statement is followed by a THEN statement*/
            if(program[*index].Token != thensym){
                printf("Error: if must be followed by then\n");
                exit(0);
            }
            (*index)++;

            /*Statement after Then*/
            Statement(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level);

            /*Syntax Check: Check if an IF statement is followed by a Fi statement*/
            if(program[*index].Token != fisym){
                printf("Error: if must be followed by fi\n");
                exit(0);
            }
            (*index)++;
    
            /*Storing the correct address for the JPC command*/
            CodeGen[jpcIdx].M = 3 * *GenIndex;
            return;
        }

        /*WHILE*/
        if(program[*index].Token == whilesym){
            (*index)++;
            
            /*Creating a reference to the index of this JPC to add correct M addr later*/
            int loopIdx = *GenIndex;

            Condition(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level);

            /*Syntax Check: Check if an While statement is followed by a do statement*/
            if(program[*index].Token != dosym){
                printf("Error: while must be followed by do\n");
                exit(0);
            }
            (*index)++;
            
            /*Creating a reference to the index of this JPC to add correct M addr later*/
            int jpcIdx = *GenIndex;

            CodeGenerator(CodeGen, GenIndex, 8, 0, 0); //JPC
            Statement(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level);
            CodeGenerator(CodeGen, GenIndex, 7, 0, 3 * loopIdx); //JMP
            
            /*Storing the correct address for the JPC command*/
            CodeGen[jpcIdx].M = 3 * *GenIndex;
            return;
        }

        /*READ*/
        if(program[*index].Token == readsym){
            (*index)++;
            /*Syntax Check: Is there an identifier after the read statement*/
            if(program[*index].Token != identsym){
                printf("Error: read statements must be followed by identifier\n");
                exit(0);
            }

            /*Syntax Check: Has the Identifier already be declared*/
            int idntaddr = SymbolTableSearch(program[*index].ID, SymbolTable, *SymbolTableSize, SymbolTableIndex, level);           
            if(idntaddr == -1){
                printf("Error: undeclared identifier\n");
                exit(0);
            }

            /*Syntax Check: is the identifier a variable*/
            if(SymbolTable[idntaddr].kind != 2){
                printf("Error: only variable values may be altered\n");
                exit(0);
            }
            (*index)++;
            CodeGenerator(CodeGen, GenIndex, 9, 0, 2); //SYS
            CodeGenerator(CodeGen, GenIndex, 04, *level - SymbolTable[idntaddr].level, SymbolTable[idntaddr].addr); //STO
            return;
        }
        
        /*WRITE*/
        if(program[*index].Token == writesym){
            (*index)++;
            Expression(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level);
            CodeGenerator(CodeGen, GenIndex, 9, 0, 1); //SYS
            return;
        }
        
        return;
    }

/*RECURSION LAYER 4: CONDITIONS AND EXPRESSIONS*/
    void Condition(Code *CodeGen, int *GenIndex, int *index ,struct Lexeme program[], struct Symbol SymbolTable[], int *SymbolTableSize, int *SymbolTableIndex, int *level){
        /*ODD*/
        if(program[*index].Token == oddsym){
            (*index)++;
            Expression(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level);
            CodeGenerator(CodeGen, GenIndex, 02, 0, 11); //ODD
        }
        else{
            Expression(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level);
            
            if(program[*index].Token == eqlsym){
                (*index)++;
                Expression(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level);
                CodeGenerator(CodeGen, GenIndex, 02, 0, 5); //EQL
            }
            else if(program[*index].Token == neqsym){
                (*index)++;
                Expression(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level);
                CodeGenerator(CodeGen, GenIndex, 02, 0, 6); //NEQ
            }
            else if(program[*index].Token == lessym){
                (*index)++;
                Expression(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level);
                CodeGenerator(CodeGen, GenIndex, 02, 0, 7); //LES
            }
            else if(program[*index].Token == leqsym){
                (*index)++;
                Expression(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level);
                CodeGenerator(CodeGen, GenIndex, 02, 0, 8); //LEQ
            }
            else if(program[*index].Token == gtrsym){
                (*index)++;
                Expression(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level);
                CodeGenerator(CodeGen, GenIndex, 02, 0, 9); //GTR
            }
            else if(program[*index].Token == geqsym){
                (*index)++;
                Expression(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level);
                CodeGenerator(CodeGen, GenIndex, 02, 0, 10); //GEQ
            }
            else{
                /*Syntax Check: is there a condition operator*/
                printf("Error: condition must contain comparison operator\n");
                exit(0);
            }
        }
        return;
    }

    void Expression(Code *CodeGen, int *GenIndex, int *index ,struct Lexeme program[], struct Symbol SymbolTable[], int *SymbolTableSize, int *SymbolTableIndex, int *level){
        Term(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level);
        /*Loop for + and - expressions*/
        while(program[*index].Token == minussym || program[*index].Token == plussym){
            if(program[*index].Token == plussym){
                (*index)++;
                Term(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level);
                CodeGenerator(CodeGen, GenIndex, 02, 0, 1); //ADD
            }
            else if(program[*index].Token == minussym){
                (*index)++;
                Term(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level);
                CodeGenerator(CodeGen, GenIndex, 02, 0, 2); //SUB
            }
        }
        return;
    }

/*RECURSION LAYER 5: TERMS*/
    void Term(Code *CodeGen, int *GenIndex, int *index ,struct Lexeme program[], struct Symbol SymbolTable[], int *SymbolTableSize, int *SymbolTableIndex, int *level){
        Factor(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level);
        /*Loop for * and / expressions*/
        while(program[*index].Token == multsym || program[*index].Token == slashsym){
            if(program[*index].Token == multsym){
                (*index)++;
                Factor(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level);
                CodeGenerator(CodeGen, GenIndex, 02, 0, 3); //MUL
            }
            else if(program[*index].Token == slashsym){
                (*index)++;
                Factor(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level);
                CodeGenerator(CodeGen, GenIndex, 02, 0, 4); //DIV
            }
        }
        return;
    }

/*RECURSION LAYER 6: FACTORS*/
    void Factor(Code *CodeGen, int *GenIndex, int *index ,struct Lexeme program[], struct Symbol SymbolTable[], int *SymbolTableSize, int *SymbolTableIndex, int *level){
        if(program[*index].Token == identsym){
            /*Syntax Check: Has the Identifier already be declared*/
            int idntaddr;
            idntaddr = SymbolTableSearch(program[*index].ID, SymbolTable, *SymbolTableSize, SymbolTableIndex, level);           
            if(idntaddr == -1){
                printf("Error: undeclared identifier\n");
                exit(0);
            }

            if(SymbolTable[idntaddr].kind == 1){ //Const
                CodeGenerator(CodeGen, GenIndex, 01, 0, SymbolTable[idntaddr].val);// LIT
            } 
            else{
                CodeGenerator(CodeGen, GenIndex, 03, *level - SymbolTable[idntaddr].level, SymbolTable[idntaddr].addr); // LOD
            } 
            (*index)++;
        }
        else if(program[*index].Token == numbersym){
            CodeGenerator(CodeGen, GenIndex, 01, 0, atoi(program[*index].ID));// LIT
            (*index)++;
        }
        else if(program[*index].Token == lparentsym){
            (*index)++;
            Expression(CodeGen, GenIndex, index, program, SymbolTable, SymbolTableSize, SymbolTableIndex, level);
            
            /*Syntax Check: Check that parenthesis are closed*/
            if(program[*index].Token != rparentsym){
                printf("Error: right parenthesis must follow left parenthesis\n");
                exit(0);
            }
            (*index)++;
        }
        else{
            /*Syntax Check: Are equations formated correctly*/
            printf("Error: arithmetic equations must contain operands, parentheses, numbers, or symbols\n");
            exit(0);
        }
        return;
    }
    
/*A Search for Identifiers inside of the Symbol Table*/
int SymbolTableSearch(char identifier[], struct Symbol SymbolTable[], int SymbolTableSize, int *SymbolTableIndex, int *level){
    for(int j = SymbolTableSize - 1; j >= 0; j--) {
        if(strcmp(SymbolTable[j].name, identifier) == 0) {
            return j;
        } 
    }
    return -1;
}

/*Adds an Identifier into the SymbolTable*/
void SymbolTableAdd(struct Symbol SymbolTable[], int kind, char identifier[], int value, int *level, int *SymbolTableSize,  int mark, int *SymbolTableIndex){
    
    SymbolTable[*SymbolTableSize].kind = kind;
    strcpy(SymbolTable[*SymbolTableSize].name, identifier);
    SymbolTable[*SymbolTableSize].level = *level;
    SymbolTable[*SymbolTableSize].mark = mark;
    
    if(kind == 1 ){ //constants
        SymbolTable[*SymbolTableSize].addr = 0;
        SymbolTable[*SymbolTableSize].val = value;
        
    }
    else if(kind == 2){ //variables
        SymbolTable[*SymbolTableSize].addr = *SymbolTableIndex;
        SymbolTable[*SymbolTableSize].val = value;
        (*SymbolTableIndex)++;
    }
    else if(kind == 3){ //procedures
        SymbolTable[*SymbolTableSize].val = 0;
        SymbolTable[*SymbolTableSize].addr = value; //address is stored in value for procedures 
    }
    (*SymbolTableSize)++;
}

/*Function to populate the Code into an Array*/
void CodeGenerator(struct Code CodeGen[], int *GenIndex, int op, int l, int m){
    CodeGen[*GenIndex].OP = op;
    CodeGen[*GenIndex].L = l;
    CodeGen[*GenIndex].M = m;
    (*GenIndex)++;
}