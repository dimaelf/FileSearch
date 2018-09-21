#include <stdio.h>
#include <stdlib.h>


int compare(char* c1, char* c2, long N){
    for(long i = 0; i<N; i++){
        if (*(c1+i)!=*(c2+i)){
            return 0;
        }
    }
    return 1;
}


int ShowProgress(long filesize, long part, long BlockCount){
    long SzPerBl = filesize/part;
    if (SzPerBl == 0){
        printf("[##########          ] 50/100");
        return 0;
    }
    printf("\r[");
    for(int j = 0; j<20; j++){
        if (20*BlockCount/SzPerBl >= j){
            printf("#");
        }
        else{
            printf(" ");
        }
    }
    printf("] %li/100", 100*BlockCount/SzPerBl);
}


int PrintOffset(long BlockCount, int degree, long i, FILE* file){
    if (BlockCount == 0){
        fprintf(file, "%li \n ", i);
        return 0;
    }
    char format[] = "%li%02li \n";
    format[5] = degree+48;
    fprintf(file, format, BlockCount, i);
}


int main(int args, char* argv[]){

    int DegreeOfPart = 6; //степень 10, которая определяет размер считываемых блоков
    long part = 1;
    for (int i = 0; i<DegreeOfPart; i++){part*=10;}
    long BlockCount = 0;

    if (args != 5){
        printf("Incorrect amount of arguments arguments\n");
        return 0;
    }

    FILE *file1 = fopen(argv[1], "r");
    if (file1 == 0){
        printf("Error for reading the first file\n");
        return 0;
    }
    FILE *file2 = fopen(argv[2], "r");
    if (file2 == 0){
        printf("Error for reading the second file\n");
        return 0;
    }
    FILE *file3 = fopen(argv[3], "w");
    if (file3 == 0){
        printf("Error for reading the log file\n");
        return 0;
    }
    long N = atol(argv[4]);
    if ((N == 0)||(N>=part)){
        printf("Error for length of vectors\n");
        return 0;
    }

    char* SearchPlace = (char*)malloc(part+N);
    int NumRead = fread(SearchPlace, 1, part+N, file1);
    if (NumRead < N){
        printf("Too small first file\n");
        return 0;
    }
    char* SearchSeq = (char*)malloc(N+1);
    if (fread(SearchSeq, 1, N, file2) < N){
        printf("Too small second file\n");
        return 0;
    }
    *(SearchSeq+N) = '\0';

    printf("\r[                    ] 0/100");
    fprintf(file3, "Searching for %s in %s:\n", SearchSeq, argv[1]);
    fseek(file1, 0L, SEEK_END);
    long sz = ftell(file1);
    fseek(file1,0, SEEK_SET);

    do{
        NumRead = fread(SearchPlace, 1, part+N, file1);
        ShowProgress(sz, part, BlockCount);
        for (long i = 0; i<NumRead-N; i++){
            if (compare(SearchPlace+i, SearchSeq, N) == 1){
                PrintOffset(BlockCount, DegreeOfPart, i, file3);
            }
        }
        BlockCount++;
        fseek(file1, (-1)*N, SEEK_CUR);
    }while(NumRead == part+N);

    printf("\r[####################] 100/100");
    return 0;
}