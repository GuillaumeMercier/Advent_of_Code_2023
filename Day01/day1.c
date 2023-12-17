#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>

#define NUM_DIGIT 10

char *DIGIT2STR[NUM_DIGIT]     = {"zero","one","two","three","four","five","six","seven","eight","nine"};
char *DIGIT2STR_REV[NUM_DIGIT] = {"orez","eno","owt","eerht","ruof","evif","xis","neves","thgie","enin"};

char *reverse_str(char *str)
{
  int size = strlen(str);

  char *reverse = calloc(size+1,sizeof(char));
  for(size_t index = 0; index < size - 1; index++){
    reverse[index] = str[size - index - 2]; // remove end of line caracter    
  }

  return reverse;
}

int find_digit(char *str, int mode)
{
#ifdef DEBUG
  fprintf(stdout,"line: %s\n", str);
#endif
  
  char *first_digit_addr = strpbrk(str,"0123456789");
  assert(first_digit_addr); // this assert can never fail because the pointer can never be NULL
  
  int value = *first_digit_addr - '0';

  if ( 2 == mode ){
    static int first_call = 0;
    first_call++;
   
    ptrdiff_t position_digit[NUM_DIGIT];
    ptrdiff_t first_digit_pos = first_digit_addr - str;
    
    for(int digit = 0 ; digit < NUM_DIGIT ; digit++){
      char *addr = strstr(str, (first_call%2) ? DIGIT2STR[digit] : DIGIT2STR_REV[digit]);
      position_digit[digit] = addr ? (addr - str) : -1;
    }
    
    for(ptrdiff_t digit = 0, min = first_digit_pos ; digit < NUM_DIGIT ; digit++){
      if((position_digit[digit] != -1) && (position_digit[digit] < min)){
	min = position_digit[digit];
	value = (int)digit;
      }
    }
  }
  
  return value;
}

int main(int argc, char *argv[])
{
  if (argc < 3){
    fprintf(stdout,"usage: ./day1 <input_file> <calibration mode (1/2)>\n");
    exit(EXIT_FAILURE);
  } else {
    char *buffer = NULL;
    size_t size = 0;
    ssize_t ret = 0;
    int sum = 0;
    FILE *file = fopen(argv[1],"r");
    int mode = atoi(argv[2]); // 1 == first cal, 2 = second cal
    assert(file);
        
    while ((ret = getline(&buffer,&size,file)) != -1) {

      int first = find_digit(buffer,mode);
#ifdef DEBUG
      fprintf(stdout,"First Value :%i\n",first);
#endif

      char *reverse = reverse_str(buffer);
      
      int second = find_digit(reverse,mode);
#ifdef DEBUG
      fprintf(stdout,"Second Value :%i\n", second);
#endif

      sum += first*10 + second;
      free(reverse);
    }

    
    fprintf(stdout,"Result = %i\n",sum);    
    free(buffer);
    fclose(file);
  }
  
  exit(EXIT_SUCCESS);
}
