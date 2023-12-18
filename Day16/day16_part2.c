#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <unistd.h>
#include <math.h>

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


typedef enum tag {
  NORTH = '^',
  SOUTH = 'v',
  EAST  = '>',
  WEST  = '<',
} tag_t;

typedef enum doff {
  NORTH_OFF = -1,
  SOUTH_OFF = +1,
  EAST_OFF  = +1,
  WEST_OFF  = -1,
} offset_t;

typedef struct {
  tag_t    tag;
  offset_t offset;
} direction_t;

typedef struct {
  int x;
  int y;
  direction_t way;
} beam_t;

typedef struct {
  char tag;
  int energized;
  int from_north;
  int from_south;
  int from_east;
  int from_west; 
} tile_t ;
  
static direction_t North = {.tag = NORTH, .offset = NORTH_OFF};
static direction_t South = {.tag = SOUTH, .offset = SOUTH_OFF};
static direction_t East  = {.tag = EAST,  .offset = EAST_OFF};
static direction_t West  = {.tag = WEST,  .offset = WEST_OFF};

static int x_max  = 0;
static int y_max  = 0;

#define TILE(map, x, y) ((map) + (x)*y_max + (y))

void swap(int *a, int *b)
{
  int  temp = *a;
  *a = *b;
  *b = temp;
}

void my_sort(int array[], int size)
{
  if (size > 1) {
    for (int i = 0; i < size -1 ; i++){
      if (array[i] > array[i+1])
	swap(array+i,array+i+1);
    }
    my_sort(array,size-1);
  }
}

int is_mirrors(char tile)
{
  return ((tile == '-') || (tile == '|') || (tile == '\\') || (tile == '/'));
}

int is_light(char tile)
{
  return ((tile == NORTH) || (tile == SOUTH) || (tile == EAST) || (tile == WEST));
}

void change_pos(beam_t *beam, direction_t way, int offset)
{
  if ((way.tag == NORTH) || (way.tag == SOUTH)){
    beam->y += offset;
  } else if ((way.tag == EAST) || (way.tag == WEST)){
    beam->x += offset;
  }
}

void change_pos_next(beam_t *beam, direction_t way)
{
  if (way.tag == NORTH) {
    change_pos(beam,way,NORTH_OFF);
  } else if (way.tag == SOUTH) {
    change_pos(beam,way,SOUTH_OFF);
  } else if (way.tag == WEST) {
    change_pos(beam,way,WEST_OFF);
  } else if (way.tag == EAST) {
    change_pos(beam,way,EAST_OFF);
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

void print_map(tile_t *data, int x_pos, int y_pos)
{
  print_bar('=',15,stdout,1);
  for(int y = 0 ; y < y_max ; y++){
    fprintf(stdout,"[%3i]: ",y);
    for(int x = 0 ; x < x_max ; x++){
      char c = (*(data + x*y_max + y)).tag;
      int ener = (*(data + x*y_max + y)).energized;
      if ((x == x_pos) && (y == y_pos)){
      	PRINT_RED(c);
      } else { 
	if (is_light(c) || (is_mirrors(c) && ener)){
	  PRINT_GREEN(c);
	} else if (is_mirrors(c)){
	  PRINT_BLUE(c);
	} else if (c == '2') {
	  PRINT_WHITE(c);
	} else	
	  PRINT_YELLOW(c);
      }
    }
    fprintf(stdout,"\n");
  }
  print_bar('=',15,stdout,1);
}
  
  
int parcours(beam_t *beam, tile_t  *map)
{
  assert(beam);
  assert(map);

  tile_t *tile      = NULL; //next
  tile_t *curr_tile = TILE(map, (beam->x), (beam->y));

  (curr_tile->energized)++;

#ifdef DEBUG
  int  x = beam->x;
  int  y = beam->y;  
  if (beam->way.tag == NORTH)
    y--;
  if (beam->way.tag == SOUTH)
    y++;  
  if (beam->way.tag == EAST)
    x++;
  if (beam->way.tag == WEST)
    x--;  
  fprintf(stdout,"CURRENT TILE @ (%3i,%3i) = %c\n", beam->x, beam->y, curr_tile->tag);
#endif
  
  if (beam->way.tag == NORTH)
    tile = TILE(map, (beam->x), (beam->y-1));  
  else if (beam->way.tag == SOUTH)
    tile = TILE(map, (beam->x), (beam->y+1));
  if (beam->way.tag == WEST)
    tile = TILE(map, (beam->x-1), (beam->y));
  else if (beam->way.tag == EAST)
    tile = TILE(map, (beam->x+1), (beam->y));

#ifdef DEBUG
  fprintf(stdout,"NEXT    TILE @ (%3i,%3i) = %c\n", x, y, tile->tag);
#endif
  if ( curr_tile->tag == '.'){
    (*curr_tile).tag = beam->way.tag;
  }
  
#ifdef DEBUG
  print_map(map, beam->x, beam->y);  
  //sleep(1);
#endif

  switch(tile->tag)
    {
    case '*': //Black Hole
      {
	free(beam);
	return 0; 	//pthread_exit(0);
      }
    case '-':
      {
	tile->from_east = 1;
	tile->from_west = 1;

	if (tile->from_south || tile->from_north){
	  free(beam);
	  return 0;	    
	}
	
	if (beam->way.tag == EAST){ 
	  (beam->x)++;
	} else if (beam->way.tag == WEST){
	  (beam->x)--;
	} else if (beam->way.tag == NORTH){
	  tile->from_north = 1;
	  (beam->y)--;
	  beam->way = East;
	  	  
	  beam_t *new = malloc(sizeof(beam_t));
	  assert(new);
	  new->x   = beam->x;
	  new->y   = beam->y;  
	  new->way = West;

	  parcours(new, map); 	  //pthread_create
	  
	} else if (beam->way.tag == SOUTH){
	  tile->from_south = 1;
	  (beam->y)++;
	  beam->way = East;

	  beam_t *new = malloc(sizeof(beam_t));
	  assert(new);
	  new->x   = beam->x;
	  new->y   = beam->y; 
	  new->way = West;
	  
	  parcours(new, map); 	  //pthread_create	  
	}
	parcours(beam, map);	
	break;
      }
    case '|':
      {
	tile->from_north = 1;
	tile->from_south = 1;

	if (tile->from_east || tile->from_west){
	  free(beam);
	  return 0;	    
	}
	
	if(beam->way.tag == NORTH){
	  (beam->y)--;
	} else if (beam->way.tag == SOUTH){
	  (beam->y)++;	  
	} else if (beam->way.tag == EAST){
	  tile->from_east = 1;
	  (beam->x)++;
	  beam->way = North;

	  beam_t *new = malloc(sizeof(beam_t));
	  assert(new);
	  new->x   = beam->x;
	  new->y   = beam->y;  
	  new->way = South;

	  parcours(new, map); 	  //pthread_create
	  
	} else if (beam->way.tag == WEST){
	  tile->from_west = 1;
	  (beam->x)--;
	  beam->way = North;

	  beam_t *new = malloc(sizeof(beam_t));
	  assert(new);
	  new->x   = beam->x;
	  new->y   = beam->y; 
	  new->way = South;

	  parcours(new, map); 
	}
	parcours(beam, map);
	break;
      }
    case '/':
      {
	if(beam->way.tag == EAST){
	  if (tile->from_south && tile->from_east){
	    free(beam);
	    return 0;	    
	  } else {
	    tile->from_east  = 1;
	    tile->from_south = 1;
	  }	  
	  (beam->x)++;
	  beam->way = North;
	}  else if (beam->way.tag == WEST){
	  if (tile->from_west && tile->from_north){
	    free(beam);
	    return 0;	    
	  } else {
	    tile->from_west  = 1;
	    tile->from_north = 1;
	  }
	  (beam->x)--;
	  beam->way = South;
	} else if (beam->way.tag == SOUTH){
	  if (tile->from_east && tile->from_south){
	    free(beam);
	    return 0;	    
	  } else {
	    tile->from_east  = 1;
	    tile->from_south = 1;
	  }
	  (beam->y)++;
	  beam->way = West;
	} else if (beam->way.tag == NORTH){
	  if (tile->from_west && tile->from_north){
	    free(beam);
	    return 0;	    
	  } else {
	    tile->from_west  = 1;
	    tile->from_north = 1;
	  }
	  (beam->y)--;
	  beam->way = East;
	}       
	parcours(beam, map);
	break;
      }
    case '\\':
      {
	if(beam->way.tag == EAST){
	  if (tile->from_east && tile->from_north){
	    free(beam);
	    return 0;	    
	  } else {
	    tile->from_east  = 1;
	    tile->from_north = 1;
	  }
	  (beam->x)++;
	  beam->way = South;
	}  else if (beam->way.tag == WEST){
	  if (tile->from_west && tile->from_south){
	    free(beam);
	    return 0;	    
	  } else {
	    tile->from_west  = 1;
	    tile->from_south = 1;
	  }
	  (beam->x)--;
	  beam->way = North;
	} else if (beam->way.tag == SOUTH){
	  if (tile->from_west && tile->from_south){
	    free(beam);
	    return 0;	    
	  } else {
	    tile->from_west  = 1;
	    tile->from_south = 1;
	  }
	  (beam->y)++;
	  beam->way = East;
	} else if (beam->way.tag == NORTH){
	  if (tile->from_east && tile->from_north){
	    free(beam);
	    return 0;	    
	  } else {
	    tile->from_east  = 1;
	    tile->from_north = 1;
	  }
	  (beam->y)--;
	  beam->way = West;
	}
	parcours(beam, map);
	break;
      }
    case '.':
      {
	change_pos_next(beam, beam->way);
	parcours(beam, map);
	break;
      }
    default:
      {
	if (tile->tag == '2'){
	  free(beam);
	  return 0;
	}
	
	if (is_light( tile->tag )) {
	  if ((beam->way.tag == NORTH)||
	      (beam->way.tag == SOUTH)){	    
	    if ((tile->tag == NORTH)||
		(tile->tag == SOUTH)){
	      free(beam);
	      return 0;
	    } else if((tile->tag == EAST)||
		      (tile->tag == WEST)){	    
	      tile->tag  = '2';
	    }
	    
	  } else if ((beam->way.tag == EAST)||
		     (beam->way.tag == WEST)) {	 
	    if ((tile->tag == NORTH)||
		(tile->tag == SOUTH)){
	      tile->tag  = '2';	 
	    } else if((tile->tag == EAST)||
		      (tile->tag == WEST)){	    
	      free(beam);
	      return 0;
	    }
	  }	  
	}
	  
	change_pos_next(beam, beam->way);
	parcours(beam, map);
	break;
      }
    }
  
  return 0;
}

int main(int argc, char *argv[])
{
  if (argc < 2){
    fprintf(stdout,"usage: ./day16_part1 <input_file>\n");
    exit(EXIT_FAILURE);
  } else {    
    char *buffer = NULL;
    size_t size = 0;
    ssize_t ret = 0;
    
    FILE *file = fopen(argv[1],"r");
    assert(file);

    while ((ret = getline(&buffer,&size,file)) != -1) {      
      buffer[strlen(buffer)-1] = '\0'; //remove EOL character
      x_max = strlen(buffer);
      fprintf(stdout,"(Line %3i): %s\n",y_max,buffer);      
      y_max++;
    }
    rewind(file);
    fprintf(stdout,"========== Matrix is (%3i)x(%3i)\n",x_max,y_max);

    x_max += 2; // b/c BH boundary
    y_max += 2; // b/c BH boundary
    tile_t data[x_max][y_max]; 
    memset(data,0,sizeof(tile_t)*(x_max)*(y_max)); 
    
    for(int x = 0 ; x < x_max + 1 ; x++){
      data[x][0].tag = data[x][y_max-1].tag = '*';
    }        
    for(int y = 1 ; y < y_max - 1 ; y++){
      data[0][y].tag = data[x_max-1][y].tag = '*'; 
      ret = getline(&buffer,&size,file);
      if( ret != -1){
	for(int x = 1 ; x < strlen(buffer) ; x++){
	  (data[x][y]).tag = buffer[x-1];
	  (data[x][y]).energized = 0;
	}
      }
    }

#ifdef DEBUG
    print_map((tile_t *)data,0,0);
#endif
    
    //W side
    int energizedW[y_max-2];
    memset(energizedW,0,sizeof(int)*(y_max-2));

#pragma omp parallel for
    for(int tries = 0 ; tries < y_max - 2 ; tries++){

      tile_t data_copy[x_max][y_max];
      for(int y = 0 ; y < y_max  ; y++){
	for(int x = 0 ; x < x_max ; x++){
	  data_copy[x][y] = data[x][y];	  
	}
      }
      
      beam_t *origin = malloc(sizeof(beam_t));
      origin->x   = 0;
      origin->y   = tries+1;
      origin->way = East;
    
      parcours(origin, (tile_t *)data_copy);

#ifdef DEBUG
      print_map((tile_t *)data_copy,origin->x,origin->y);
#endif
      for(int y = 1 ; y < y_max - 1 ; y++){
	for(int x = 1 ; x < x_max - 1 ; x++){
	  if(data_copy[x][y].energized)
	    energizedW[tries]++;
	}
      }      
    }
    my_sort(energizedW,y_max-2);    
    fprintf(stdout,"========== EnergizedW = %i\n",energizedW[y_max-3]); 

    // N side
    int energizedN[x_max-2];
    memset(energizedN,0,sizeof(int)*(x_max-2));

#pragma omp parallel for
    for(int tries = 0 ; tries < x_max - 2 ; tries++){

      tile_t data_copy[x_max][y_max];
      for(int y = 0 ; y < y_max  ; y++){
	for(int x = 0 ; x < x_max ; x++){
	  data_copy[x][y] = data[x][y];	  
	}
      }
      
      beam_t *origin = malloc(sizeof(beam_t));
      origin->x   = tries+1;
      origin->y   = 0;
      origin->way = South;
    
      parcours(origin, (tile_t *)data_copy);

#ifdef DEBUG
      print_map((tile_t *)data_copy,origin->x,origin->y);
#endif
      for(int y = 1 ; y < y_max - 1 ; y++){
	for(int x = 1 ; x < x_max - 1 ; x++){
	  if(data_copy[x][y].energized)
	    energizedN[tries]++;
	}
      }      
    }
    my_sort(energizedN,x_max-2);    
    fprintf(stdout,"========== EnergizedN = %i\n",energizedN[x_max-3]);
    
    //E Side
    int energizedE[y_max-2];
    memset(energizedE,0,sizeof(int)*(y_max-2));

#pragma omp parallel for
    for(int tries = 0 ; tries < y_max - 2 ; tries++){

      tile_t data_copy[x_max][y_max];
      for(int y = 0 ; y < y_max  ; y++){
	for(int x = 0 ; x < x_max ; x++){
	  data_copy[x][y] = data[x][y];	  
	}
      }
      
      beam_t *origin = malloc(sizeof(beam_t));
      origin->x   = x_max-1;
      origin->y   = tries+1;
      origin->way = West;
    
      parcours(origin, (tile_t *)data_copy);

#ifdef DEBUG
      print_map((tile_t *)data_copy,origin->x,origin->y);
#endif
      for(int y = 1 ; y < y_max - 1 ; y++){
	for(int x = 1 ; x < x_max - 1 ; x++){
	  if(data_copy[x][y].energized)
	    energizedE[tries]++;
	}
      }      
    }
    my_sort(energizedE,y_max-2);    
    fprintf(stdout,"========== EnergizedE = %i\n",energizedE[y_max-3]); 

    //S Side
    int energizedS[x_max-2];
    memset(energizedS,0,sizeof(int)*(x_max-2));

#pragma omp parallel for
    for(int tries = 0 ; tries < x_max - 2 ; tries++){

      tile_t data_copy[x_max][y_max];
      for(int y = 0 ; y < y_max  ; y++){
	for(int x = 0 ; x < x_max ; x++){
	  data_copy[x][y] = data[x][y];	  
	}
      }
      
      beam_t *origin = malloc(sizeof(beam_t));
      origin->x   = tries+1;
      origin->y   = y_max-1;;
      origin->way = North;
    
      parcours(origin, (tile_t *)data_copy);

#ifdef DEBUG
      print_map((tile_t *)data_copy,origin->x,origin->y);
#endif
      
      for(int y = 1 ; y < y_max - 1 ; y++){
	for(int x = 1 ; x < x_max - 1 ; x++){
	  if(data_copy[x][y].energized)
	    energizedS[tries]++;
	}
      }      
    }
    my_sort(energizedS,x_max-2);    
    fprintf(stdout,"========== EnergizedS = %i\n",energizedS[x_max-3]);
    
    free(buffer);
    fclose(file);    
  }
  
  exit(EXIT_SUCCESS);
}

  
