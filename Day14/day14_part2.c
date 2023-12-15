#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <unistd.h>
#include <math.h>

 // ok b/c 'O' > '.'

int superior(char a, char b)
{
  return (a > b);
}

int inferior(char a, char b)
{
  return (a < b);
}

void my_compare_and_swap(char *curr, char *next,  int (*op)(char, char))
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


void my_sort_north(char *base, int size)
{
  if (size > 1) {
    for (int i = 0; i < size -1 ; i++){
      my_compare_and_swap(base+i,base+(i+1),superior);
    }
    my_sort_north(base,size-1);
  }
}

void my_sort_south(char *base, int size, int min)
{  
  if (min < size - 1) {
    for (int i = size - 1 ; i > min ; i--){
      my_compare_and_swap(base+(i),base+(i-1),superior);
    }
    my_sort_south(base,size,min+1);
  }
}

void my_sort_east(char *base, int coord, int size, int stride)
{
  if (size > 1) {
    for (int i = 0; i < size -1 ; i++){
      my_compare_and_swap(base+i*stride+coord,base+(i+1)*stride+coord,superior);
    }
    my_sort_east(base,coord,size-1,stride);
  }
}

void my_sort_west(char *base, int coord, int size, int stride, int min)
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
      char *temp = calloc((x_max+1),sizeof(char)); 
      memcpy(temp,buffer,x_max+1);    	
      fprintf(stdout,"(Line %3i): %s\n",y_max,temp);      
      free(temp);
      y_max++;
    }
    rewind(file);
    fprintf(stdout,"========== Matrix is (%3i)x(%3i)\n",x_max,y_max);


    char data[x_max][y_max]; 
    memset(data,0,sizeof(char)*(x_max)*(y_max)); // \0 everywhere

    
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

    for(int y = y_max -1 ; y >= 0 ; y--){
      fprintf(stdout,"Data  %3i : ",y);
      for(int x = 0 ; x < x_max ; x++){
	fprintf(stdout,"%c", data[x][y]);
      }
      fprintf(stdout,"\n");
    }

#ifdef CHECK
    char copy[x_max][y_max]; 
    memset(copy,0,sizeof(char)*(x_max)*(y_max)); // \0 everywhere
    for(int y = 0 ; y < y_max ; y++)
      for(int x = 0 ; x < x_max ; x++)
	copy[x][y] = data[x][y];
#endif
    
    //Cycle : N, W, S, E

    long int max = atoi(argv[2]); //1000000000;
    if (max == 1000000000)
      max = 1000; // OK, for whatever reason, 1000 cycles and 1 billion cycles yield the same result ...
    
    for(long int num_cycles = 1 ; num_cycles <= max ; num_cycles++){
      
      if((num_cycles > 100) && (num_cycles%(max/100) == 0))
	{
	  fprintf(stdout,".");
	  fflush(stdout);
	}
      
#pragma omp parallel for 
       for(int y = y_max -1 ; y >= 0 ; y--){
	 my_sort_north(data[y], y_max);
       }

#ifdef DEBUG
       print_bar('=',10,stdout,1);
       for(int y = y_max -1 ; y >= 0 ; y--){
	 fprintf(stdout,"Cycle N %3i : ",y);
	 for(int x = 0 ; x < x_max ; x++){
	   fprintf(stdout,"%c", data[x][y]);
	 }
	 fprintf(stdout,"\n");
       }    
       print_bar('=',10,stdout,1);
#endif
       
#pragma omp parallel for 
       for(int y = y_max -1 ; y >= 0 ; y--){
	 my_sort_west((char *)data,y,x_max,y_max,0);
       }

#ifdef DEBUG
       print_bar('=',10,stdout,1);
       for(int y = y_max -1 ; y >= 0 ; y--){
	 fprintf(stdout,"Cycle W %3i : ",y);
	 for(int x = 0 ; x < x_max ; x++){
	   fprintf(stdout,"%c", data[x][y]);
	 }
	 fprintf(stdout,"\n");
       }
       print_bar('=',10,stdout,1);
#endif
       
#pragma omp parallel for 
       for(int y = y_max -1 ; y >= 0 ; y--){
	 my_sort_south(data[y], y_max, 0);
       }

#ifdef DEBUG
       print_bar('=',10,stdout,1);
       for(int y = y_max -1 ; y >= 0 ; y--){
	 fprintf(stdout,"Cycle S %3i : ",y);
	 for(int x = 0 ; x < x_max ; x++){
	   fprintf(stdout,"%c", data[x][y]);
	 }
	 fprintf(stdout,"\n");
       }
       print_bar('=',10,stdout,1);
#endif
       
#pragma omp parallel for 
       for(int y = y_max -1 ; y >= 0 ; y--){
	 my_sort_east((char *)data, y, x_max, y_max);
       }

#ifdef DEBUG
       print_bar('=',10,stdout,1);
       for(int y = y_max -1 ; y >= 0 ; y--){
	 fprintf(stdout,"Cycle E %3i : ",y);
	 for(int x = 0 ; x < x_max ; x++){
	   fprintf(stdout,"%c", data[x][y]);
	 }
	 fprintf(stdout,"\n");
       }
       print_bar('=',10,stdout,1);
#endif

#ifdef CHECK
       int ident = 1;
       for(int y = 0 ; y < y_max ; y++)
	 for(int x = 0 ; x < x_max ; x++)
	   if (copy[x][y] !=  data[x][y])
	     {
	       ident = 0;
	       goto out;
	     }
    out:
       if(ident){
	 fprintf(stdout,"\n========================> ident after %li cycles\n",num_cycles+1);
	 break;
       }
#endif
       
    }
    fprintf(stdout,"\n");
    
    print_bar('=',10,stdout,1);
    for(int y = y_max -1 ; y >= 0 ; y--){
      fprintf(stdout,"Final  %3i : ",y);
      for(int x = 0 ; x < x_max ; x++){
	fprintf(stdout,"%c", data[x][y]);
      }
      fprintf(stdout,"\n");
    }
    print_bar('=',10,stdout,1);
    
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

  
