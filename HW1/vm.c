//Dylan Money & Ryan Richards

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//constant
#define ARRAY_SIZE 500

//Given L Function
int base(int bp, int L, int pas[])
{
  int arb = bp; // arb = activation record base
  while ( L > 0) //find base L levels down
  {
    arb = pas[arb];
    L--;
  }
  return arb;
}

int main(int argc, char *argv[])
{
    //initiate variables
    int pas[ARRAY_SIZE], activated[100]; 
    int value, count = 0, pc = 0, sp = 0, bp = 499, done = 1, command = 0, runtime = 1, op, L, M, output = 1, j = 0; 

    
    //open file
    FILE *inputFilePointer;
    FILE *outputFilePointer;

    char *inputfile;
    
    // error if arguments are not passed correctly from the command line
    if (argc < 2)
    {
      printf("ERROR: Not enough arguments from command line\n");
      exit(1);
    }
    else //initialize input file
    {
      inputfile = argv[1];
      printf("File: %s\n", inputfile);
      inputFilePointer = fopen(inputfile, "r");
    }
    
    //initialize output file
    outputFilePointer = fopen("VMOutput.txt", "w");
    
    //input file error
    if (!inputFilePointer)
    {
      printf("Failed to open file\n");
      exit(1);
    }

    //initialize array
    for(int i = 0; i < ARRAY_SIZE; i++)
    {
      pas[i] = 0; 
    }

    //Take values from the input file and pass them to the PAS
    while (fscanf(inputFilePointer, "%d", &value) != EOF) 
    {
      if (count >= ARRAY_SIZE) {
        printf("PAS is Full\n");
        break;
      }
      pas[count] = value;
      count++;
    }
    
    //initiate sp
    sp = bp + 1;
    
    //close file
    fclose(inputFilePointer);

    //print init 
    printf("\t\t     PC   BP    SP   \tstack\n");
    printf("Initial Values: %6d%6d%6d\n", pc, bp, sp);
    printf("\n");
    
    command = bp;
    
    //iterate through commands
    while(runtime == 1)
    {
      op = pas[pc];
      L = pas[pc+1];
      M = pas[pc+2];
      pc = pc + 3;
      done = 0;
      switch(op)
      {
        case 1://LIT
          sp = sp - 1;
          pas[sp] = M;
          printf("\tLIT %2d  %2d  %2d%6d%6d", L, M, pc, bp, sp);
          break;
        case 2://OPR
          switch(M)
          {
            case 0://RTN
              sp = bp + 1;
              bp = pas[sp - 2];
              pc = pas[sp - 3];
              done = 1;
              printf("\tRTN %2d  %2d  %2d%6d%6d", L, M, pc, bp, sp);
              break;
            case 1://ADD
              pas[sp + 1] = pas[sp + 1] + pas[sp];
              sp = sp + 1;
              printf("\tADD %2d  %2d  %2d%6d%6d", L, M, pc, bp, sp);
              break;
            case 2://SUB
              pas[sp + 1] = pas[sp + 1] - pas[sp];
              sp = sp + 1;
              printf(" \tSUB %2d  %2d  %2d%6d%6d", L, M, pc, bp, sp);
              break;
            case 3://MUL
              pas[sp + 1] = pas[sp + 1] * pas[sp];
              sp = sp + 1;
              printf("\tMUL %2d  %2d  %2d%6d%6d", L, M, pc, bp, sp);
              break;
            case 4://DIV
              pas[sp + 1] = pas[sp + 1] / pas[sp];
              sp = sp + 1;
              printf("\tDIV %2d  %2d  %2d%6d%6d", L, M, pc, bp, sp);
              break;
            case 5://EQL
              pas[sp + 1] = pas[sp + 1] == pas[sp];
              sp = sp + 1;
              printf("\tEQL %2d  %2d  %2d%6d%6d", L, M, pc, bp, sp);
              break;
            case 6://NEQ
              pas[sp + 1] = pas[sp + 1] != pas[sp];
              sp = sp + 1;
              printf("\tNEQ %2d  %2d  %2d%6d%6d", L, M, pc, bp, sp);
              break;
            case 7://LSS
              pas[sp + 1] = pas[sp + 1] < pas[sp];
              sp = sp + 1;
              printf("\tLSS %2d  %2d  %2d%6d%6d", L, M, pc, bp, sp);
              break;
            case 8://LEQ
              pas[sp + 1] = pas[sp + 1] <= pas[sp];
              sp = sp + 1;
              printf("\tLEQ %2d  %2d  %2d%6d%6d", L, M, pc, bp, sp);
              break;
            case 9://GTR
              pas[sp + 1] = pas[sp + 1] > pas[sp];
              sp = sp + 1;
              printf("\tGTR %2d  %2d  %2d%6d%6d", L, M, pc, bp, sp);
              break;
            case 10://GEQ
              pas[sp + 1] = pas[sp + 1] >= pas[sp];
              sp = sp + 1;
              printf("\tGEQ %2d  %2d  %2d%6d%6d", L, M, pc, bp, sp);
              break;
            default:
              printf("Error: OPR NOT FOUND\n");
              runtime = 0;
              break;
          }
          break;
        case 3: //LOD 
          sp = sp - 1;
          pas[sp] = pas[base(bp, L, pas) - M];
          printf("\tLOD %2d  %2d  %2d%6d%6d", L, M, pc, bp, sp);
          break;
        case 4: //STO
          pas[base(bp, L, pas) - M] = pas[sp];
          sp = sp + 1;
          printf("\tSTO %2d  %2d  %2d%6d%6d", L, M, pc, bp, sp);
          break;
        case 5: //CAL
          pas[sp - 1] = base(bp,L, pas);
          pas[sp - 2] = bp;
          pas[sp - 3] = pc;
          activated[j] = sp - 1;
          bp = sp - 1;
          pc = M;
          printf("\tCAL %2d  %2d  %2d%6d%6d", L, M, pc, bp, sp);
          break;
        case 6: //INC
          sp = sp - M;
          printf("\tINC %2d  %2d  %2d%6d%6d", L, M, pc, bp, sp);
          break;
        case 7: //JMP
          pc = M;
          printf("\tJMP %2d  %2d  %2d%6d%6d", L, M, pc, bp, sp);
          break;
        case 8: //JPC
          if(pas[sp] ==  0)
            pc = M;
          sp = sp + 1;
          printf("\tJPC %2d  %2d  %2d%6d%6d", L, M, pc, bp, sp);
          break;
        case 9: //SYS
          switch(M)
          {
            case 1: //Output
              printf("Output result is: %d\n", pas[sp]);
              sp = sp + 1;
              printf("\tSYS %2d  %2d  %2d%6d%6d", L, M, pc, bp, sp);
              break;
            case 2://Input
              sp = sp - 1;
              printf("Please enter an integer: ");
              scanf("%d", &pas[sp]);
              printf("\tSYS %2d  %2d  %2d%6d%6d", L, M, pc, bp, sp);
              break;
            case 3://Stop
              printf("\tSYS %2d  %2d  %2d%6d%6d", L, M, pc, bp, sp);
              runtime = 0;
              break;
            default:
              printf("Error: SYS NOT FOUND\n");
              runtime = 0;
              break;
          }
          break;
        default:
          printf("Error: OP NOT FOUND\n");
          runtime = 0;
          break;
        }
        
        //print stack
        printf("     ");
        fprintf(outputFilePointer, "     ");
        for(int i = command; i >= sp; i--)
        {
          for(int y = 0; y < j; y++)
          {
            if(i == activated[y] )
              {
                printf(" | ");
                fprintf(outputFilePointer, " | ");
              }
          }
          printf("%4d", pas[i]); 
          fprintf(outputFilePointer, "%4d", pas[i]); 
        }
        printf("\n");
        fprintf(outputFilePointer, "\n");
        j++;
    }

    fclose(outputFilePointer);
    return 0;
}