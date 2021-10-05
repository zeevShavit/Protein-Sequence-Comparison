#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <omp.h>
#include "myFunctions.h"

#define BEST_SCORE_TYPES {MPI_DOUBLE, MPI_INT, MPI_INT}
#define BEST_SCORE_BLOCK_LENGTH {1,1,1}
#define BEST_SCORE_NUM_ATTRIBUTES 3

typedef struct BestScore_struct {
	double score;
	int offset;
	int mutantIndex;
} BestScore;



void parallelSolution(char *primeSeq, char **semiSeq, int semiSeqLen,
		double *weights, int argc, char *argv[]);
BestScore calcScore(char *primeSeq, char *semiSeq, int my_rank, double *weights);
void createDataType(MPI_Datatype *bestScoreType);


int main(int argc, char *argv[]) {

	char* filePath = "/home/linuxu/Downloads/MPI+mp+cuda/input.txt";
	char* primeSeq;
	char** semiSeq;
	int semiSeqLen;
	double weights[EQUATION_ELEMENTS];

	readFromFile(weights, &semiSeqLen, &primeSeq, &semiSeq, filePath);
	parallelSolution(primeSeq, semiSeq, semiSeqLen, weights, argc, argv);
	return 0;
}

void parallelSolution(char *primeSeq, char **semiSeq, int semiSeqLen,
		double *weights, int argc, char *argv[]) {

	int my_rank; /* rank of process */
	int p; /* number of processes */
	int tag = 0;
	MPI_Status status;

	BestScore *BestScores = (BestScore*) malloc(sizeof(BestScore) * EQUATION_ELEMENTS); /*Array to store the best score for each sequence*/
	BestScore score;
	char semiSequence[MAX_SEMI_SEQ]; /*storage for message*/

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	MPI_Datatype bestScoreType;
	createDataType(&bestScoreType);

	MPI_Scatter(BestScores, 1, bestScoreType, &score, 1, bestScoreType, 0,
	MPI_COMM_WORLD);

	if (my_rank == 0) {

		for (int i = 1; i < p; i++) {
			MPI_Send(semiSeq[i], strlen(semiSeq[i]) + 1, MPI_CHAR, i, tag,
			MPI_COMM_WORLD);
		}
		score = calcScore(primeSeq, semiSeq[my_rank], my_rank, weights);
		BestScores[my_rank] = score;
	} else {
		MPI_Recv(semiSequence, MAX_SEMI_SEQ, MPI_CHAR, 0, tag, MPI_COMM_WORLD,
				&status);
		score = calcScore(primeSeq, semiSequence, my_rank, weights);
		BestScores[my_rank] = score;
	}

	MPI_Gather(&score, 1, bestScoreType, BestScores, 1, bestScoreType, 0,
	MPI_COMM_WORLD);

	printf("process %d returned score of %f and mutant idx %d \n", my_rank,
			BestScores[my_rank].score, BestScores[my_rank].mutantIndex);
	if(my_rank == 0){
		free(BestScores);
	}
	MPI_Finalize();
}

BestScore calcScore(char *primeSeq, char *semiSeq, int my_rank, double* weights) {

	int lenPrimeSequence = strlen(primeSeq);
	int lenSemiSeq = strlen(semiSeq);

	int my_threads = lenSemiSeq;

	//private variables
	char* mutant;
	int i;
	int tid;
	double score;

	//shared variables
	int offset = lenPrimeSequence - lenSemiSeq;
	double* Scores = (double*)malloc(sizeof(double)*lenSemiSeq);/*Holds best score for each mutant*/
	//int* allOffsets = (int*)malloc(sizeof(int)*lenSemiSeq);/*Holds p_offset for each mutant*/

	BestScore bestscore;
	int* p_offset;/*Pointer to retrieve the offset that gives the best score*/
#pragma omp parallel for num_threads(my_threads) shared(primeSeq, semiSeq, weights, offset, Scores) private(i, mutant, tid, score, p_offset) 
	for ( i = 0; i < lenSemiSeq; i++) {
			
		tid = omp_get_thread_num();
		
		mutant = (char*)malloc(sizeof(char)* (lenSemiSeq + 1));
		changeMutant(semiSeq, mutant, i + 1);
		
		score = ComputeOnGPU(primeSeq, mutant, weights, offset, tid , my_rank, p_offset);
		Scores[tid] = score;
		//allOffsets[tid] = *p_offset;
	}

	bestscore.score = Scores[0];
	//bestscore.offset = allOffsets[0];
	for(int k=1;k<lenSemiSeq;k++){
		if(bestscore.score < Scores[k]){
			bestscore.score = Scores[k];
			bestscore.mutantIndex = k;
			//bestscore.offset = allOffsets[k];
		}
	}
	free(Scores);
	return bestscore;
}

void createDataType(MPI_Datatype *bestScoreType) {

	int blocklengths[DATA_TYPE_SIZE] = BEST_SCORE_BLOCK_LENGTH;
	MPI_Datatype types[DATA_TYPE_SIZE] = BEST_SCORE_TYPES;
	MPI_Aint offsets[DATA_TYPE_SIZE];
	offsets[0] = offsetof(BestScore, score);
	offsets[1] = offsetof(BestScore, offset);
	offsets[2] = offsetof(BestScore, mutantIndex);
	MPI_Type_create_struct(DATA_TYPE_SIZE, blocklengths, offsets, types, bestScoreType);
	MPI_Type_commit(bestScoreType);
}
