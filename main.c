#include <stdio.h>
#include <stdlib.h>

#define  WEIGHTS_SIZE = 4;

int isEqual(char x , char y);
int isConservative(char x , char y);
int numOfChars(char* group);
int isSemiConservative(char x , char y);
char* changeMutant(char* semiSeq ,int seqLength,int idx);

int main()
{

  FILE *file;
  int seq_len;
  int numOfSemiSeq;
  char c;
  double weights[4];
  file = fopen("input.txt" , "r");
  if (file != NULL){
    printf("succes \n");
  }

  double num;
  int idx=0;
  while(fscanf(file, "%lf", &num) != '\0'){
        weights[idx] = num;
        printf("%lf \n" , weights[idx]);
        idx++;
  }

  //reading first sequence from file//
  char *sequence = (char*)malloc(sizeof(char)*1);
  c = fgetc(file);
  int buf_size = 1;
  while( c != '\n' && c != EOF){
        *(sequence+(buf_size-1)) = c;
        printf("%c",*(sequence+(buf_size-1)));
        buf_size++;
        sequence = (char*)realloc(sequence , buf_size*sizeof(char));
        c = fgetc(file);
  }

  buf_size++;
  sequence = (char*)realloc(sequence , buf_size*sizeof(char));
  *(sequence+(buf_size-1)) = '\0';

  //num of semi sequences
  fscanf(file, "%d", &numOfSemiSeq);
  printf("\n");
  printf("%d \n" , numOfSemiSeq);
  c = fgetc(file);

  //reading semi-sequences from file//
  char *semiSeq[numOfSemiSeq];
  for(int i=0;i<numOfSemiSeq;i++){
        buf_size=1;
        semiSeq[i] = malloc(buf_size * sizeof(char));
        c = fgetc(file);
        while( c != '\n' && c != EOF){
                *(semiSeq[i]+(buf_size-1)) = c;
                //printf("%c",*(semiSeq[i]+(buf_size-1)));
                buf_size++;
                semiSeq[i] = realloc(semiSeq[i] , buf_size*sizeof(char*));
                c = fgetc(file);
        }
        buf_size++;
        semiSeq[i] = realloc(semiSeq[i] , buf_size*sizeof(char*));
        *(semiSeq[i]+(buf_size-1)) = '\0';
        printf("\n");
  }

  //print sequnces
  for(int j=0;j < numOfSemiSeq;j++){
    printf("seq number %d \n" , j+1);
    printf("Array: %s\n", semiSeq[j]);
  }


  //primeSeqChar = *(semiSeq[0]+1);
  //printf("%c" , primeSeqChar);











  free(sequence);
  free(semiSeq);
  fclose(file);
  return 0;
}
int numOfChars(char* group){
    int count=0;
    while(*(group+count) != '\0'){
        count++;
    }
    return count;
}

int isEqual(char x , char y){
    if(x == y){
        return 1;
    }
    return 0;
}

int isConservative(char x , char y){
    char* conserGroups[9] = {"NDEQ","NEQK","STA","MILV","QHRK","NHQK","FYW","HY","MILF"};
    int count,groupSize=0;
    for(int i=0;i<9;i++){
        count=0;
        groupSize = numOfChars(*(conserGroups+i));
        for(int j=0;j<groupSize;j++){

            if(isEqual(x,*(*(conserGroups+i)+j))){
                    count++;
            }
            else if(isEqual(y,*(*(conserGroups+i)+j))){
                    count++;
            }
        }
        if(count == 2){
            return 1;
        }
    }
    return 0;
}

int isSemiConservative(char x , char y){
    char* semiConserGroups[11] = {"SAG","ATV","CSA","SGND","STPA","STNK","NEQHRK","NDEQHK","SNDEQK","HFY","FVLIM"};
    int count,groupSize=0;
    for(int i=0;i<11;i++){
        count=0;
        groupSize = numOfChars(*(semiConserGroups+i));
        for(int j=0;j<groupSize;j++){

            if(isEqual(x,*(*(semiConserGroups+i)+j))){
                    count++;
            }
            else if(isEqual(y,*(*(semiConserGroups+i)+j))){
                    count++;
            }
        }
        if(count == 2){
            return 1;
        }
    }
    return 0;
}

int calcScore(char* primeSeq , char* semiSeq , int* results){


  int lenPrimeSequence = numOfChars(primeSeq);
  int lenSemiSeq = numOfChars(semiSeq);

  int offset = lenPrimeSequence - lenSemiSeq;

  char primeSeqChar,semiSeqChar;

  for(int o=0;o<offset;o++){
        for(int i=0;i<lenSemiSeq;i++){
                char* mutant = changeMutant(semiSeq,lenSemiSeq,i+1);
                primeSeqChar = *(primeSeq+o+i);
                semiSeqChar = *(semiSeq+i);

                if(isEqual(primeSeqChar,semiSeqChar)){
                    (*(results))++;
                }
                else if(isConservative(primeSeqChar , semiSeqChar)){
                    (*(results+1))++;
                }
                else if(isSemiConservative(primeSeqChar,semiSeqChar)){
                    (*(results+2))++;

                }
        }
  }
  printf("* %d , : %d , .%d");
  return 0;
}

char* changeMutant(char* semiSeq ,int seqLength,int idx){

    char sign = '-';
    int semiSeqIdx=0;
    char* mutantSeq = malloc(sizeof(char)*(seqLength+1));
    for(int i=0;i<=seqLength;i++){
            if(idx==i){
                *(mutantSeq+i) = sign;

            }else{
                *(mutantSeq+i) = *(semiSeq+semiSeqIdx);
                semiSeqIdx++;
            }
    }
    return mutantSeq;
}
