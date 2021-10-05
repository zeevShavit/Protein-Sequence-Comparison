build:
	mpicxx -fopenmp -c zeev.c -o zeev.o
	mpicxx -fopenmp -c cFunctions.c -o cFunctions.o
	nvcc -I./inc -c cudaFunctions.cu -o cudaFunctions.o
	mpicxx -fopenmp -o mpiCudaOpemMP  zeev.o cFunctions.o cudaFunctions.o  /usr/local/cuda-9.1/lib64/libcudart_static.a -ldl -lrt

clean:
	rm -f *.o ./mpiCudaOpemMP

run:
	mpiexec -np 4 ./mpiCudaOpemMP

runOn2:
	mpiexec -np 4 -machinefile  mf  -map-by  node  ./mpiCudaOpemMP
