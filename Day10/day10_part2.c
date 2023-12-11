#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <unistd.h>

#define X_DIM (140)
#define Y_DIM (140)

#define COLOR_END    "\x1b[0m" // To flush out prev settings
#define COLOR_RED    "\x1b[31m"
#define COLOR_GREEN  "\x1b[32m"
#define COLOR_YELLOW "\x1b[93m"
#define COLOR_BLUE   "\x1b[34m"


#define PRINT_RED(c)    do{fprintf(stdout,"%s%c%s", COLOR_RED,(c),COLOR_END);}while(0);
#define PRINT_GREEN(c)  do{fprintf(stdout,"%s%c%s", COLOR_GREEN,(c),COLOR_END);}while(0);
#define PRINT_BLUE(c)   do{fprintf(stdout,"%s%c%s", COLOR_BLUE,(c),COLOR_END);}while(0);
#define PRINT_YELLOW(c) do{fprintf(stdout,"%s%c%s", COLOR_YELLOW,(c),COLOR_END);}while(0);

static ssize_t x_dim = 0; //X_DIM
static ssize_t y_dim = 0; //Y_DIM

//record the starting position
static ssize_t x_start = 0;
static ssize_t y_start = 0;

#define CONVERT_COORD_X( pos ) ((x_dim-1)-(pos))
#define CONVERT_COORD_Y( pos ) ((y_dim-1)-(pos))

typedef enum {
  POINT     = '.',
  PIPE_H    = '-',
  PIPE_V    = '|',
  CORNER_SE = 'F',
  CORNER_SW = '7',
  CORNER_NE = 'L',
  CORNER_NW = 'J',  
} type_t;

typedef struct {
  type_t type;
  ssize_t x;
  ssize_t y;
  int visited;
} point_t;

char *bar(int size)
{
  char *str = calloc(size,sizeof(char));
  for(int i = 0 ; i < size ;i++){
    sprintf(str+i,"%c",'=');
  }
  return str;
}

//fixme ugly Y_DIM
void print_map(point_t map[][Y_DIM],point_t *current, ssize_t max_x, ssize_t max_y)
{
  char *str = bar(max_x+11);
  fprintf(stdout,"%s\n",str);
  for(ssize_t y = max_y - 1 ; y >= 0 ; y--){
    fprintf(stdout,"Line (%3li): ",y);
    for(ssize_t x = max_x - 1 ; x >= 0 ; x--){
      if ( x == x_start && y == y_start){ 
	PRINT_RED(map[x][y].type);
	continue;
      }
      
      if((current) && ((x == current->x) && (y == current->y))){ 
	PRINT_YELLOW(map[x][y].type);
	continue;
      }      
      if( map[x][y].type == 'S' ){
	PRINT_RED(map[x][y].type); 
      } else if( map[x][y].visited != -1){
	PRINT_GREEN(map[x][y].type); 
      } else
	PRINT_BLUE(map[x][y].type); 
    }
    fprintf(stdout,"\n");    
  }
  fprintf(stdout,"%s\n",str);
  free(str);
}
// find next point based on current and previous location.
// FIXME ugly hardcoded Y_DIM
point_t *find_next_point(point_t map[][Y_DIM], point_t *current, point_t *previous, ssize_t max_x, ssize_t max_y)
{
  ssize_t x = -1;
  ssize_t y = -1;
  
  switch(current->type)
    {
    case PIPE_H: // -
      {
	if (((current->x)-1) == (previous->x)){ //can't go W , go E 
	  x = (current->x + 1);
	  y = (current->y);
	  assert(x < max_x);
	  assert(((map[x][y].type) == PIPE_H) ||
		 ((map[x][y].type) == CORNER_NE) ||
		 ((map[x][y].type) == CORNER_SE));
	} else {
	  if (((current->x)+1) == (previous->x)){ //can't go E, go W
	    x = (current->x - 1);
	    y = (current->y);
	    assert(x >= 0);
	    assert(((map[x][y].type) == PIPE_H) ||
		   ((map[x][y].type) == CORNER_NW) ||
		   ((map[x][y].type) == CORNER_SW));
	  }
	}
	break;
      }
    case PIPE_V: // |
      {
	if (((current->y)-1) == (previous->y)){ //can't go N, go S
	  x = (current->x);
	  y = (current->y + 1);
	  assert(y < max_y);
	  assert(((map[x][y].type) == PIPE_V) ||
		 ((map[x][y].type) == CORNER_SE) ||
		 ((map[x][y].type) == CORNER_SW));
	} else {
	  if (((current->y)+1) == (previous->y)){ //can't go S, go N
	    x = (current->x);
	    y = (current->y - 1);
	    assert(y >= 0);
	    assert(((map[x][y].type) == PIPE_V) ||
		   ((map[x][y].type) == CORNER_NE) ||
		   ((map[x][y].type) == CORNER_NW));
	  }
	}
	break;
      }
    case CORNER_SE: //F
      {
	if (((current->x)-1) == (previous->x)){ //can't go W, go N
	  x = (current->x);
	  y = (current->y - 1);
	  assert(y >= 0);
	  assert(((map[x][y].type) == PIPE_V) ||
		 ((map[x][y].type) == CORNER_NE) ||
		 ((map[x][y].type) == CORNER_NW));	  
	} else {
	  if (((current->y)-1) == (previous->y)){ //can't go N, go W
	    x = (current->x - 1);
	    y = (current->y);
	    assert(x >= 0);
	    assert(((map[x][y].type) == PIPE_H) ||
		   ((map[x][y].type) == CORNER_SW) ||
		   ((map[x][y].type) == CORNER_NW));
	  }
	}
	break;
      }
    case CORNER_SW: //7
      {
	if (((current->x)+1) == (previous->x)){ //can't go E, go N
	  x = (current->x);
	  y = (current->y - 1);
	  assert(y >= 0);
	  assert(((map[x][y].type) == PIPE_V) ||
		 ((map[x][y].type) == CORNER_NE) ||
		 ((map[x][y].type) == CORNER_NW));	  
	} else {
	  if (((current->y)-1) == (previous->y)){ //can't go N, go E
	    x = (current->x + 1);
	    y = (current->y);
	    assert(x < max_x);
	    assert(((map[x][y].type) == PIPE_H) ||
		   ((map[x][y].type) == CORNER_SE) ||
		   ((map[x][y].type) == CORNER_NE));
	  }
	}
	break;
      }
    case CORNER_NE: //L
      {
	if (((current->x)-1) == (previous->x)){ //can't go W, go S
	  x = (current->x);
	  y = (current->y + 1);
	  assert(y <  max_y);
	  assert(((map[x][y].type) == PIPE_V) ||
		 ((map[x][y].type) == CORNER_SE) ||
		 ((map[x][y].type) == CORNER_SW));	  
	} else {
	  if (((current->y)+1) == (previous->y)){ //can't go S, go W
	    x = (current->x - 1);
	    y = (current->y);
	    assert(x >= 0);
	    assert(((map[x][y].type) == PIPE_H) ||
		   ((map[x][y].type) == CORNER_SW) ||
		   ((map[x][y].type) == CORNER_NW));
	  }
	}
	break;
      }      
    case CORNER_NW: //J
      {
	if (((current->x)+1) == (previous->x)){ //can't go E, go S
	  x = (current->x);
	  y = (current->y + 1);
	  assert(y <  max_y);
	  assert(((map[x][y].type) == PIPE_V) ||
		 ((map[x][y].type) == CORNER_SE) ||
		 ((map[x][y].type) == CORNER_SW));	  
	} else {
	  if (((current->y)+1) == (previous->y)){ //can't go S, go E
	    x = (current->x + 1);
	    y = (current->y);
	    assert(x < max_x);
	    assert(((map[x][y].type) == PIPE_H) ||
		   ((map[x][y].type) == CORNER_SE) ||
		   ((map[x][y].type) == CORNER_NE));
	  }
	}	
	break;
      }      
    default:
      {
	fprintf(stdout,"Could not find a direction!\n");
	abort();
      }      
    }  
  return &(map[x][y]);
}


int main(int argc, char *argv[])
{
  if (argc < 2){
    fprintf(stdout,"usage: ./day10_part1 <input_file>\n");
    exit(EXIT_FAILURE);
  } else {    
    char *buffer = NULL;
    size_t size = 0;
    ssize_t ret = 0;
    
    FILE *file = fopen(argv[1],"r");
    assert(file);

    //Get map dimensions
    while ((ret = getline(&buffer,&size,file)) != -1) {
      buffer[strlen(buffer)-1] = '\0'; //remove EOL character
      char *temp = calloc(strlen(buffer)+1,sizeof(char));
      memcpy(temp,buffer,strlen(buffer)+1);    

      x_dim = strlen(temp);
#ifdef DEBUG
      //      fprintf(stdout,"Input (%3li)(size %3li): %s\n",139-y_dim,strlen(temp),temp);
      fprintf(stdout,"Input (%3li)(size %3li): ",(Y_DIM-1)-y_dim,strlen(temp));
      for(int idx = 0 ; idx < strlen(temp) ; idx++){
	if( temp[idx] == 'S'){
	  PRINT_RED(temp[idx]); 
	} else PRINT_BLUE(temp[idx]);
      }
      fprintf(stdout,"\n");
#endif
      free(temp);
      y_dim++;
    }
    fprintf(stdout,"Matrix is %li x %li\n",x_dim,y_dim);
    rewind(file);
    
    //init important points values
    ssize_t x_origin = x_dim - 1;
    ssize_t y_origin = y_dim - 1;
    
    assert(x_origin == (X_DIM -1));
    assert(y_origin == (Y_DIM -1));

    //ssize_t x_max = 0;
    ssize_t y_max = 0;
    
    //allocate map
    point_t map[x_dim][y_dim];
    memset(map,0,x_dim*y_dim*sizeof(point_t));

    //get map and starting point
    // reverse y -> don't need to convert this coordinate
    for(ssize_t y = y_origin ; y >= y_max ; y--){
      ret = getline(&buffer,&size,file);
      buffer[strlen(buffer)-1] = '\0'; //remove EOL character
      char *temp = calloc(strlen(buffer)+1,sizeof(char));
      char *base = temp;
      memcpy(temp,buffer,strlen(buffer)+1);    

      //need to convert x 
      for(ssize_t x = 0 ; x < x_origin ; x++){ 
	map[CONVERT_COORD_X(x)][y].type = temp[x];
	map[CONVERT_COORD_X(x)][y].x = CONVERT_COORD_X(x);
	map[CONVERT_COORD_X(x)][y].y = y;
	map[CONVERT_COORD_X(x)][y].visited = -1;

	if (temp[x] == 'S'){
	  y_start = y;
	  x_start = CONVERT_COORD_X(x);
#ifdef DEBUG
	  fprintf(stdout,"Found starting position @ (%li, %li) \n",x_start, y_start);
#endif	  
	}
      }
      free(base);
    }   
#ifdef DEBUG 
    print_map(map,NULL,x_dim,y_dim);
#endif
    point_t *current = &(map[x_start][y_start]);    
    current->type    = CORNER_NE; // Given the input map, 'S' is actually an 'L'
    current->visited = 1;  

    //Given the input, the previous point is @: 
#ifdef REVERSE
    point_t *previous = &(map[x_start-1][y_start]);
#else
    point_t *previous = &(map[x_start][y_start+1]);
#endif
    previous->visited = 1; // this forces a direction in the loop.
    point_t *saved_current = NULL;
				    
    long int num_steps = 0;
#ifdef DEBUG
    print_map(map,NULL,x_dim,y_dim);
#endif
    //do/discover the loop
    do {
      saved_current = current;
      current = find_next_point(map, current, previous, x_dim, y_dim);
      current->visited = 1;
#ifdef DEBUG
      print_map(map,current,x_dim,y_dim);
      usleep(100);
#endif
      previous = saved_current;
      num_steps++;
    }while(!((current->x == x_start)&&(current->y == y_start)));

    num_steps = (num_steps/2) + (num_steps%2); //in case there is an odd number of steps ...

    print_map(map,NULL,x_dim,y_dim);
    fprintf(stdout,"================= num_steps : %li\n",num_steps);
    
    free(buffer);
    fclose(file);    
  }
  
  exit(EXIT_SUCCESS);
}

  
