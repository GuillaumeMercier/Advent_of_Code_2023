#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>

#define WN_NUM (10)
#define PN_NUM (25)
#define MAX_CARDS (202)

int main(int argc, char *argv[])
{
  if (argc < 3){
    fprintf(stdout,"usage: ./day4_part2 <num_cards> <output file from part 1 OR stdin> \n");
    //exit(EXIT_FAILURE);
  } 

  FILE *file = argv[2] ? fopen(argv[2],"r") : stdin;
  assert(file);
  
  int  num_cards  = atoi(argv[1]) + 1; // card[0] will not be used
  int *ncards     = calloc(num_cards,sizeof(int));
  for(int index = 1; index <= num_cards ; index++){
    ncards[index] = 1;
  }
  
  int *matches = calloc(num_cards,sizeof(int));
  int index = 1;    
  while(!feof(file)){
    fscanf(file,"%i\n",&matches[index++]);      
  }
  
#ifdef DEBUG
  fprintf(stdout,"=========================\n");
  for(int index = 1 ; index < num_cards ; index++){
    fprintf(stdout,"Card[%3i]:  matches %3i | copies %3i\n", index, matches[index], ncards[index]);
  }
  fprintf(stdout,"=========================\n");
#endif
  
  for(int card_id = 1 ; card_id < num_cards ; card_id++){ // loop over card ids      
    for(int nadd = 1 ; nadd <= ncards[card_id] ; nadd++){ 
      for(int card_offset = 1 ; ((card_offset <= matches[card_id]) && ((card_offset + card_id) <= num_cards)) ; card_offset++){ 
	ncards[card_id + card_offset]++;		
      }	
    }
  }
  
#ifdef DEBUG
  fprintf(stdout,"=========================\n");
  for(int index = 1 ; index < num_cards ; index++){
    fprintf(stdout,"Card[%3i]:  matches %3i | copies %6i\n", index, matches[index], ncards[index]);
  }
  fprintf(stdout,"=========================\n");
#endif
  
  long long int ncards_total = 0;
  for(int index = 1; index < num_cards ; index++){
    ncards_total += ncards[index];
  }
  fprintf(stdout,"Total number of Cards = %lli\n",ncards_total);
  
  free(ncards);
  free(matches);
  fclose(file);
  
  exit(EXIT_SUCCESS);
}

  
