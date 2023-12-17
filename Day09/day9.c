#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>

#define VEC_SIZE (21)

long int vector_reduction_part1(long int vector[], size_t size)
{
  long int reduction = 0;
  for(size_t idx = 0 ; idx < size ; idx++){
    reduction += vector[idx];
  }
  return reduction;
}

long int vector_reduction_part2(long int vector[], size_t size)
{
  long int reduction = 0;
  for(int idx = (size - 1) ; idx >= 0 ; idx--){
    reduction = (vector[idx] - reduction);
  }
  return reduction;
}

long int vector_null(long int vector[], size_t size)
{
  for(size_t idx = 0 ; idx < size ; idx++){
    if (vector[idx]){
      return 0;
    }
  }
  return 1;
}

void display_vec(long int vector[], size_t size, char *tag)
{
  fprintf(stdout,"================== Vector %s (%li) ==============\n",tag, size);
  for(size_t idx = 0 ; idx < size ; idx++){
    fprintf(stdout,"%li ",vector[idx]);
  }
  fprintf(stdout,"\n========================================\n");
}

long int *next_vector(long int vector[], size_t *size)
{
  long int *ptr = calloc(*size,sizeof(long int));

  for(size_t idx = 0 ; idx < *size ; idx++){
    ptr[idx] = vector[idx+1] - vector[idx];    
  }
  
  (*size)--;
  return ptr;
}

long int *str_2_vec(char str[], size_t *size)
{
  assert(str);  
  long int *ptr = calloc(strlen(str),sizeof(long int));
  *size = 0;
  while(str){    
    ptr[(*size)++] = atol(strsep(&str," "));
  }
  return ptr;
}



int main(int argc, char *argv[])
{
  if (argc < 3){
    fprintf(stdout,"usage: ./day9 <input_file> <mode>\n");
    //exit(EXIT_FAILURE);
  }

  char *buffer = NULL;
  size_t size = 0;
  ssize_t ret = 0;
  
  FILE *file = fopen(argv[1],"r");
  assert(file);
  
  int mode = (argc >= 3) ? atoi(argv[2]) : 1; // mode 1 by default
  
  long int sum = 0;
  
  while ((ret = getline(&buffer,&size,file)) != -1) {
    buffer[strlen(buffer)-1] = '\0'; //remove EOL character
    char *temp = calloc(strlen(buffer)+1,sizeof(char));
    memcpy(temp,buffer,strlen(buffer)+1);    
#ifdef DEBUG
    fprintf(stdout,"Input: %s\n",temp);
#endif
    size_t vec_size = 0;
    long int *ptr = str_2_vec(temp,&vec_size);
    
    long int save_state[VEC_SIZE];
    memset(save_state,0,VEC_SIZE*sizeof(long int));
    int save_state_index = 0;
    
    long int (*(f_ptr[2]))(long int[], size_t) = {vector_reduction_part1, vector_reduction_part2};
    
    do{
      long int *save = ptr;
      
#ifdef DEBUG
      display_vec(ptr,vec_size,"in loop");
#endif
      save_state[save_state_index++] = (mode == 1 ? ptr[vec_size-1] : ptr[0]);

#ifdef DEBUG
      fprintf(stdout,"Saved value ===================== %li\n",ptr[0]);
#endif
      ptr = next_vector(ptr,&vec_size);

      free(save);
    }while(!(ret = vector_null(ptr,vec_size)));

    save_state[save_state_index] = 0;
    
#ifdef DEBUG
    for(int idx = save_state_index  ; idx >= 0 ; idx--){
      fprintf(stdout," value to report == %li\n",save_state[idx]);
    }
#endif
    
    sum += f_ptr[mode-1](save_state, save_state_index+1);

#ifdef DEBUG
    fprintf(stdout,"===================== current sum %li\n",sum);
#endif

    free(ptr);    	
    free(temp);       
  }

  
  fprintf(stdout,"===================== sum %li\n",sum);
  
  free(buffer);
  fclose(file);    
  
  exit(EXIT_SUCCESS);
}

  
