#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>

#define NUM_CARDS      (5)  // number of cards in hand
#define NUM_CARD_TYPES (13) // A, K, Q , J, T , 9, 8, 7, 6, 5, 4, 3, 2
#define NUM_HAND_TYPES (7)  // See below types

typedef enum {
  HIGH_CARD  = ((unsigned char)0x00), // DEFAULT VALUE
  ONE_PAIR   = ((unsigned char)0x02), // ONE_PAIR = 2*HIGH_CARD
  TWO_PAIR   = ((unsigned char)0x04), // TWO_PAIR = ONE_PAIR + ONE_PAIR
  THREE_OF   = ((unsigned char)0x03), // CARD #
  FULL_HOUSE = ((unsigned char)0x05), // THREE_OF + ONE_PAIR
  FOUR_OF    = ((unsigned char)0x08), // can't use 4 b/c conflict with TWO_PAIR, take 2*(CARD #) instead
  FIVE_OF    = ((unsigned char)0x0a), // can't use 5 b/c conflict with FULL_HOUSE, take 2*(CARD #) instead
} hand_type_t;

hand_type_t hand_type_val[NUM_HAND_TYPES] = {
  HIGH_CARD,
  ONE_PAIR,
  THREE_OF,
  TWO_PAIR,
  FULL_HOUSE,
  FOUR_OF,
  FIVE_OF
}; 

char *hand_type_str[FIVE_OF+1] = {
  "HIGH_CARD",
  "DUMMY_01",//unused
  "ONE_PAIR",
  "THREE_OF",
  "TWO_PAIR",
  "FULL_HOUSE",
  "DUMMY_06",//unused
  "DUMMY_07",//unused
  "FOUR_OF",
  "DUMMY_09",//unused
  "FIVE_OF",
};

#define PRINT_HAND_TYPE(type)  do{fprintf(stdout,"Hand is of type %s\n",hand_type_str[type]);} while(0);

//sort values in ascending order to ease array access
static unsigned char indices[NUM_CARD_TYPES] = {50, 51, 52, 53, 54, 55, 56, 57, 65, 74, 75, 81, 84};
//                                               2   3   4   5   6   7   8   9   A   J   K   Q   T
#define ARRAY_SIZE (85)  // FIXME: replace w/ ((int)(indices[NUM_CARD_TYPES-1]+1)) 
#define GET_CARD_INDEX(c) (indices[(int)(c)])

typedef struct hand{
  char *cards;      //the hand
  int   bid;        //the bid
  unsigned char repart[ARRAY_SIZE]; //card count:  only NUM_CARD_TYPE values are used but faster acccess
  hand_type_t type; //hand type
  int  rank;        //Global hand rank
} hand_t;

// plus one for the total (sanity check)
static int hand_stats[FIVE_OF+2] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 
  
#define PRINT_HAND_STATS() do{                                                       \
    fprintf(stdout,"============ HANDS STATS ====\n");			             \
    for(int index = 0 ; index < NUM_HAND_TYPES ; index++){		             \
      fprintf(stdout,"============ %10s: %4i\n",hand_type_str[hand_type_val[index]], \
	      hand_stats[hand_type_val[index]]);			             \
    }									             \
    fprintf(stdout,">>>>>>>>>>>>>>>>> TOTAL: %4i\n",hand_stats[FIVE_OF+1]);         \
    fprintf(stdout,"==============================\n");			             \
  } while(0);

#define GATHER_HAND_STATS(hand_array, num_hands) do {	\
    for(int index = 0 ; index < (num_hands) ; index++){ \
      hand_stats[(int)((hand_array)[index].type)]++;    \
    }							\
    for(int index = 0 ; index <= FIVE_OF ; index++){	\
      hand_stats[FIVE_OF+1] += hand_stats[index];	\
    }							\
  } while(0);


/******************************************************/
/******************************************************/
/******************************************************/

void print_hand_array(hand_t array[], int num)
{
  fprintf(stdout,"== ARRAY OF *%s* HANDS\n",hand_type_str[array[0].type]); 
  for(int index = 0 ; index < num ; index++){				
    fprintf(stdout,"Hand[%4i] %s wins %4i \n",index, array[index].cards, array[index].bid); 
  }									
  fprintf(stdout,"===============================\n");			
} 

void swap_hands(hand_t *h1, hand_t *h2) 
{
  hand_t tmp = *h1;
  *h1 = *h2;
  *h2 = tmp;
}

// returns positive iff h1 > h2, 0 iff h1 == h2, negative otherwise
int compare_hands(hand_t h1, hand_t h2, int strengths[])
{
  for(int index = 0 ; index < NUM_CARDS ; index ++){ 
    if((strengths[(int)(h1.cards[index])]) != (strengths[(int)(h2.cards[index])])){
      return (strengths[(int)(h1.cards[index])] - strengths[(int)(h2.cards[index])]);
    }    
  }
  return 0;
}

//simple bubble sort 
void bsort(hand_t tab[], int size_tab, int str[], int (*compare_hands)(hand_t, hand_t, int []))
{  
  if (size_tab > 1)
    {
      for(int i = 0 ; i < size_tab - 1 ; i++){
        if ( compare_hands(tab[i],tab[i+1], str) > 0){
          swap_hands(tab+i,tab+i+1);
	}
      }
      bsort(tab,size_tab-1,str,compare_hands);
    }
}         

unsigned char find_best_card(hand_t hand, int strengths[])
{
  unsigned char max = 0;
  unsigned char best_type = 0;
  for(unsigned char card_type = 0 ; card_type < (unsigned char)NUM_CARD_TYPES ; card_type++){

    //skip Joker cards
    if ( GET_CARD_INDEX(card_type) == 'J' )
      continue;
	
    unsigned char num_cards = hand.repart[GET_CARD_INDEX(card_type)];

    if ((num_cards > max) ||
	((num_cards == max) && (strengths[GET_CARD_INDEX(card_type)] > strengths[best_type]))){
      max = num_cards;
      best_type = GET_CARD_INDEX(card_type);
    }
  }
#ifdef DEBUG
  fprintf(stdout,"Found best card : %c\n",best_type);
#endif
  return best_type;
}

int main(int argc, char *argv[])
{
  if (argc < 2){
    fprintf(stdout,"usage: ./day7_part1 <input_file>\n");
    exit(EXIT_FAILURE);
  } else {
    char *buffer = NULL;
    size_t size = 0;
    ssize_t ret = 0;

    FILE *file = fopen(argv[1],"r");
    assert(file);

    //init card_strengths
    int card_strength[ARRAY_SIZE];// to compare cards      
    memset(card_strength,0,ARRAY_SIZE*sizeof(int));
    card_strength['A'] = 1000;
    card_strength['K'] =  100;
    card_strength['Q'] =   50;
    card_strength['J'] =    1;  // J is now the weakest card 
    card_strength['T'] =   10;
    for(int c = '2' ; c <= '9' ; c++){
      card_strength[c] = (int)((char)c - '0');
    }
    
#ifdef DEBUG
    for(unsigned char idx = 0 ; idx < (unsigned char)NUM_CARD_TYPES ; idx++){
      fprintf(stdout,"===> Strength of card %c is %4i\n",indices[idx],card_strength[indices[idx]]);
    }
#endif
    
    int num_hands = 0;
    while ((ret = getline(&buffer,&size,file)) != -1) {
      num_hands++;
    }
    rewind(file);
    fprintf(stdout,"Number of hands = %i\n",num_hands);
    
    hand_t *hand_array = calloc(num_hands,sizeof(hand_t));
    int index = 0; 
    while ((ret = getline(&buffer,&size,file)) != -1) {
       buffer[strlen(buffer)-1] = '\0'; //remove EOL character
#ifdef DEBUG
       fprintf(stdout,"Input: %s\n",buffer);
#endif
       char *temp = calloc(strlen(buffer)+1,sizeof(char));
       memcpy(temp,buffer,strlen(buffer)+1);    
       hand_array[index].cards = strsep(&temp," ");
       hand_array[index].bid   = atoi(strpbrk(temp,"0123456789"));
       
#ifdef DEBUG
       fprintf(stdout,"Hand[%4i] %s wins %4i --- ",index,
	       hand_array[index].cards,
	       hand_array[index].bid);  
#endif
       
       //Check number of various cards in hand
       for(int idx = 0 ; idx < NUM_CARDS ; idx++){
	 int location = (int)(hand_array[index].cards[idx]);
	 (hand_array[index].repart[location])++;
#ifdef DEBUG
	 fprintf(stdout,"adding to place %c\n",(char)location);
#endif
       }

       //Joker management
       int num_jokers = hand_array[index].repart[(int)'J'];
#ifdef DEBUG
       fprintf(stdout,"========== Hand %s has %i jokers\n",hand_array[index].cards,num_jokers);
#endif

	 for(int idx = 0 ; idx < num_jokers ; idx++){	   
	   unsigned char best_card = find_best_card(hand_array[index],card_strength);
#ifdef DEBUG
	   fprintf(stdout,"================== Best card is %c\n",best_card);
#endif
	   (hand_array[index].repart[(int)(best_card)])++;
	 }	 
       
       // Determine hand type
       for(int card_type = 0 ; card_type < NUM_CARD_TYPES ; card_type++){
	 unsigned char num_cards = hand_array[index].repart[GET_CARD_INDEX(card_type)];
#ifdef DEBUG
	 if( num_cards)
	   fprintf(stdout,"Hand[%i] has %i %c\n",index,num_cards,GET_CARD_INDEX(card_type));
#endif	 
	 if( num_cards > 1 ){	   
	   if ( GET_CARD_INDEX(card_type) != 'J' ){ // skip joker cards
	     if(num_cards <= THREE_OF) 
	       hand_array[index].type += num_cards;   //see hand_type_t definition 
	     else 
	       hand_array[index].type  = num_cards*2; //see hand_type_t definition 
	   }
	 }
       }       
#ifdef DEBUG
       PRINT_HAND_TYPE(hand_array[index].type);
#endif
       index++;
    }
    
    // dispatch hands by their types.
    GATHER_HAND_STATS(hand_array,num_hands);
#ifdef DEBUG
    PRINT_HAND_STATS();
#endif
    
    int max_size = (FIVE_OF+1); //max + 1
    hand_t *hand_sorted[max_size];
    memset(hand_sorted,0,max_size*sizeof(hand_t *));    
    for(int idx = 0 ; idx < NUM_HAND_TYPES ; idx++){
      int index = (int)hand_type_val[idx];
      hand_sorted[index] = calloc(hand_stats[index],sizeof(hand_t));
    }

    // to keep track of current position in each array
    int hands_indices[max_size];
    memset(hands_indices,0,max_size*sizeof(int));

    //copy hand struct into its array by type
    for(int idx = 0 ; idx < num_hands ; idx++){
      int index = (int)(hand_array[idx].type);
      (hand_sorted[index])[(hands_indices[index])++] = hand_array[idx];      
    }
    
#ifdef DEBUG
    for(int idx = 0 ; idx < NUM_HAND_TYPES ; idx++){
      int index = (int)(hand_type_val[idx]);
      print_hand_array(hand_sorted[index], hand_stats[index]);
    }
#endif

    //Sort arrrays of hands
    for(int idx = 0; idx < NUM_HAND_TYPES ; idx++ ){
      int     index = (int)(hand_type_val[idx]);
      hand_t *array = hand_sorted[index];
      int     size  = hand_stats[index];
      bsort(array, size, card_strength, compare_hands);
#ifdef DEBUG
      print_hand_array(array, size);
#endif
    }

    // Finally, compute winnings
    long long int winnings = 0;
    int rank = 1;
    // process in strength order
    // BEWARE : Due to value definitions;
    // THREE_OF and TWO_PAIR are inverted !!!
    // SO, we'll use this array instead:
    hand_type_t hand_type_ordered[NUM_HAND_TYPES] = {
      HIGH_CARD, ONE_PAIR, TWO_PAIR, THREE_OF, FULL_HOUSE, FOUR_OF, FIVE_OF};
    
    for(int type_num = 0  ; type_num < NUM_HAND_TYPES ; type_num++){
      int type       = (int)(hand_type_ordered[type_num]);
      int array_size = hand_stats[type];
#ifdef DEBUG
      fprintf(stdout,"========= Array %s (%i)\n", hand_type_str[type],array_size );
#endif
      for(int idx = 0 ; idx < array_size ; idx++){
	winnings += (long long int)(rank*((hand_sorted[type])[idx].bid));
#ifdef DEBUG
	fprintf(stdout,"======= Bid %4i [ Rank %4i | total winnings %10lli\n",
		(hand_sorted[type])[idx].bid,rank, winnings );
#endif
	rank++;		
      }      
    }
        
    fprintf(stdout,"Total Winnings = %lli\n",winnings);
    free(hand_array);
    fclose(file);    
  }
  
  exit(EXIT_SUCCESS);
}

  
