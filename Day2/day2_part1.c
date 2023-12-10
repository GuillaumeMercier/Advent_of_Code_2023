#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>

#define NUM_COLORS (3)

typedef struct {
  char *name;
  int  value;
} color_t;

color_t color_set[NUM_COLORS] = {{.name = "red",   .value = 12},
                                 {.name = "green", .value = 13},
                                 {.name = "blue",  .value = 14}};

int main(int argc, char *argv[])
{
  if (argc < 2){
    fprintf(stdout,"usage: ./day2_part1 <input_file>\n");
    exit(EXIT_FAILURE);
  } else {
    char *buffer = NULL;
    size_t size = 0;
    ssize_t ret = 0;
    int sum = 0;
    FILE *file = fopen(argv[1],"r");
    assert(file);
        
    while ((ret = getline(&buffer,&size,file)) != -1) {

      //make a copy of the buffer
      char *temp = calloc(strlen(buffer),sizeof(char));
      memcpy(temp,buffer,strlen(buffer)-1); //-1 to remove EOL character

      //get a reference on the original addr, will be used to free the temporary buffer
      //as strsep will mess up with the temp pointer
      char *copy = temp;

#ifdef DEBUG
      fprintf(stdout,"Line : %s\n",temp);
#endif
      
      //First, get game id
      char *game_str = strsep(&temp,":");
      int id = atoi(strpbrk(game_str,"0123456789")); //ok to use atoi since
                                                     //strpbrk will never return NULL in this case      
#ifdef DEBUG
      fprintf(stdout,"Game id:%i\n",id);
#endif
      // this game is valid until it is not
      int is_game_ok = 1;
      
      //Then, get each round
#ifdef DEBUG
      int num_round = 0;
#endif
      while(temp){
	char *round = strsep(&temp,";");
#ifdef DEBUG
	fprintf(stdout,"Round[%i]: %s : ",++num_round, round);
#endif
	//Then, in each round, get the number of colors
	while(round){
	  char *color = strsep(&round,",");
	  
	  for(int index = 0 ; index < NUM_COLORS ; index++){
	    // found color name in current round
	    if( strstr(color,color_set[index].name) != NULL){
	      int num_cubes = atoi(strpbrk(color,"0123456789")); //this works b/c there is a space between the number
	                                                         //and the color name 
	      // check validity
	      if (num_cubes > color_set[index].value){
		is_game_ok = 0;
		goto end; // no need to check other colors and rounds.
	      }
	    }
	  }
	}

#ifdef DEBUG	  	
	fprintf(stdout,"is OK\n");
#endif	
      }

    end:
      if (is_game_ok){	
	sum += id;
#ifdef DEBUG
	fprintf(stdout,">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Game %i is OK\n",id);
#endif
      } else {
#ifdef DEBUG
	fprintf(stdout,"is NOT OK\n");
	fprintf(stdout,">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Game %i is NOT OK\n",id);
#endif	
      }
      
      free(copy);
    }

    fprintf(stdout,"Result = %i\n",sum);    
    free(buffer);
    fclose(file);    
  }
  
  exit(EXIT_SUCCESS);
}
