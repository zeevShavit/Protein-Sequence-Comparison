#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <omp.h>
#include "myFunctions.h"
#include <ctype.h>







void resultsZeroLike(int* results, int arrayLen) {
	for (int i = 0; i < arrayLen; i++) {
		*(results + i) = 0;
	}
}

void changeMutant(char* semiSeq, char* mutantSeq, int idx) {

	char sign = '-';
	int semiSeqIdx = 1;
	int seqLength = strlen(semiSeq);
	*(mutantSeq + 0) = *(semiSeq + 0);
	for (int i = 1; i <= seqLength; i++) {
		if (idx == i) {
			*(mutantSeq + i) = sign;
		} else {
			*(mutantSeq + i) = *(semiSeq + semiSeqIdx);
			semiSeqIdx++;
		}
	}
	*(mutantSeq+seqLength) = '\0';
}

void readFromFile(double weights[], int *numOfSemiSeq, char **sequence,
		char ***semiSeq, char *filePath) {

	FILE *file;

	file = fopen(filePath, "r");
	if (file != NULL) {
		printf("File read successfully \n");
	}
	double num;
	int idx = 0;
	//Read weights
	while (fscanf(file, "%lf", &num) != '\0') {
		weights[idx] = num;
		idx++;
	}
	//Prime sequence
	*sequence = (char*) malloc(sizeof(char) * MAX_PRIME_SEQ);
	fscanf(file, "%s", *sequence);

	fscanf(file, "%d", numOfSemiSeq);

	//Semi_sequemces
	*semiSeq = (char**) malloc(*numOfSemiSeq * sizeof(char*));
	for (int i = 0; i < *numOfSemiSeq; i++) {
		(*semiSeq)[i] = (char*) malloc(MAX_SEMI_SEQ * sizeof(char));
		fscanf(file, "%s", (*semiSeq)[i]);
	}
	fclose(file);
}
