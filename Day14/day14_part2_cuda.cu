#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <unistd.h>
#include <math.h>
#include <cuda.h>
#include <cuda_runtime.h>

#ifndef N
#define N (100)
#endif

__device__ int superior(char a, char b)
{
  return (a > b);
}

__device__ int inferior(char a, char b)
{
  return (a < b);
}

__device__ void my_compare_and_swap(char curr[N], char *next,  int (*op)(char, char))
{
#ifdef DEBUG
  assert(curr);
  assert(next);
#endif		
  if (*next != '#'){
    if (op(*curr, *next)){ 
      char temp = *curr;
      *curr = *next;
      *next = temp;
    }
  }
}
__device__ void my_sort_north(char *base, int size)
{
  if (size > 1) {
    for (int i = 0; i < size -1 ; i++){
      my_compare_and_swap(base+i,base+(i+1),superior);
    }
    my_sort_north(base,size-1);
  }
}

__device__ void my_sort_south(char *base, int size, int min)
{  
  if (min < size - 1) {
    for (int i = size - 1 ; i > min ; i--){
      my_compare_and_swap(base+(i),base+(i-1),superior);
    }
    my_sort_south(base,size,min+1);
  }
}

__device__ void my_sort_east(char *base, int coord, int size, int stride)
{
  if (size > 1) {
    for (int i = 0; i < size -1 ; i++){
      my_compare_and_swap(base+i*stride+coord,base+(i+1)*stride+coord,superior);
    }
    my_sort_east(base,coord,size-1,stride);
  }
}

__device__ void my_sort_west(char *base, int coord, int size, int stride, int min)
{  
  if (min < size - 1) {
    for (int i = size - 1 ; i > min ; i--){
      my_compare_and_swap(base+i*stride+coord,base+(i-1)*stride+coord,superior);
    }
    my_sort_west(base,coord,size,stride,min+1);
  }
}
  
void print_bar(char c, int size, FILE *file, int flush)
{
  char str[size+1];
  memset(str,0,(size+1)*sizeof(char));
  for(int i = 0 ; i < size ;i++){
    sprintf(str+i,"%c",c);
  }
  
  fprintf(file,"%s%s",str,(flush) ? "\n" : "");
}

__global__ void sortN(char *base, int size) {
  int tid = threadIdx.x;  
  
  if (tid < size){    
   my_sort_north((base+tid*size), size);
  }
}

__global__ void sortW(char *base, int size) {
  int tid = threadIdx.x;    
  if (tid < size){
    my_sort_west(base,tid,size,size,0);
  }
}

__global__ void sortS(char *base, int size) {
  int tid = threadIdx.x;    
   if (tid < size){
    my_sort_south((base+tid*size), size, 0);
   }
}

__global__ void sortE(char *base, int size) {
  int tid = threadIdx.x;    
  if (tid < size){
    my_sort_east(base, tid, size, size);
  }
}

int main(int argc, char *argv[])
{
  if (argc < 3){
    fprintf(stdout,"usage: ./day14_part2 <input_file> <num_cycles>\n");
    exit(EXIT_FAILURE);
  } else {    
    char *buffer = NULL;
    size_t size = 0;
    ssize_t ret = 0;
    
    FILE *file = fopen(argv[1],"r");
    assert(file);

    long int load = 0;

    int x_max = 0;
    int y_max = 0;
    
    while ((ret = getline(&buffer,&size,file)) != -1) {      
      buffer[strlen(buffer)-1] = '\0'; //remove EOL character
      x_max = strlen(buffer);
      char *temp = (char *)calloc((N+1),sizeof(char)); 
      memcpy(temp,buffer,N+1);    	
      fprintf(stdout,"(Line %3i): %s\n",N,temp);      
      free(temp);
      y_max++;
    }
    rewind(file);
    fprintf(stdout,"========== Matrix is (%3i)x(%3i)\n",N,N);


    char data[N][N];
    memset(data,0,sizeof(char)*(N)*(N)); // \0 everywhere

    char copy[N][N]; 
    memset(copy,0,sizeof(char)*(N)*(N)); // \0 everywhere
    
    //rotate matrix so that columns can be handled
    //directly as arrays
    for(int y = (N-1) ; y >= 0  ; y--){
      ret = getline(&buffer,&size,file);
      if( ret != -1){	
	for(int x = 0 ; x < N ; x++){
	  data[x][y] = buffer[x];
	}	
      }
    }

    for(int y = N-1 ; y >= 0 ; y--){
      fprintf(stdout,"Data  %3i : ",y);
      for(int x = 0 ; x < N ; x++){
	fprintf(stdout,"%c", data[x][y]);
      }
      fprintf(stdout,"\n");
    }

    for(int y = 0 ; y < N ; y++)
      for(int x = 0 ; x < N ; x++)
	copy[x][y] = data[x][y];

    char *gpu_data, *gpu_copy;
    
    // Allocate device memory 
    cudaMalloc((void**)&gpu_data, N * N);
    cudaMalloc((void**)&gpu_copy, N * N);

    // Transfer data from host to device memory
    cudaMemcpy(gpu_data, data, sizeof(char) * N * N, cudaMemcpyHostToDevice);
    cudaMemcpy(gpu_copy, copy, sizeof(char) * N * N, cudaMemcpyHostToDevice);


    //Cycle : N, W, S, E

    long int max = atoi(argv[2]); //1000000000;

    for(long int num_cycles = 0 ; num_cycles < max ; num_cycles++){

      if((num_cycles > 100) &&(num_cycles%(max/100) == 0))
	{
	  fprintf(stdout,".");
	  fflush(stdout);
	}

      sortN <<<1,N>>> (gpu_data, N);
      sortW <<<1,N>>> (gpu_data, N);	
      sortS <<<1,N>>> (gpu_data, N);	
      sortE <<<1,N>>> (gpu_data, N);	
    }

    cudaMemcpy(data, gpu_data, sizeof(char) * N * N, cudaMemcpyDeviceToHost);
    
    print_bar('=',10,stdout,1);
    for(int y = N -1 ; y >= 0 ; y--){
      fprintf(stdout,"Final  %3i : ",y);
      for(int x = 0 ; x < N ; x++){
	fprintf(stdout,"%c", data[x][y]);
      }
      fprintf(stdout,"\n");
    }
    print_bar('=',10,stdout,1);
    
    //compute load
    for(int y = 0 ; y < N ; y++){
      for(int i = 0 ; i < N ; i++){
	if( (data[i][y]) == 'O'){
	  load += (y+1);
	}	
      }
    }
    
    fprintf(stdout,"========== Load = %li\n",load);


    free(buffer);
    fclose(file);    
  }
  
  exit(EXIT_SUCCESS);
}

  
