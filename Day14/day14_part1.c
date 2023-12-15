#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <unistd.h>
#include <math.h>

void my_compare_and_swap(char *curr, char *next)
{
  assert(curr);
  assert(next);
  if (*next != '#'){
    if (*curr > *next){  // ok b/c 'O' > '.'
      char temp = *curr;
      *curr = *next;
      *next = temp;
    }
  }
}

void my_sort(char array[], int size)
{
  if (size > 1) {
    for (int i = 0; i < size -1 ; i++){
      my_compare_and_swap(array+i,array+i+1);
    }
    my_sort(array,size-1);
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

int main(int argc, char *argv[])
{
  if (argc < 2){
    fprintf(stdout,"usage: ./day14_part1 <input_file>\n");
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
      char *temp = calloc((x_max+1),sizeof(char)); 
      memcpy(temp,buffer,x_max+1);    	
      fprintf(stdout,"(Line %3i): %s\n",y_max,temp);      
      free(temp);
      y_max++;
    }
    rewind(file);
    fprintf(stdout,"========== Matrix is (%3i)x(%3i)\n",x_max,y_max);


    char data[x_max][y_max+1]; //+1 to store \0 in rotate mode 
    memset(data,0,sizeof(char)*(x_max+1)*y_max); // \0 everywhere

    //rotate matrix so that columns can be handled
    //directly as arrays
    for(int y = y_max-1; y >= 0  ; y--){
      ret = getline(&buffer,&size,file);
      if( ret != -1){	
	for(int x = 0 ; x < x_max ; x++){
	  data[x][y] = buffer[x];
	}	
      }
    }

    for(int y = 0 ; y < y_max ; y++){
      fprintf(stdout,"Data %3i :",y);
      for(int x = 0 ; x < x_max ; x++){
	fprintf(stdout,"%c", data[x][y]);
      }
      fprintf(stdout,"\n");
    }
    
    //data[x] is the array of chars corresponding to column #x
    //in the original output
    // sort array to pout stones to the right
    for(int x = 0 ; x < x_max ; x++){
#ifdef DEBUG
      print_bar('=',10,stdout,0);
      fprintf(stdout," Managing array %i\n",x);
      fprintf(stdout,"Orig  : %s\n",data[x]);
      my_sort(data[x], y_max);     
      fprintf(stdout,"Sorted: %s\n",data[x]);
#else
      my_sort(data[x], y_max);
#endif
    }

    print_bar('=',20,stdout,1);
    for(int y = 0 ; y < y_max ; y++){
      fprintf(stdout,"Sorted %3i :",y);
      for(int x = 0 ; x < x_max ; x++){
	fprintf(stdout,"%c", data[x][y]);
      }
      fprintf(stdout,"\n");
    }
    
    //compute load
    for(int y = 0 ; y < y_max ; y++){
      for(int i = 0 ; i < x_max ; i++){
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

  
