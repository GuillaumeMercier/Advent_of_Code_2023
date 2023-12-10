#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>


#define COLOR_END  "\x1b[0m" // To flush out prev settings
#define NUM_COLORS (3)

typedef struct {
  char *str;
  char *name;
} color_t;

color_t color_set[NUM_COLORS] = {{.str = "\x1b[31m", .name = "red"},
				 {.str = "\x1b[32m", .name = "green"},
				 {.str = "\x1b[34m", .name = "blue"}};
  
int main(int argc, char *argv[])
{
  if (argc < 2){
    fprintf(stdout,"usage: ./day2_part2 <input_file>\n");
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
      char *game_str __attribute__((unused)) = strsep(&temp,":"); //avoids compiler warning
#ifdef DEBUG
      int id = atoi(strpbrk(game_str,"0123456789")); //ok to use atoi since
                                                     //strpbrk will never return NULL in this case      
      fprintf(stdout,"Game id:%i\n",id);
#endif

      //Then, get each round
#ifdef DEBUG
      int num_round = 0;
#endif
      int possibles[NUM_COLORS] = {0, 0, 0};
      
      while(temp){
	char *round = strsep(&temp,";");
#ifdef DEBUG
	fprintf(stdout,"Round[%i]: %s\n",++num_round, round);
#endif
	//Then, in each round, get the number of colors
	while(round){
	  char *color = strsep(&round,",");
	  
	  for(int index = 0 ; index < NUM_COLORS ; index++){
	    // found color name in current round
	    if( strstr(color,color_set[index].name) != NULL){
	      int num_cubes = atoi(strpbrk(color,"0123456789")); //this works b/c there is a space between the number
	                                                         //and the color name 
	      // determine new possible for that color
	      if (num_cubes > possibles[index]){
		possibles[index] = num_cubes;
#ifdef DEBUG
		fprintf(stdout,"    Found new possible for color %s %s : %i %s\n",
			color_set[index].str,color_set[index].name,possibles[index],COLOR_END);
#endif
	      }
	    }
	  }
	}
      }

      //computer the set power for this game
      int power = 1;
      for(int index = 0 ; index < NUM_COLORS ; index++){
	power *= possibles[index];
      }

      //update result
      sum += power;      
      free(copy);

#ifdef DEBUG
      fprintf(stdout,"================================ Set power = %i\n",power);
#endif		   
    }

    fprintf(stdout,"Result = %i\n",sum);    
    free(buffer);
    fclose(file);    
  }
  
  exit(EXIT_SUCCESS);
}
