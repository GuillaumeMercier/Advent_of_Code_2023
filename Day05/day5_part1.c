#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>

#define NUM_FIELDS  (3)
#define NUM_TABLES  (7)
#define MAX_ENTRIES (100)

typedef struct {
  long int dest;
  long int source;
  long int offset;  
} entry_t;

typedef struct {
  char   *name;
  int     size;
  entry_t data[MAX_ENTRIES];
} table_t;

void display_table(table_t *table)
{
  fprintf(stdout,"===========TABLE === %s\n",table->name);
  for(int index = 0 ; index < table->size ; index++){
    fprintf(stdout,"[%10li][%10li][%10li]\n",
	    table->data[index].dest,
	    table->data[index].source,
	    table->data[index].offset);
  }
  fprintf(stdout,"======================================================\n");
}

int main(int argc, char *argv[])
{
  if (argc < 2){
    fprintf(stdout,"usage: ./day5_part1 <input_file>\n");
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
    getline(&buffer,&size,file);
    buffer[strlen(buffer)-1] = '\0'; //remove EOL character      
    temp = calloc(strlen(buffer)+1,sizeof(char));
    memcpy(temp,buffer,strlen(buffer));
    copy = temp;
    str = strsep(&temp,":");
    assert( strcmp(str,"seeds") == 0);

    temp = strpbrk(temp,"0123456789"); //go to first number
    //compute seeds number 
    int num_seeds = 0;
    while((str = strsep(&temp," ")) != NULL) num_seeds++;
    fprintf(stdout,"Number of seeds: %i\n",num_seeds);
    seeds = malloc(num_seeds*sizeof(long int));     

    //get seeds values
    temp = copy;
    memcpy(temp,buffer,strlen(buffer));
    temp = strpbrk(temp,"0123456789"); //go to first number    
    for(int index = 0 ; index < num_seeds ; index++){
      seeds[index] = atol(strsep(&temp," "));
    }

#ifdef DEBUG
    for(int index = 0 ; index < num_seeds ; index++){
      fprintf(stdout,"Seed[%2i] = %10li\n",index, seeds[index]);
    }
#endif
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
	tables[index].name = calloc(strlen(buffer)+1,sizeof(char));
	memcpy(tables[index].name,buffer,strlen(buffer)+1);	
      } else if(isdigit(buffer[0])){
	tables[index].data[tables[index].size].dest   = atol(strsep(&buffer," "));
	tables[index].data[tables[index].size].source = atol(strsep(&buffer," "));
	tables[index].data[tables[index].size].offset = atol(strsep(&buffer," "));
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
    for(int seed_index = 0 ; seed_index < num_seeds ; seed_index++){
      long int chain[NUM_TABLES+1];
      chain[0] = seeds[seed_index];
#ifdef DEBUG
      fprintf(stdout,"=============> Seed : %li\n",chain[0]);
#endif
      for(int table_index = 0 ; table_index < NUM_TABLES ; table_index++){
#ifdef DEBUG
	fprintf(stdout,"=============> checking table: %s\n",tables[table_index].name);
#endif
	for(int entry_index = 0 ; entry_index < tables[table_index].size ; entry_index++){
	  if((tables[table_index].data[entry_index].source <= chain[table_index])  &&
	     (chain[table_index] < (tables[table_index].data[entry_index].source +
				    tables[table_index].data[entry_index].offset))){
	    chain[table_index+1] = chain[table_index] + (tables[table_index].data[entry_index].dest
							 - tables[table_index].data[entry_index].source);
	    break;
	  }
	  chain[table_index+1] = chain[table_index];      	
	}
      }  
      if ((min_location == -1) || (chain[NUM_TABLES] < min_location)){
#ifdef DEBUG
	fprintf(stdout,"New min found (old : %li) (new %li)\n",min_location,chain[NUM_TABLES]);
#endif
	min_location = chain[NUM_TABLES];
      }      
    }
    
    fprintf(stdout,"Min location = %li\n",min_location);
    
    free(seeds);
    free(buffer);
    fclose(file);
  }
  
  exit(EXIT_SUCCESS);
}

  
