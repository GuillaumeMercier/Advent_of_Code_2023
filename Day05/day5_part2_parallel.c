#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <pthread.h>

#define NUM_FIELDS  (3)
#define NUM_TABLES  (7)
#define MAX_ENTRIES (100)

#ifdef GLOBAL_MIN
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

typedef struct {
  long int dest;
  long int source;
  long int offset;  
} entry_t;

typedef struct {
  long int base;
  long int max;
  long int delta;  
} entry_transpose_t;

typedef struct {
  char   *name;
  int     size;
  entry_t data[MAX_ENTRIES];
  entry_transpose_t tdata[MAX_ENTRIES];
} table_t;

typedef struct {
  int tid;
  long int  seed;
  long int  max;
  long int *min_loc;
  table_t  *tables;
} thread_args_t;

void display_table(table_t *table)
{
  fprintf(stdout,"||| TABLE ||||||| %s\n",table->name);

  fprintf(stdout,"============== Original\n");
  for(int index = 0 ; index < table->size ; index++){
    fprintf(stdout,"[%10li][%10li][%10li]\n",
	    table->data[index].dest,
	    table->data[index].source,
	    table->data[index].offset);
  }
  fprintf(stdout,"==============================================\n");
  fprintf(stdout,"============== Transpose\n");
  for(int index = 0 ; index < table->size ; index++){
    fprintf(stdout,"[%10li][%10li][%10li]\n",
	    table->tdata[index].base,
	    table->tdata[index].max,
	    table->tdata[index].delta);
  }
  fprintf(stdout,"|||||||||||||||||||||||||||||||||||||||||||||||\n");
}

int compute_seed(table_t tables[], long int seed_value, long int *min_location)
{
  long int chain[NUM_TABLES+1];
  memset(chain,0,(NUM_TABLES+1)*sizeof(long int));
  chain[0] = seed_value;	
#ifdef DEBUG
  fprintf(stdout,"=============> Seed: %li\n",seed_value);
#endif
  //loop over tables
  for(int table_index = 0 ; table_index < NUM_TABLES ; table_index++){
    
#ifdef DEBUG
    fprintf(stdout,"=============> checking table: %s\n",tables[table_index].name);
#endif
    //Loop over entries in table
    for(int entry_index = 0 ; entry_index < tables[table_index].size ; entry_index++){
      if((tables[table_index].data[entry_index].source <= chain[table_index])  &&
	 (chain[table_index] < (tables[table_index].data[entry_index].source +
				tables[table_index].data[entry_index].offset))){
	chain[table_index+1] = chain[table_index] + (tables[table_index].data[entry_index].dest
						     - tables[table_index].data[entry_index].source);
	break;
      }
      chain[table_index+1] = chain[table_index];      	
    }//entries
  }//tables

#ifdef GLOBAL_MIN
  pthread_mutex_lock(&mutex);
#endif
  if ((*min_location == -1) || (chain[NUM_TABLES] < *min_location)){
#ifdef DEBUG
    fprintf(stdout,"New min found (old : %li) (new %li)\n",*min_location,chain[NUM_TABLES]);
#endif
    *min_location = chain[NUM_TABLES];
  }
#ifdef GLOBAL_MIN
  pthread_mutex_unlock(&mutex);
#endif
  
  return 0;
}

int compute_seed_transpose(table_t tables[], long int seed_value, long int *min_location)
{
  long int chain[NUM_TABLES+1];
  memset(chain,0,(NUM_TABLES+1)*sizeof(long int));
  chain[0] = seed_value;	
#ifdef DEBUG
  fprintf(stdout,"=============> Seed: %li\n",seed_value);
#endif
  //loop over tables
  for(int table_index = 0 ; table_index < NUM_TABLES ; table_index++){
    
#ifdef DEBUG
    fprintf(stdout,"=============> checking table: %s\n",tables[table_index].name);
#endif
    //Loop over entries in table
    for(int entry_index = 0 ; entry_index < tables[table_index].size ; entry_index++){
      if((tables[table_index].tdata[entry_index].base <= chain[table_index])  &&
	 (chain[table_index] < tables[table_index].tdata[entry_index].max)) {
	chain[table_index+1] = chain[table_index] + tables[table_index].tdata[entry_index].delta;	
	break;
      }      
      chain[table_index+1] = chain[table_index];      	
    }//entries
  }//tables
#ifdef GLOBAL_MIN
  pthread_mutex_lock(&mutex);
#endif
  if ((*min_location == -1) || (chain[NUM_TABLES] < *min_location)){
#ifdef DEBUG
    fprintf(stdout,"New min found (old : %li) (new %li)\n",*min_location,chain[NUM_TABLES]);
#endif
    *min_location = chain[NUM_TABLES];
  }
#ifdef GLOBAL_MIN
  pthread_mutex_unlock(&mutex);
#endif
  return 0;
}

void *thread_func(void *arg)
{
  thread_args_t *args = (thread_args_t *)arg;
  long int initial_seed = args->seed;
  long int max_seed = args->max;
  
#ifdef DEBUG 
  fprintf(stdout,"=============> T[%i] Loop over interval [%12li -> %12li[\n", args->tid,initial_seed,max_seed);
#endif
  //loop over seeds in interval      
  for(long int seed_value = initial_seed ; seed_value < max_seed ; seed_value++){	
    //(void)compute_seed(args->tables,seed_value,args->min_loc);
    (void)compute_seed_transpose(args->tables,seed_value,args->min_loc);
  }

  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  if (argc < 2){
    fprintf(stdout,"usage: ./day5_part2 <input_file>\n");
    exit(EXIT_FAILURE);
  } else {
    char *buffer = NULL;
    char *temp   = NULL;
    char *str    = NULL;
    char *copy   = NULL; 
    long int *seeds = NULL;
    size_t size = 0;
    ssize_t ret = 0;
    FILE *file = fopen(argv[1],"r");
    assert(file);

    // get number of seeds and their values
    (void)getline(&buffer,&size,file);
    buffer[strlen(buffer)-1] = '\0'; //remove EOL character      
    temp = calloc(strlen(buffer)+1,sizeof(char));
    memcpy(temp,buffer,strlen(buffer));
    copy = temp;
    str = strsep(&temp,":");
    assert( strcmp(str,"seeds") == 0);

    temp = strpbrk(temp,"0123456789"); //go to first number
    //compute seeds number 
    int num_values = 0;
    while((str = strsep(&temp," ")) != NULL) num_values++;
    fprintf(stdout,"Number of values: %i\n",num_values);
    seeds = malloc(num_values*sizeof(long int));     

    //get seeds values
    temp = copy;
    memcpy(temp,buffer,strlen(buffer));
    temp = strpbrk(temp,"0123456789"); //go to first number    
    for(int index = 0 ; index < num_values ; index++){
      seeds[index] = atol(strsep(&temp," "));
    }

#ifdef DEBUG
    for(int index = 0 ; index < num_values ; index++){
      fprintf(stdout,"values[%2i] = %10li\n",index, seeds[index]);
    }
#endif
    
    long int num_seeds = 0;
    for(int index = 1 ; index < num_values ; index += 2){
      num_seeds += seeds[index];
    }    
    fprintf(stdout,"Number of seeds: %li\n", num_seeds);

    free(copy);

    //get the table
    table_t tables[NUM_TABLES];
    for(int index = 0; index < NUM_TABLES; index++){     
      memset(tables+index,0,sizeof(table_t));
    }
    int index = -1;
    while ((ret = getline(&buffer,&size,file)) != -1) {
      buffer[strlen(buffer)-1] = '\0'; //remove EOL character      
      if(isalpha(buffer[0])){
	buffer[strlen(buffer)-1] = '\0'; //remove ":" character
	index++;
	tables[index].size = 0;
	tables[index].name = strsep(&buffer," ");//calloc(strlen(buffer)+1,sizeof(char));
	//memcpy(tables[index].name,buffer,strlen(buffer)+1);	
      } else if(isdigit(buffer[0])){
	//original
	tables[index].data[tables[index].size].dest   = atol(strsep(&buffer," "));
	tables[index].data[tables[index].size].source = atol(strsep(&buffer," "));
	tables[index].data[tables[index].size].offset = atol(strsep(&buffer," "));
	//transpose
	long int source = tables[index].data[tables[index].size].source;
	tables[index].tdata[tables[index].size].base  = source;
	tables[index].tdata[tables[index].size].max   = source + tables[index].data[tables[index].size].offset;
	tables[index].tdata[tables[index].size].delta = tables[index].data[tables[index].size].dest - source;
	tables[index].size++;
      }     
    }
#ifdef DEBUG
    for(int index = 0; index < NUM_TABLES; index++){     
      display_table(tables+index);
    }
#endif
    
    //Compute all locations based on seeds.
    long int min_location = -1;

    //spawn a thread per interval
    pthread_t tids[num_values/2];
    thread_args_t arg[num_values/2];
    
    for(int index = 0 ; index < num_values ; index += 2){
      arg[index/2].tid  = index/2;
      arg[index/2].seed = seeds[index];
      arg[index/2].max  = seeds[index] + seeds[index+1];
#ifdef GLOBAL_MIN 
      arg[index/2].min_loc = &min_location;
#else
      arg[index/2].min_loc = malloc(sizeof(long int));
      *(arg[index/2].min_loc) = -1;
#endif
      arg[index/2].tables = tables;
      pthread_create(tids+index/2,NULL,thread_func,(void *)(arg+index/2));
    }

    for(int index = 0 ; index < num_values/2 ; index++){
      pthread_join(tids[index],NULL);
    }

#ifndef GLOBAL_MIN
    min_location = *(arg[0].min_loc);
    free(arg[0].min_loc);
    for(int index = 1 ; index < num_values/2 ; index++){
      if( *(arg[index].min_loc) < min_location){
	min_location = *(arg[index].min_loc);
      }
      free(arg[index].min_loc);
    }
#endif
    
    fprintf(stdout,"Min location = %li\n",min_location);

    free(seeds);
    free(buffer);
    fclose(file);
  }
  
  exit(EXIT_SUCCESS);
}

  
