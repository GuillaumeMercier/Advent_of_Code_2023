#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <unistd.h>
#include <math.h>


#define NMAX (32)
#define MIN(a,b) ((a)<(b) ? (a) : (b))

void print_bar(char c, int size, FILE *file, int flush)
{
  char str[size+1];
  memset(str,0,(size+1)*sizeof(char));
  for(int i = 0 ; i < size ;i++){
    sprintf(str+i,"%c",c);
  }
  
  fprintf(file,"%s%s",str,(flush) ? "\n" : "");
}

void print_matrix(char **matrix)
{
  print_bar('=',30,stdout,1);
  int index = 0;
  while(matrix[index]){
    fprintf(stdout,"[%2i]: %s\n",index,matrix[index]);
    index++;
  }
  print_bar('=',30,stdout,1);
}
  
void reset_matrix(char **matrix)
{
  for(int i = 0 ; i < NMAX ; i++){
    matrix[i] = NULL;
  }
}

void free_matrix(char **matrix)
{
  for(int i = 0 ; i < NMAX ; i++){
    free(matrix[i]);
  }
}

void print_col(char **matrix, int x , int y_max)
{
  print_bar('=',30,stdout,1);
  for(int y = 0 ; y < y_max ; y++){
    fprintf(stdout,"Col[%i][%i] = %c\n",x,y, (matrix[y])[x]);  
  }
  print_bar('=',30,stdout,1);
}
  
int compare_columns(char **matrix, int x1, int x2 , int y_max)
{
  for(int y = 0 ; y < y_max ; y++){
    if(((matrix[y])[x1] ) != ((matrix[y])[x2])) {
      return 0;
    }
  }
  return 1;
}

int find_y_candidates(char **matrix, int y_max, int *array)
{
  int index = 0;
  int candidate_y = 0;
  for(candidate_y = 0 ; candidate_y < (y_max-1) ; candidate_y++){
    if( strcmp(matrix[candidate_y], matrix[candidate_y+1]) == 0){
      array[index++] = candidate_y;
    }
  }
  return index;
}

int find_x_candidates(char **matrix, int x_max, int y_max, int *array)
{
  int index = 0;
  int candidate_x = 0;
  for(candidate_x = 0 ; candidate_x < (x_max-1) ; candidate_x++){
    if( compare_columns(matrix,candidate_x,candidate_x+1,y_max)){
      array[index++] = candidate_x;
    }
  }	  
  return index;
}

int scan_around_y(char **matrix,int candidate_y, int y_max)
{
  int max  = MIN(candidate_y, (y_max -1)-(candidate_y+1)) + 1;
  
  //check line around possible mirror
  int tries;
  for(tries = 0 ; tries < max  ; tries++){
#ifdef DEBUG
    fprintf(stdout,"Comparing Lines %i and %i\n", candidate_y-tries, candidate_y+1+tries);
#endif
    if(strcmp(matrix[candidate_y-tries], matrix[candidate_y+1+tries])){
#ifdef DEBUG
      fprintf(stdout,"Lines %i and %i don't match\n", candidate_y-tries, candidate_y+1+tries);
#endif
      return 0;
    }
  }
  if (tries == max){
#ifdef DEBUG
    fprintf(stdout,"Mirror confirmed, number of lines above axis is %i\n",candidate_y+1);
#endif
    return 100*(candidate_y+1);
  }
  return 0; 
}

int scan_around_x(char **matrix,int candidate_x, int x_max, int y_max)
{
  int max  = MIN(candidate_x, (x_max -1)-(candidate_x+1)) + 1;
  //check line around possible mirror
  int tries;
  for(tries = 0 ; tries < max  ; tries++){
#ifdef DEBUG
    fprintf(stdout,"Comparing Columns %i and %i\n", candidate_x-tries, candidate_x+1+tries);
#endif
    if(compare_columns(matrix,candidate_x-tries,candidate_x+1+tries,y_max) == 0){
#ifdef DEBUG
      fprintf(stdout,"Columns %i and %i don't match\n", candidate_x-tries, candidate_x+1+tries);
#endif
      return 0;
    }
  }
  
  if (tries == max){
#ifdef DEBUG
    fprintf(stdout,"Mirror confirmed, number of Columns before axis is %i\n",candidate_x+1);
#endif
    return (candidate_x+1);
  }
  return 0;
 }
		  
int main(int argc, char *argv[])
{
  if (argc < 2){
    fprintf(stdout,"usage: ./day11_part1 <input_file>\n");
    exit(EXIT_FAILURE);
  } else {    
    char *buffer = NULL;
    size_t size = 0;
    ssize_t ret = 0;
    
    FILE *file = fopen(argv[1],"r");
    assert(file);

    long int sum = 0;
    
    char *matrix[NMAX];
    reset_matrix(matrix);

    int x_max = 0;
    int y_max = 0;
    int index = 0;
    
    while ((ret = getline(&buffer,&size,file)) != -1) {      
      //fprintf(stdout,"=========== Matrix[%i]:(Line %i): %s\n",index,y_max,buffer);      
      if(*buffer != '\n'){
	buffer[strlen(buffer)-1] = '\0'; //remove EOL character
	x_max = strlen(buffer);
	matrix[y_max]= calloc(x_max+1,sizeof(char));
	memcpy(matrix[y_max],buffer,x_max+1);    	
	//fprintf(stdout,"Matrix[%i]:(Line %i): %s\n",index,y_max,matrix[y_max]);	       	
	y_max++;
      } else { 
	// handle the current matrix
#ifdef DEBUG
	fprintf(stdout,"======= Matrix %i is (%i x %i)\n",index,x_max,y_max);
	print_matrix(matrix);
#endif

	//scan vertically (=> can use strcmp)
	int candidate_y[y_max];
	int num_candidate_y = find_y_candidates(matrix,y_max,candidate_y);
	for(int i = 0 ; i < num_candidate_y ; i++){
#ifdef DEBUG
	  fprintf(stdout,"Candidate %3i (num %3i)\n",candidate_y[i],num_candidate_y);
#endif
	  sum += scan_around_y(matrix,candidate_y[i],y_max);
	}

	//scan horizontally (=> can use strcmp)
	int candidate_x[x_max];
	int num_candidate_x = find_x_candidates(matrix,x_max,y_max,candidate_x);
	for(int i = 0 ; i < num_candidate_x ; i++){
#ifdef DEBUG
	  fprintf(stdout,"Candidate %3i (num %3i)\n",candidate_y[i],num_candidate_y);
#endif
	  sum += scan_around_x(matrix,candidate_x[i],x_max,y_max);
	}

	//reset old matrix
	free_matrix(matrix);
	reset_matrix(matrix);
	x_max = 0;
	y_max = 0;
	index++;
      }
    }
    
    
    fprintf(stdout,"========== Sum = %li\n",sum);
    
    free(buffer);
    fclose(file);    
  }
  
  exit(EXIT_SUCCESS);
}

  
