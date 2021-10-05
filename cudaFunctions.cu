#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include "myFunctions.h"

__device__ int numOfChars(char* group){
    int count=0;
    while(*(group+count) != '\0'){
        count++;
    }
    return count;
}
__device__ int isEqual(char x, char y){
	if (x == y) {
			return 1;
		}
		return 0;
}

__device__ int isConservative(char x, char y) {
	char *conserGroups[9] = { "NDEQ", "NEQK", "STA", "MILV", "QHRK", "NHQK",
			"FYW", "HY", "MILF" };
	int count, groupSize = 0;
	for (int i = 0; i < 9; i++) {
		count = 0;
		groupSize = numOfChars(*(conserGroups + i));
		for (int j = 0; j < groupSize; j++) {

			if (isEqual(x, *(*(conserGroups + i) + j))) {
				count++;
			} else if (isEqual(y, *(*(conserGroups + i) + j))) {
				count++;
			}
		}
		if (count == 2) {
			return 1;
		}
	}
	return 0;
}

__device__ int isSemiConservative(char x, char y) {
	char *semiConserGroups[11] = { "SAG", "ATV", "CSA", "SGND", "STPA", "STNK",
			"NEQHRK", "NDEQHK", "SNDEQK", "HFY", "FVLIM" };
	int count, groupSize = 0;
	for (int i = 0; i < 11; i++) {
		count = 0;
		groupSize = numOfChars(*(semiConserGroups + i));
		for (int j = 0; j < groupSize; j++) {

			if (isEqual(x, *(*(semiConserGroups + i) + j))) {
				count++;
			} else if (isEqual(y, *(*(semiConserGroups + i) + j))) {
				count++;
			}
		}
		if (count == 2) {
			return 1;
		}
	}
	return 0;
}

__device__ double calcResult(double* results, double* weights) {

	double star = *(results);
	double colon = *(results + 1);
	double dot = *(results + 2);
	double space = *(results + 3);
	double score = star * weights[0] - colon * weights[1] - dot * weights[2] - space * weights[3];
	return score;
}
//each offset is calculating his score
__global__ void comparisonMutantPrime_gpu(char* primeSeq, char* mutant,
		int lenMutant, int offset, double* weights, double* offSetScores, int my_rank) {

	int tid = blockIdx.x * blockDim.x + threadIdx.x;//identify the current offset
	if(tid < lenMutant){
		char primeSeqChar;
		char mutantChar;
		double results[4] = {0, 0, 0, 0}; /*Sum each one of the equation variables {*, :, . ,""}*/
		
		for(int j=0;j<lenMutant;j++){
			primeSeqChar = *(primeSeq + tid + j);
			mutantChar = *(mutant + j);
			if (isEqual(primeSeqChar, mutantChar)) {
				//*
				results[0]++;
			} else if (isConservative(primeSeqChar, mutantChar)) {
				//:
				results[1]++;
			} else if (isSemiConservative(primeSeqChar, mutantChar)) {
				//.
				results[2]++;
			} else {
				//isSpace
				//" "
				results[3]++;
			}
		}
		double score = calcResult(results, weights);
		offSetScores[tid] = score;
	}
}

double ComputeOnGPU(char* primeSeq, char* mutant, double* weights, int possibleOffset, int tid, int my_rank, int* p_offset) {

	
	int lenPrimeSequence = strlen(primeSeq);
	int lenMutant = strlen(mutant);
	
	//Allocateing memory
	double *offsetScore = (double*)malloc(sizeof(double)*possibleOffset);
	double *d_offsetScore;
	cudaMalloc((void**) &d_offsetScore, sizeof(double) * possibleOffset);
	
	//Allocateing memory and trasfer data from CPU to GPU
	char *d_primeSeq;
	cudaMalloc((void**) &d_primeSeq, sizeof(char) * (lenPrimeSequence+1));
	cudaMemcpy(d_primeSeq, primeSeq, sizeof(char) * (lenPrimeSequence+1),
			cudaMemcpyHostToDevice);

	//Allocateing memory and trasfer data from CPU to GPU
	char* d_mutant;
	cudaMalloc((void**) &d_mutant, sizeof(char) * (lenMutant+1));
	cudaMemcpy(d_mutant, mutant, sizeof(char) * (lenMutant+1),
			cudaMemcpyHostToDevice);

	//Allocateing memory and trasfer data from CPU to GPU
	double *d_weights;
	cudaMalloc((void**) &d_weights, sizeof(double) * EQUATION_ELEMENTS);
	cudaMemcpy(d_weights, weights, sizeof(double) * EQUATION_ELEMENTS,
			cudaMemcpyHostToDevice);

	//Calc blocks and threads
	int block_size = 256;
	int grid_size = (possibleOffset + block_size - 1) / block_size;

	comparisonMutantPrime_gpu<<<grid_size,block_size>>>(d_primeSeq, d_mutant, lenMutant, possibleOffset,d_weights, d_offsetScore, my_rank);

	//Transfer data from GPU to CPU
	cudaMemcpy(offsetScore, d_offsetScore, sizeof(char) * possibleOffset,
			cudaMemcpyDeviceToHost);

	double bestScore = offsetScore[0];
	//p_offset = 0;
	for(int i=1;i<possibleOffset;i++){
		
		if(bestScore < offsetScore[i]){
			bestScore = offsetScore[i];
			//p_offset = i;	
		}
	}
	
	
	cudaFree(d_primeSeq);
	cudaFree(d_mutant);
	free(offsetScore);
	cudaFree(d_offsetScore);
	return bestScore;
}
