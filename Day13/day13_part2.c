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

//returns the number of differents characters
int compare_lines(char **matrix, int y1, int y2 , int x_max)
{
  int diff = 0;
#ifdef DEBUG
  int x_to_change = 0;
#endif
  for(int x = 0 ; x < x_max ; x++){
    if(((matrix[y1])[x] ) != ((matrix[y2])[x])) {
#ifdef DEBUG
      x_to_change = x;
#endif
      diff++;
    }
  }
  //Found smudge
#ifdef DEBUG
  if (1 == diff){    
    fprintf(stdout,"==============>>>>>>> Found Potential Smudge on line @ (%i, %i <-> %i)\n", x_to_change,y1,y2);
  }
#endif
  
  return diff;
}

//returns the number of differents characters
int compare_columns(char **matrix, int x1, int x2 , int y_max)
{
  int diff = 0;
#ifdef DEBUG
  int y_to_change;
#endif
  for(int y = 0 ; y < y_max ; y++){
    if(((matrix[y])[x1] ) != ((matrix[y])[x2])) {
#ifdef DEBUG
      y_to_change = y;
#endif
      diff++;
    }
  }
#ifdef DEBUG
  //Found smudge
  if (1 == diff){    
    fprintf(stdout,"==============>>>>>>> Found Potential Smudge on column @ (%i <-> %i, %i)\n",x1,x2, y_to_change);    
  }
#endif
  
  return diff;
}

int main(int argc, char *argv[])
{
  if (argc < 2){
    fprintf(stdout,"usage: ./day13_part2 <input_file>\n");
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

	//scan lines
	//find smudge on y axis
	int candidate_y = 0;
	for( candidate_y = 0 ; candidate_y < (y_max - 1) ; candidate_y++){
	  int max  = MIN(candidate_y, (y_max -1)-(candidate_y+1)) + 1;
	  int tries;
	  int diff = 0;
	  for(tries = 0 ; tries < max  ; tries++){
	    diff += compare_lines(matrix, candidate_y-tries, candidate_y+1+tries, x_max);
	  }
	  
	  if( 1 == diff ){// found smudge 
	    int add = 100*(candidate_y+1); 
	    sum += add;
#ifdef DEBUG
	    fprintf(stdout,"============================= Adding value %i\n",add);
#endif
	    goto end;
	  }
#ifdef DEBUG
	  else {
	    fprintf(stdout,"====Smudge not valid \n");
	  }
#endif
	}
	
	//scan columns
	//find smudge on x axis
	int candidate_x = 0;
	for( candidate_x = 0 ; candidate_x < (x_max - 1) ; candidate_x++){
	  int max  = MIN(candidate_x, (x_max -1)-(candidate_x+1)) + 1;
	  int tries;
	  int diff = 0;
	  for(tries = 0 ; tries < max  ; tries++){
	    diff += compare_columns(matrix, candidate_x-tries, candidate_x+1+tries, y_max);
	  }
	  
	  if( 1 == diff ){// found smudge 
	    int add = (candidate_x+1); 
	    sum += add;
#ifdef DEBUG
	    fprintf(stdout,"============================= Adding value %i\n",add);
#endif
	    goto end;
	  }
#ifdef DEBUG
	  else {
	    fprintf(stdout,"====Smudge not valid\n");
	  }
#endif
	}
	
      end:
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

  
