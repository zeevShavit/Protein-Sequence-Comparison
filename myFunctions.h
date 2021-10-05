
#define DATA_TYPE_SIZE 3
#define EQUATION_ELEMENTS 4
#define MAX_PRIME_SEQ 3000
#define MAX_SEMI_SEQ  2000



void changeMutant(char *semiSeq, char *mutantSeq, int idx);
double ComputeOnGPU(char *primeSeq, char *mutant, double *weights, int possibleOffset,int tid, int my_rank, int* p_offset);
void resultsZeroLike(int* results, int arrayLen);
void readFromFile(double weights[], int *numOfSemiSeq, char **sequence,
		char ***semiSeq, char *filePath);
