#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>

#define WN_NUM (10)
#define PN_NUM (25)

int main(int argc, char *argv[])
{
  if (argc < 3){
    fprintf(stdout,"usage: ./day4_part1 <input_file> <mode>\n");
    exit(EXIT_FAILURE);
  } else {
    char *buffer = NULL;
    size_t size = 0;
    ssize_t ret = 0;
    long long int sum = 0;        
    FILE *file = fopen(argv[1],"r");
    assert(file);

    int num_cards = 0;
    while ((ret = getline(&buffer,&size,file)) != -1) {
      num_cards++;
    }
    rewind(file);

#ifdef DEBUG
    fprintf(stdout,"Num cards = %i\n",num_cards);
#endif
    
    int *matches = calloc(num_cards,sizeof(int));
    
    while ((ret = getline(&buffer,&size,file)) != -1) {
      buffer[strlen(buffer)-1] = '\0'; //remove EOL character      
      //fprintf(stdout,"Line(%li): %s\n",strlen(buffer),buffer);
      
      char *temp = calloc(strlen(buffer),sizeof(char));
      memcpy(temp,buffer,strlen(buffer));
      char *copy = temp;
#ifdef DEBUG
      fprintf(stdout,"Line : %s\n",temp);
#endif
      char *str = strsep(&temp,":");
      int card_id = atoi(strpbrk(str,"0123456789"));
      char *wining_numbers_str = strsep(&temp,"|");
      char *present_numbers_str = temp;

      // Get wining numbers
      int wn[WN_NUM];
      for(int index = 0 ; index < WN_NUM ; index++, wining_numbers_str += 3){
	wn[index] = atoi(strpbrk(wining_numbers_str,"0123456789"));
      }

#ifdef DEBUG
      fprintf(stdout,"Wining numbers  = ");	
      for(int i = 0; i < WN_NUM ; i++){
	if(wn[i] != -1)
	  fprintf(stdout,"[%2i]",wn[i]);	
      }
      fprintf(stdout,"\n");
#endif
	    
      // Get present numbers
      int pn[PN_NUM];
      for(int index = 0 ; index < PN_NUM ; index++, present_numbers_str += 3){
	pn[index] = atoi(strpbrk(present_numbers_str,"0123456789"));
      }

#ifdef DEBUG
      fprintf(stdout,"Present numbers = ");	
      for(int i = 0; i < PN_NUM ; i++){
	if(pn[i] != -1)
	  fprintf(stdout,"[%2i]",pn[i]);	
      }
      fprintf(stdout,"\n");	
#endif
      
      int points = 0;
      for(int i = 0 ; i < PN_NUM ; i++){
	int number = pn[i];
	if (number != -1){
	  for(int j = 0 ; j < WN_NUM ; j++){
	    if(wn[j] == number){
	      matches[card_id-1]++;
	      if (points)
		points *= 2;
	      else
		points = 1;
#ifdef DEBUG
	      fprintf(stdout,"=== %2i is a winner!\n",number);
#endif
	      break;
	    }	  
	  }
	}
      }
#ifdef DEBUG
      fprintf(stdout,">>>>>>>>>>>>>>>>> Points for card[%3i] = %3i\n",card_id,points);
#endif
      free(copy);
      sum += points;
    }

    if (atoi(argv[2]))
      fprintf(stdout,"Points = %lli\n",sum);
    else     
      for(int i = 0 ; i < num_cards ; i++){
	fprintf(stdout,"%i\n",matches[i]);
      }
    
    free(matches);
    free(buffer);
    fclose(file);    
  }
  
  exit(EXIT_SUCCESS);
}

  
