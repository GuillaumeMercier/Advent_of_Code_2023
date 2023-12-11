#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <unistd.h>

//#define X_DIM (20) //(140)
//#define Y_DIM (10) //(140)

#define COLOR_END    "\x1b[0m" // To flush out prev settings
#define COLOR_RED    "\x1b[31m"
#define COLOR_GREEN  "\x1b[32m"
#define COLOR_YELLOW "\x1b[93m"
#define COLOR_BLUE   "\x1b[34m"
#define COLOR_ORANGE "\x1b[33m"
#define COLOR_WHITE  "\x1b[37m"

#define PRINT_RED(c)    do{fprintf(stdout,"%s%c%s", COLOR_RED,(c),COLOR_END);}while(0);
#define PRINT_GREEN(c)  do{fprintf(stdout,"%s%c%s", COLOR_GREEN,(c),COLOR_END);}while(0);
#define PRINT_BLUE(c)   do{fprintf(stdout,"%s%c%s", COLOR_BLUE,(c),COLOR_END);}while(0);
#define PRINT_YELLOW(c) do{fprintf(stdout,"%s%c%s", COLOR_YELLOW,(c),COLOR_END);}while(0);
#define PRINT_ORANGE(c) do{fprintf(stdout,"%s%c%s", COLOR_ORANGE,(c),COLOR_END);}while(0);
#define PRINT_WHITE(c)  do{fprintf(stdout,"%s%c%s", COLOR_WHITE,(c),COLOR_END);}while(0);

static ssize_t x_dim = 0; //set to X_DIM in main
static ssize_t y_dim = 0; //set to Y_DIM in main

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
  int is_ok;
} point_t;

void print_bar(char c, int size, FILE *file, int flush)
{
  char str[size+1];
  memset(str,0,(size+1)*sizeof(char));
  for(int i = 0 ; i < size ;i++){
    sprintf(str+i,"%c",c);
  }
  
  fprintf(file,"%s%s",str,(flush) ? "\n" : "");
}

void print_map(point_t *map,point_t *current, ssize_t max_x, ssize_t max_y)
{
  print_bar('=',max_x+12,stdout,1);
  for(ssize_t y = max_y - 1 ; y >= 0 ; y--){
    fprintf(stdout,"Line (%3li): ",y);
    for(ssize_t x = max_x - 1 ; x >= 0 ; x--){
      if ( x == x_start && y == y_start){ 
	PRINT_RED((*(map + max_y*x + y)).type);
      } else if((current) && ((x == current->x) && (y == current->y))){ 
	PRINT_YELLOW((*(map + max_y*x + y)).type);
      } else if( (*(map + max_y*x + y)).type  == 'S'){
	PRINT_RED((*(map + max_y*x + y)).type);
      } else if( (*(map + max_y*x + y)).visited != -1){
	PRINT_GREEN((*(map + max_y*x + y)).type);
      } else if ((*(map + max_y*x + y)).is_ok == 1){
	PRINT_WHITE((*(map + max_y*x + y)).type);
      } else if ((*(map + max_y*x + y)).visited == -1 ){	
	PRINT_BLUE((*(map + max_y*x + y)).type);
      } else { // never reached
	PRINT_ORANGE((*(map + max_y*x + y)).type);
      }
    }
    fprintf(stdout,"\n");    
  }
  print_bar('=',max_x+12,stdout,1);
}

// find next point based on current and previous location.
point_t *find_next_point(point_t *map, point_t *current, point_t *previous, ssize_t max_x, ssize_t max_y)
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
#ifdef DEBUG
	  assert(x < max_x);
	  assert((((*(map + max_y*x + y)).type) == PIPE_H) ||
		 (((*(map + max_y*x + y)).type) == CORNER_NE) ||
		 (((*(map + max_y*x + y)).type) == CORNER_SE));
#endif
	} else {
	  if (((current->x)+1) == (previous->x)){ //can't go E, go W
	    x = (current->x - 1);
	    y = (current->y);
#ifdef DEBUG
	    assert(x >= 0);
	    assert((((*(map + max_y*x + y)).type) == PIPE_H) ||
		   (((*(map + max_y*x + y)).type) == CORNER_NW) ||
		   (((*(map + max_y*x + y)).type) == CORNER_SW));
#endif
	  }
	}
	break;
      }
    case PIPE_V: // |
      {
	if (((current->y)-1) == (previous->y)){ //can't go N, go S
	  x = (current->x);
	  y = (current->y + 1);
#ifdef DEBUG
	  assert(y < max_y);
	  assert((((*(map + max_y*x + y)).type) == PIPE_V) ||
		 (((*(map + max_y*x + y)).type) == CORNER_SE) ||
		 (((*(map + max_y*x + y)).type) == CORNER_SW));
#endif
	} else {
	  if (((current->y)+1) == (previous->y)){ //can't go S, go N
	    x = (current->x);
	    y = (current->y - 1);
#ifdef DEBUG
	    assert(y >= 0);
	    assert((((*(map + max_y*x + y)).type) == PIPE_V) ||
		   (((*(map + max_y*x + y)).type) == CORNER_NE) ||
		   (((*(map + max_y*x + y)).type) == CORNER_NW));
#endif
	  }
	}
	break;
      }
    case CORNER_SE: //F
      {
	if (((current->x)-1) == (previous->x)){ //can't go W, go N
	  x = (current->x);
	  y = (current->y - 1);
#ifdef DEBUG
	  assert(y >= 0);
	  assert((((*(map + max_y*x + y)).type) == PIPE_V) ||
		 (((*(map + max_y*x + y)).type) == CORNER_NE) ||
		 (((*(map + max_y*x + y)).type) == CORNER_NW));
#endif
	} else {
	  if (((current->y)-1) == (previous->y)){ //can't go N, go W
	    x = (current->x - 1);
	    y = (current->y);
#ifdef DEBUG
	    assert(x >= 0);
	    assert((((*(map + max_y*x + y)).type) == PIPE_H) ||
		   (((*(map + max_y*x + y)).type) == CORNER_SW) ||
		   (((*(map + max_y*x + y)).type) == CORNER_NW));
#endif
	  }
	}
	break;
      }
    case CORNER_SW: //7
      {
	if (((current->x)+1) == (previous->x)){ //can't go E, go N
	  x = (current->x);
	  y = (current->y - 1);
#ifdef DEBUG
	  assert(y >= 0);
	  assert((((*(map + max_y*x + y)).type) == PIPE_V) ||
		 (((*(map + max_y*x + y)).type) == CORNER_NE) ||
		 (((*(map + max_y*x + y)).type) == CORNER_NW));
#endif
	} else {
	  if (((current->y)-1) == (previous->y)){ //can't go N, go E
	    x = (current->x + 1);
	    y = (current->y);
#ifdef DEBUG
	    assert(x < max_x);
	    assert((((*(map + max_y*x + y)).type) == PIPE_H) ||
		   (((*(map + max_y*x + y)).type) == CORNER_SE) ||
		   (((*(map + max_y*x + y)).type) == CORNER_NE));
#endif
	  }
	}
	break;
      }
    case CORNER_NE: //L
      {
	if (((current->x)-1) == (previous->x)){ //can't go W, go S
	  x = (current->x);
	  y = (current->y + 1);
#ifdef DEBUG
	  assert(y <  max_y);
	  assert((((*(map + max_y*x + y)).type) == PIPE_V) ||
		 (((*(map + max_y*x + y)).type) == CORNER_SE) ||
		 (((*(map + max_y*x + y)).type) == CORNER_SW));
#endif
	} else {
	  if (((current->y)+1) == (previous->y)){ //can't go S, go W
	    x = (current->x - 1);
	    y = (current->y);
#ifdef DEBUG
	    assert(x >= 0);
	    assert((((*(map + max_y*x + y)).type) == PIPE_H) ||
		   (((*(map + max_y*x + y)).type) == CORNER_SW) ||
		   (((*(map + max_y*x + y)).type) == CORNER_NW));
#endif
	  }
	}
	break;
      }      
    case CORNER_NW: //J
      {
	if (((current->x)+1) == (previous->x)){ //can't go E, go S
	  x = (current->x);
	  y = (current->y + 1);
#ifdef DEBUG
	  assert(y <  max_y);
	  assert((((*(map + max_y*x + y)).type) == PIPE_V) ||
		 (((*(map + max_y*x + y)).type) == CORNER_SE) ||
		 (((*(map + max_y*x + y)).type) == CORNER_SW));
#endif
	} else {
	  if (((current->y)+1) == (previous->y)){ //can't go S, go E
	    x = (current->x + 1);
	    y = (current->y);
#ifdef DEBUG
	    assert(x < max_x);
	    assert((((*(map + max_y*x + y)).type) == PIPE_H) ||
		   (((*(map + max_y*x + y)).type) == CORNER_SE) ||
		   (((*(map + max_y*x + y)).type) == CORNER_NE));
#endif
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
  return &((*(map + max_y*x + y)));
}

int is_inside(point_t *map, point_t *point, ssize_t max_x, ssize_t max_y)
{
#ifdef DEBUG
  fprintf(stdout,"=========> checking for tile %c @ (%3li, %3li) (%i, %i) \n",
	  point->type,point->x,point->y,point->in_loop_x,point->in_loop_y);
#endif
  //count walls on E, W, S and N directions.
  // X axis
  type_t previous = POINT;
  int num_walls = 0;
  for(ssize_t x = 0 ; x < max_x ; x++){
    if (x == point->x ){
#ifdef DEBUG
      fprintf(stdout,"Found %i walls on W side for pt[%c](%li, %li)\n",num_walls,point->type,point->x,point->y);
#endif
      if((num_walls%2) == 0){
	return 0;
      } else {
	num_walls = 0;
      }
    } else {
      if((*(map + max_y*x + point->y)).visited == 1){
	switch( (*(map + max_y*x + point->y)).type  )//map[x][point->y].type)
	  {
	  case PIPE_V:
	    {
	      num_walls++;
	      continue;
	    }
	  case CORNER_SE: //F
	  case CORNER_SW: //7
	    {
	      if(previous == POINT){
		previous = (*(map + max_y*x + point->y)).type; //map[x][point->y].type;
	      } else if ((previous == CORNER_NE) || (previous == CORNER_NW)){
		num_walls++;
		previous = POINT;
	      } else if ((previous == CORNER_SE) || (previous == CORNER_SW)){	    
		previous = POINT;
	      }
	      continue;
	    }
	  case CORNER_NW: //J
	  case CORNER_NE: //L
	    {
	      if(previous == POINT){
		previous = (*(map + max_y*x + point->y)).type; //map[x][point->y].type;
	      } else if ((previous == CORNER_SE) || (previous == CORNER_SW)){
		num_walls++;
		previous = POINT;
	      } else if ((previous == CORNER_NE) || (previous == CORNER_NW)){	    	    
		previous = POINT;
	      }
	      continue;
	    }
	  default:
	    {
	      continue;
	    }
	  }
      }
    }
  }
#ifdef DEBUG
  fprintf(stdout,"Found %i walls on E side for pt[%c](%li, %li)\n",num_walls,point->type,point->x,point->y);
#endif
  if((num_walls%2) == 0){
      return 0;
  }
    
  // Y axis
  num_walls = 0;
  previous  = POINT;
  for(ssize_t y = 0 ; y < max_y ; y++){
    if( y == point->y){
#ifdef DEBUG
      fprintf(stdout,"Found %i walls on N side for pt[%c](%li, %li)\n",num_walls,point->type,point->x,point->y);
#endif
      if((num_walls%2) == 0){
	return 0;
      } else {
	num_walls = 0;
      }
    } else {
      if ((*(map + max_y*(point->x) + y)).visited == 1){ //(map[point->x][y].visited == 1){
	switch((*(map + max_y*(point->x) + y)).type)
	  {
	  case PIPE_H:
	    {
	      num_walls++;
	      continue;
	    }
	  case CORNER_SE: //F
	  case CORNER_NE: //L
	    {
	      if(previous == POINT){
		previous = (*(map + max_y*(point->x) + y)).type;
	      } else if ((previous == CORNER_SW) || (previous == CORNER_NW)){
		num_walls++;
		previous = POINT;
	      } else if ((previous == CORNER_SE) || (previous == CORNER_NE)){	    
		previous = POINT;
	      }
	      continue;
	    }
	  case CORNER_SW: //7
	  case CORNER_NW: //J
	    {
	      if(previous == POINT){
		previous = (*(map + max_y*(point->x) + y)).type;
	      } else if ((previous == CORNER_SE) || (previous == CORNER_NE)){
		num_walls++;
		previous = POINT;
	      } else if ((previous == CORNER_NW) || (previous == CORNER_SW)){	    
		previous = POINT;
	      }
	      continue;
	    }
	  default:
	    {
	      continue;
	    }
	  }    
      }
    }
  }
#ifdef DEBUG
  fprintf(stdout,"Found %i walls on S side for pt[%c](%li, %li)\n",num_walls,point->type,point->x,point->y);
#endif
  if((num_walls%2) == 0){
    return 0;
  }
  
  return 1;
}

int main(int argc, char *argv[])
{
  if (argc < 3){
    fprintf(stdout,"usage: ./day10 <input_file> <output 0: none>\n");
    exit(EXIT_FAILURE);
  } else {    
    char *buffer = NULL;
    size_t size = 0;
    ssize_t ret = 0;
    
    FILE *file = fopen(argv[1],"r");
    assert(file);

    int output = (atoi(argv[2]) > 0) ? 1 : 0;

    int input = 0;
    if (strcmp(argv[1],"input_test.txt") == 0)
      input = 1;
    else if (strcmp(argv[1],"input_test2.txt") == 0)
      input = 2;
    else if (strcmp(argv[1],"input_test3.txt") == 0)
      input = 3;    
    
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
      for(ssize_t x = 0 ; x <= x_origin ; x++){ 
	map[CONVERT_COORD_X(x)][y].type = temp[x];
	map[CONVERT_COORD_X(x)][y].x = CONVERT_COORD_X(x);
	map[CONVERT_COORD_X(x)][y].y = y;
	map[CONVERT_COORD_X(x)][y].visited   = -1;
	map[CONVERT_COORD_X(x)][y].is_ok     = -1;
	
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
    print_map((point_t*)map,NULL,x_dim,y_dim);
#endif

    point_t *current = &(map[x_start][y_start]);
    if (0 == input){
      current->type = CORNER_NE; // Given the input map, 'S' is actually an 'L' // real input
    }
    else if ((1 == input) || (3 == input)){
      current->type = CORNER_SE; // Given the input map, 'S' is actually an 'F' // input test
    }
    else if (2 == input){
      current->type = CORNER_SW; // Given the input map, 'S' is actually an '7' // input test 2
    }
    current->visited = 1;  

    //Given the input, the previous point is @: 
#ifdef REVERSE
    point_t *previous = &(map[x_start-1][y_start]);
#else
    point_t *previous = NULL;
    if (0 == input){
      previous = &(map[x_start][y_start+1]); //real input
    }
    else if (1 == input){
      previous = &(map[x_start][y_start-1]); //input test 1
    }
    else if (2 == input){
      previous = &(map[x_start+1][y_start]); //input test 2
    }
    else if (3 == input){
      previous = &(map[x_start][y_start-1]); //input test 2
    }
#endif    
    previous->visited = 1; // this forces a direction in the loop.

    point_t *saved_current = NULL;    
    long int num_steps = 0;
#ifdef DEBUG
    print_map((point_t*)map,NULL,x_dim,y_dim);
#endif
    //do/discover the loop
    do {
      saved_current = current;
      current = find_next_point((point_t *)map, current, previous, x_dim, y_dim);
      current->visited = 1;
      //#ifdef DEBUG
      if(output) {
	print_map((point_t *)map,current,x_dim,y_dim);
	//sleep(output);
	sleep(1);
      }
      //#endif
      previous = saved_current;
      num_steps++;
    }while(!((current->x == x_start)&&(current->y == y_start)));

    num_steps = (num_steps/2) + (num_steps%2); //in case there is an odd number of steps ...

    print_map((point_t *)map,NULL,x_dim,y_dim);
    fprintf(stdout,"================= num_steps : %li\n",num_steps);
    
    //Find right tiles
    int num_tiles = 0;
    for(ssize_t y = 0 ; y < y_origin ; y++){
      for(ssize_t x = 0 ; x < x_origin ; x++){	
	if((map[x][y].visited == -1)){
	  int res =  is_inside((point_t *)map, &map[x][y], x_dim, y_dim);
	  map[x][y].is_ok = res;
	  num_tiles += res;
#ifdef DEBUG
	  if(res) fprintf(stdout, "==========================================================res %i\n",res);
#endif
	}
      }
    }

    print_map((point_t*)map,NULL,x_dim,y_dim);
    fprintf(stdout,"================= num tiles : %i\n",num_tiles);
    
    free(buffer);
    fclose(file);    
  }
  
  exit(EXIT_SUCCESS);
}

  
