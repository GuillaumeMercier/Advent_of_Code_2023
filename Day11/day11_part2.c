#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <unistd.h>
#include <math.h>

static long long int x_dim = 0;
static long long int y_dim = 0;
static long long int new_x_dim = 0;
static long long int new_y_dim = 0;
static long long int num_galaxies = 0;

#ifndef ROW
#define ROW 1000000
#endif
//there is one column/row already present, hence -1
#define SHIFT_VAL (ROW-1) 

typedef struct {
  long long int x;
  long long int y;  
} coords_t;

static coords_t farthest;

int within_bounds(coords_t point)
{
  return (((point.x >= 0) && (point.x < farthest.x)) &&
	  ((point.y >= 0) && (point.y < farthest.y)));
}

long long int manhattan_distance(coords_t a, coords_t b)
{
#ifdef DEBUG
  assert(within_bounds(a));
  assert(within_bounds(b));
#endif
  return (abs(a.x - b.x) + abs(a.y - b.y));
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
    fprintf(stdout,"usage: ./day11_part2 <input_file>\n");
    exit(EXIT_FAILURE);
  } else {    
    char *buffer = NULL;
    size_t size = 0;
    ssize_t ret = 0;
    
    FILE *file = fopen(argv[1],"r");
    assert(file);
    
    //Get map dimensions and galaxies #
    while ((ret = getline(&buffer,&size,file)) != -1) {
      buffer[strlen(buffer)-1] = '\0'; //remove EOL character
      char *temp = calloc(strlen(buffer)+1,sizeof(char));
      memcpy(temp,buffer,strlen(buffer)+1);    
      
      x_dim = strlen(temp);
#ifdef DEBUG
      fprintf(stdout,"Input (%3lli)(size %3li): %s\n",y_dim,strlen(temp),temp);
#endif
      for(int idx = 0 ; idx < strlen(temp) ; idx++){
	if( temp[idx] == '#'){
	  num_galaxies++;
	} 
      }  
      free(temp);
      y_dim++;
    }
#ifdef DEBUG
    fprintf(stdout,"Matrix is %lli x %lli, galaxy # is %lli\n",x_dim,y_dim,num_galaxies);
#endif
    rewind(file);

    new_x_dim = x_dim;
    new_y_dim = y_dim;
    
    long long int shift_x[x_dim]; 
    long long int shift_y[y_dim];
    memset(shift_x,0,sizeof(long long int)*x_dim);
    memset(shift_y,0,sizeof(long long int)*y_dim);

    coords_t galaxies[num_galaxies];
    memset(galaxies,0,sizeof(coords_t)*num_galaxies);

    //Determine expansion 
    long long int index = 0;    
    for(long long int y = 0 ; y < y_dim; y++)
      {
	ret = getline(&buffer,&size,file);
	buffer[strlen(buffer)-1] = '\0'; //remove EOL character
	char *temp = calloc(strlen(buffer)+1,sizeof(char));
	memcpy(temp,buffer,strlen(buffer)+1);

	int ngalaxies = 0;
	for(long long int x = 0 ; x < x_dim ; x++){
	  if(temp[x] == '#'){
	    shift_x[x]++;
	    ngalaxies++;
	    galaxies[index].x = x;
	    galaxies[index].y = y;
#ifdef DEBUG
	    fprintf(stdout,"Galaxy %lli present @( %lli, %lli) \n",index,x,y);
#endif
	    index++;
	  }
	}

	if(ngalaxies == 0){
#ifdef DEBUG
	  fprintf(stdout,"Line #%lli must be doubled\n",y);
#endif
	  new_y_dim += SHIFT_VAL;
	} else{
	  shift_y[y]++;
	}
	
	free(temp);
      }
    
    for(long long int x = 0 ; x < x_dim; x++){
      if (shift_x[x] == 0){
#ifdef DEBUG
	  fprintf(stdout,"Column #%lli must be doubled\n",x);
#endif
	  new_x_dim += SHIFT_VAL;
      }
    }
#ifdef DEBUG
    fprintf(stdout,"New Matrix is %lli x %lli, galaxy # is %lli\n",new_x_dim,new_y_dim,num_galaxies);
    fprintf(stdout,"Shift X: ");
    print_bar('=', 20, stdout, 1);
    for(int x = 0 ; x < x_dim; x++){
      fprintf(stdout, " %lli - ",shift_x[x]);
    }
    fprintf(stdout,"\n");
    print_bar('=', 20, stdout, 1);

    fprintf(stdout,"Shift Y: ");
    print_bar('=', 20, stdout, 1);
    for(int x = 0 ; x < y_dim; x++){
      fprintf(stdout, " %lli - ",shift_y[x]);
    }
    fprintf(stdout,"\n");
    print_bar('=', 20, stdout, 1);
#endif
    
    farthest.x = new_x_dim;
    farthest.y = new_y_dim;
        
    //shift galaxies coordinates
    for(long long int idx = 0 ; idx < num_galaxies; idx++){      
#ifdef DEBUG
      fprintf(stdout,"========= Shifting Galaxy %3lli @ (%3lli, %3lli)\n",idx,galaxies[idx].x,galaxies[idx].y);
#endif
      // shift X
      long long int x_shift = 0;
      for(long long int x = 0 ; x < galaxies[idx].x ; x++){
	if(shift_x[x] == 0){
	  x_shift += SHIFT_VAL;
	}
      }
#ifdef DEBUG
      fprintf(stdout,"Galaxie #%lli shifted on x %3lli -> %3lli (+%3lli)\n",idx,galaxies[idx].x,galaxies[idx].x+x_shift,x_shift);
#endif
      galaxies[idx].x += x_shift;      
      // shift X
      long long int y_shift = 0;
      for(long long int y = 0 ; y < galaxies[idx].y ; y++){
	if(shift_y[y] == 0){
	  y_shift += SHIFT_VAL;
	}
      }
#ifdef DEBUG
      fprintf(stdout,"Galaxie #%lli shifted on y %3lli -> %3lli (+%3lli)\n",idx,galaxies[idx].y,galaxies[idx].y+y_shift,y_shift);
#endif
      galaxies[idx].y += y_shift;      
    }

#ifdef DEBUG
    long long int num_pairs = num_galaxies*(num_galaxies-1)/2;
    fprintf(stdout,"========== Num pairs = %lli\n",num_pairs);
    for(long long int idx = 0 ; idx < num_galaxies ; idx++){
      fprintf(stdout,"Galaxy [%3lli] @ (%3lli, %3lli) | %i\n",idx, galaxies[idx].x, galaxies[idx].y,within_bounds(galaxies[idx]));
    }
#endif
    
    long long int sum = 0;
    // no need to compute for last galaxy.
    for(long long int idx = 0 ; idx < (num_galaxies-1) ; idx++){      
      for(long long int idx2 = idx+1 ; idx2 < num_galaxies ; idx2++){
	sum += manhattan_distance(galaxies[idx],galaxies[idx2]);
      }
    }
    
    fprintf(stdout,"========== Sum = %lli\n",sum);
    
    free(buffer);
    fclose(file);    
  }
  
  exit(EXIT_SUCCESS);
}

  
