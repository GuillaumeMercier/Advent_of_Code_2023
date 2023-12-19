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

#define COLOR_STR (6)
typedef struct {
  int x;
  int y;
  int is_inside;
} point_t; 

typedef enum {
  R_off =  1,
  L_off = -1,
  U_off = -1,
  D_off =  1,
  O_off =  0,
} offset_t;

typedef enum {
  R = 'R',
  L = 'L',
  U = 'U',
  D = 'D',
  O = '.'
} tag_t;

typedef struct {
  tag_t    tag;
  offset_t off;  
} direction_t;

const direction_t East  = {.tag = R, .off = R_off};
const direction_t West  = {.tag = L, .off = L_off};
const direction_t North = {.tag = U, .off = U_off};
const direction_t South = {.tag = D, .off = D_off};
const direction_t None  = {.tag = O, .off = O_off};

typedef struct cell_s{
  point_t     coords;
  direction_t way;
  int         steps;
  char        color[COLOR_STR+1];
  struct cell_s *prev;
  struct cell_s *next;
} cell_t;

static int X_max = 0;
static int X_min = 0;
static int Y_max = 0;
static int Y_min = 0;

void enqueue(cell_t **head, cell_t newcell)
{
  if (*head == NULL){
    cell_t *tmp = calloc(1,sizeof(cell_t));
    *tmp = newcell;
    *head = tmp;
    tmp->prev = tmp->next  = *head;
  } else {
    assert(*head);
    cell_t *curr = *head;
    cell_t *prev = NULL;;
    int done = 0;
    while(!done){
      prev = curr;	    
      curr = curr->next;
      if(curr == *head)
	done = 1;      
    }
    cell_t *tmp = calloc(1,sizeof(cell_t));
    *tmp = newcell;
    tmp->next  = *head;
    tmp->prev  = prev;   	
    prev->next = tmp;
    (*head)->prev = tmp;
  }
}

void update_coords(cell_t *head, int x_min, int y_min)
{
  assert(head);
  cell_t *curr= head;
  int done = 0;
  while(!done){
    curr->coords.x -= x_min;
    curr->coords.y -= y_min;
    curr = curr->next;
    if(curr == head)
      done = 1;
  }
}  

cell_t *find_angle(cell_t *head, point_t pt)
{
  assert(head);
  cell_t *curr= head;
  int done = 0;
  while(!done){
    if((curr->coords.x == pt.x) &&
       (curr->coords.y == pt.y))
      return curr;
    curr = curr->next;
    if(curr == head)
      done = 1;
  }

  return NULL;
}

direction_t find_angle_vertical_direction(cell_t *head, cell_t *angle)
{
  assert(head);
  assert(angle);

  if ((angle->way.tag == U) || (angle->way.tag == D)){
    return angle->way;
  } else {
    if (angle->prev->way.tag == U) {
      return South;
    } else if (angle->prev->way.tag == D) {
      return North;
    }
  }
  return None;
}

void print_q(cell_t *head)
{
  if(head == NULL){
    fprintf(stdout," Q is empty !!!!!!!! \n");
  } else {
    cell_t *curr = head;
    int index = 0;
    int done = 0;
    while(!done){
      fprintf(stdout,"= Cell[%3i]: [x= %3i | y= %3i][Dir = %c][steps= %2i ][color=#%s] [me @%p | prev @%p | next @%p] \n",
	      index++,curr->coords.x,curr->coords.y,curr->way.tag,curr->steps,curr->color, curr, curr->prev, curr->next);  
      curr = curr->next;
      if(curr == head)
	done = 1;
    }
    fprintf(stdout,"\n");
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

void print_map(char*data, int x_max, int y_max)
{
  
  print_bar('=',15,stdout,1);
  for(int y = 0 ; y < y_max ; y++){
    fprintf(stdout,"[%3i]: ",y);
    for(int x = 0 ; x < x_max ; x++){
      fprintf(stdout,"%c",*(data + x*Y_max + y));
    }
    fprintf(stdout,"\n");
  }
  print_bar('=',15,stdout,1);
}

  

int main(int argc, char *argv[])
{
  if (argc < 2){
    fprintf(stdout,"usage: ./day17_part1 <input_file>\n");
    exit(EXIT_FAILURE);
  } else {    
    char *buffer = NULL;
    size_t size = 0;
    ssize_t ret = 0;
    
    FILE *file = fopen(argv[1],"r");
    assert(file);

    cell_t *head  = NULL;
    
    int x_max = 0;
    int x_min = 0;
    int y_max = 0;
    int y_min = 0;
    int index = 0;

    //create the list of angles and determine grid size
    while ((ret = getline(&buffer,&size,file)) != -1) {      
      buffer[strlen(buffer)-1] = '\0'; //remove EOL character
      
      cell_t newcell;
      memset(&newcell,0,sizeof(cell_t));
      
      char direction = *buffer;
      if (direction == 'R')
	newcell.way = East;
      else if (direction == 'L')
	newcell.way = West;
      else if (direction == 'U')
	newcell.way = North;
      else if (direction == 'D')
	newcell.way = South;

      newcell.steps = atoi(strpbrk(buffer,"0123456789"));
      memcpy(newcell.color,strpbrk(buffer,"#")+1,COLOR_STR);
      newcell.next = NULL;      

      if(head){
	
	if(head->prev->way.tag == R){
	  newcell.coords.x = head->prev->coords.x + head->prev->steps;
	  newcell.coords.y = head->prev->coords.y;
	  if(newcell.coords.x > x_max) x_max = newcell.coords.x;
	}
	else if (head->prev->way.tag == L){
	  newcell.coords.x = head->prev->coords.x - head->prev->steps;
	  newcell.coords.y = head->prev->coords.y;
	  if(newcell.coords.x < x_min) x_min = newcell.coords.x;
	}
	else if (head->prev->way.tag == U){
	  newcell.coords.x = head->prev->coords.x;
	  newcell.coords.y = head->prev->coords.y - head->prev->steps;
	  if(newcell.coords.y < y_min) y_min = newcell.coords.y;
	}
	else if (head->prev->way.tag == D){
	  newcell.coords.x = head->prev->coords.x;
	  newcell.coords.y = head->prev->coords.y + head->prev->steps;
	  if(newcell.coords.y > y_max) y_max = newcell.coords.y;
	}
	else {
	  abort();
	}
      } else {
	newcell.coords.x = 0;
	newcell.coords.y = 0;
      }
      
      enqueue(&head,newcell);
      assert(head->prev->next == head);
      index++;
    }   

    
    int size_x = (x_max-x_min)+1;
    int size_y = (y_max-y_min)+1;

#ifdef DEBUG
    fprintf(stdout,"=========== Grid is %2i x %2i\n",size_x, size_y);
    print_q(head);
#endif
    
    //shift all coords in the list by -x_min and -y_min
    update_coords(head,x_min,y_min);
    x_max -= x_min;
    y_max -= y_min;
    X_max = size_x;
    Y_max = size_y;
    X_min = Y_min = 0;

#ifdef DEBUG    
    print_q(head);
#endif
    
    // Fill map
    char map[size_x][size_y];
    memset(map,'.',sizeof(char)*size_x*size_y);

    cell_t *curr = head;
    do{
      map[curr->coords.x][curr->coords.y] = '#';
      if (curr->way.tag == R){	
	for(int i = curr->coords.x + 1; i < curr->coords.x + curr->steps ; i++){
	  map[i][curr->coords.y] = 'X';	  
	}
      } else if (curr->way.tag == L){	
	for(int i = curr->coords.x - 1 ; i > curr->coords.x - curr->steps ; i--){
	  map[i][curr->coords.y] = 'X';	  
	}
      } else if (curr->way.tag == D){
	for(int i = curr->coords.y + 1; i < curr->coords.y + curr->steps ; i++){
	  map[curr->coords.x][i] = 'X';	  
	}
      } else if (curr->way.tag == U){
	for(int i = curr->coords.y - 1; i > curr->coords.y - curr->steps ; i--){
	  map[curr->coords.x][i] = 'X';	  
	}
      } else {
	abort();
      }      
      curr = curr->next;
    }while(curr != head);

#ifdef DEBUG
    print_map((char*)map, size_x, size_y);
#endif
    
    //determine chars within bounds
    // on W side
#pragma omp parallel for
    for(int y = 0 ; y < size_y ; y++){
      int angles  = 0;
      int borders = 0;
      int direction = 0;
      cell_t *angle = NULL;		
      for(int x = 0 ; x < size_x ; x++){
	if( map[x][y] == 'X'){
	  if (angles == 0){
	    borders++;
	  }
	} else if ( map[x][y] == '#'){
	  point_t pt = {.x = x, .y = y};	    	  
	  angles++;
	  angle = find_angle(head,pt);
	  direction_t dir = find_angle_vertical_direction(head,angle);
	  direction += dir.off;

	  if ( 2 == angles ){
	    if(direction == 0){
	      borders++;
	    } else {
	      direction = 0;
	    }
	    angles = 0;
	    angle = NULL;
	    dir = None;
	  }
	} else if (map[x][y] == '.'){
	  if(borders%2){
	    map[x][y] = 'O';
	  }
	}
      }
    }

#pragma omp parallel for    
    for(int y = 0 ; y < size_y ; y++){
      for(int x = 0 ; x < size_x ; x++){
	if ((map[x][y] == 'X') || (map[x][y] == 'O'))
	  map[x][y] = '#';
      }
    }
#ifdef DEBUG    
   print_map((char*)map, size_x, size_y);
#endif
    //determine number of blocks
    int num_blocks = 0;
    for(int y = 0 ; y < size_y ; y++){
      for(int x = 0 ; x < size_x ; x++){
	if (map[x][y] == '#')
	  num_blocks++;
      }
    }

    fprintf(stdout,"=============== Num blocks = %3i\n", num_blocks);
    
    free(buffer);
    fclose(file);    
  }
  
  exit(EXIT_SUCCESS);
}

  
