#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <unistd.h>
#include <math.h>

static int x_dim = 0;
static int y_dim = 0;
static int new_x_dim = 0;
static int new_y_dim = 0;
static int num_galaxies = 0;

typedef struct {
  int x;
  int y;  
} coords_t;

static coords_t farthest;

int within_bounds(coords_t point)
{
  return (((point.x >= 0) && (point.x < farthest.x)) &&
	  ((point.y >= 0) && (point.y < farthest.y)));
}

int manhattan_distance(coords_t a, coords_t b)
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
    fprintf(stdout,"usage: ./day11_part1 <input_file>\n");
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
      fprintf(stdout,"Input (%3i)(size %3li): %s\n",y_dim,strlen(temp),temp);
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
    fprintf(stdout,"Matrix is %i x %i, galaxy # is %i\n",x_dim,y_dim,num_galaxies);
#endif
    rewind(file);

    new_x_dim = x_dim;
    new_y_dim = y_dim;
    
    int shift_x[x_dim]; 
    int shift_y[y_dim];
    memset(shift_x,0,sizeof(int)*x_dim);
    memset(shift_y,0,sizeof(int)*y_dim);

    coords_t galaxies[num_galaxies];
    memset(galaxies,0,sizeof(coords_t)*num_galaxies);
    int index = 0;
    
    //Determine expansion 
    for(int y = 0 ; y < y_dim; y++)
      {
	ret = getline(&buffer,&size,file);
	buffer[strlen(buffer)-1] = '\0'; //remove EOL character
	char *temp = calloc(strlen(buffer)+1,sizeof(char));
	memcpy(temp,buffer,strlen(buffer)+1);

	int ngalaxies = 0;
	for(int x = 0 ; x < x_dim ; x++){
	  if(temp[x] == '#'){
	    shift_x[x]++;
	    ngalaxies++;
	    galaxies[index].x = x;
	    galaxies[index].y = y;
#ifdef DEBUG
	    fprintf(stdout,"Galaxy %i present @( %i,% i) \n",index,x,y);
#endif
	    index++;
	  }
	}

	if(ngalaxies == 0){
#ifdef DEBUG
	  fprintf(stdout,"Line #%i must be doubled\n",y);
#endif
	  new_y_dim++;
	} else{
	  shift_y[y]++;
	}
	
	free(temp);
      }
    
    for(int x = 0 ; x < x_dim; x++){
      if (shift_x[x] == 0){
#ifdef DEBUG
	  fprintf(stdout,"Column #%i must be doubled\n",x);
#endif
	  new_x_dim++;
      }
    }
#ifdef DEBUG
    fprintf(stdout,"New Matrix is %i x %i, galaxy # is %i\n",new_x_dim,new_y_dim,num_galaxies);
    fprintf(stdout,"Shift X: ");
    print_bar('=', 20, stdout, 1);
    for(int x = 0 ; x < x_dim; x++){
      fprintf(stdout, " %i - ",shift_x[x]);
    }
    fprintf(stdout,"\n");
    print_bar('=', 20, stdout, 1);

    fprintf(stdout,"Shift Y: ");
    print_bar('=', 20, stdout, 1);
    for(int x = 0 ; x < y_dim; x++){
      fprintf(stdout, " %i - ",shift_y[x]);
    }
    fprintf(stdout,"\n");
    print_bar('=', 20, stdout, 1);
#endif

    farthest.x = new_x_dim;
    farthest.y = new_y_dim;
    //shift galaxies coordinates
    for(int idx = 0 ; idx < num_galaxies; idx++){      
#ifdef DEBUG
      fprintf(stdout,"========= Shifting Galaxy %3i @ (%3i, %3i)\n",idx,galaxies[idx].x,galaxies[idx].y);
#endif
      // shift X
      int x_shift = 0;
      for(int x = 0 ; x < galaxies[idx].x ; x++){
	if(shift_x[x] == 0){
	  x_shift++;
	}
      }
#ifdef DEBUG
      fprintf(stdout,"Galaxie #%i shifted on x %3i -> %3i (+%3i)\n",idx,galaxies[idx].x,galaxies[idx].x+x_shift,x_shift);
#endif
      galaxies[idx].x += x_shift;      
      // shift X
      int y_shift = 0;
      for(int y = 0 ; y < galaxies[idx].y ; y++){
	if(shift_y[y] == 0){
	  y_shift++;
	}
      }
#ifdef DEBUG
      fprintf(stdout,"Galaxie #%i shifted on y %3i -> %3i (+%3i)\n",idx,galaxies[idx].y,galaxies[idx].y+y_shift,y_shift);
#endif
      galaxies[idx].y += y_shift;      
    }

#ifdef DEBUG  
    //Allocate new map
    char map[new_x_dim][new_y_dim];
    memset(map,'.',sizeof(char)*new_x_dim*new_y_dim);
    //place galaxies in new map
    for(int idx = 0 ; idx < num_galaxies; idx++){      
      map[galaxies[idx].x][galaxies[idx].y] = (char)('0' + idx%10);
    }

    //print map
    fprintf(stdout,"Map : %i x %i\n",new_x_dim,new_y_dim);
    fprintf(stdout,"Map : %i x %i\n",farthest.x,farthest.y);
    print_bar('=', 20, stdout, 1);
    for(int y = 0 ; y < new_y_dim; y++){      
      fprintf(stdout,"[%3i] :",y);
      for(int x = 0 ; x < new_x_dim; x++){      
	fprintf(stdout,"%c",map[x][y]);
      }
      fprintf(stdout,"\n");
    }
    print_bar('=', 20, stdout, 1);

    int num_pairs = num_galaxies*(num_galaxies-1)/2;
    fprintf(stdout,"========== Num pairs = %i\n",num_pairs);
    for(int idx = 0 ; idx < num_galaxies ; idx++){
      fprintf(stdout,"Galaxy [%3i] @ (%3i, %3i) | %i\n",idx, galaxies[idx].x, galaxies[idx].y,within_bounds(galaxies[idx]));
    }
#endif
    
    int sum = 0;
    // no need to compute for last galaxy.
    for(int idx = 0 ; idx < (num_galaxies-1) ; idx++){      
      for(int idx2 = idx+1 ; idx2 < num_galaxies ; idx2++){
	sum += manhattan_distance(galaxies[idx],galaxies[idx2]);
      }
    }
    
    fprintf(stdout,"========== Sum = %i\n",sum);
    
    free(buffer);
    fclose(file);    
  }
  
  exit(EXIT_SUCCESS);
}

  
