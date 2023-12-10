#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>

#define NUM_DIRS (8)
#define IS_SYMBOL(c) ((ispunct((c)) && ((c)!='.')) ? 1 : 0)

int num_digits(char *ptr, char *base_addr, int max)
{
  int value = 1;
  assert(isdigit(*ptr));
  if (((ptr+1) < (base_addr + max)) && (isdigit(*(ptr+1)))){
    value++;
    if (((ptr+2) < (base_addr + max)) && (isdigit(*(ptr+2))))
      value++;    
  }
  return value;
}

int get_num_value(char *str, int num)
{
  int num_value = 0;     
  for(int index = 0 ; index < num ; index++){
    num_value = (10*num_value + (*(str + index) - '0'));
  }
  return num_value;
}

int mypow10(int num)
{
  int value = 1;
  for(int index = 0 ; index < num ; index++){
    value *= 10;
  }
  return value;
}

void check_left(char *str, ptrdiff_t position, int *value, int *num)
{
  for(int index = 1 ; index <= 3 ; index++){ //max digits is 3	  
    if (((position-index) >=0) && (isdigit(str[position-index]))){ 
      *value += (str[position-index] - '0')*mypow10(index-1) ;	    
    } else {
      break;
    }
  }
  if(*value > 0){
    (*num)++;
#ifdef DEBUG
    fprintf(stdout,"Found left value : %i\n", *value);
#endif
  }
}

void check_right(char *str, ptrdiff_t position, int max, int *value, int *num)
{
  for(int index = 1 ; index <= 3 ; index++){ //max digits is 3	  
    if (((position+index) < max) && (isdigit(str[position+index]))){ 
      *value  = (*value)*10 + (str[position+index] - '0');	      
    } else {
      break;
    }
  }
  if(*value > 0){
    (*num)++;
#ifdef DEBUG
    fprintf(stdout,"Found right value : %i\n", *value);
#endif
  }
}

void get_up_or_down(char *str, ptrdiff_t position, int max, int *value, int *num)
{
  *value = str[position] - '0';	       
  for(int index = 1 ; index <= 2 ; index++){ 
    if (((position-index) >=0) && (isdigit(str[position-index]))){ 
      *value  += (str[position-index] - '0')*mypow10(index) ;	    
    } else {
      break;
    }
  }
  for(int index = 1 ; index <= 2 ; index++){
    if (((position+index) < max) && (isdigit(str[position+index]))){ 
      *value = (*value)*10 + (str[position+index] - '0');	      
    } else {
      break;
    }
  }
  if(*value > 0){
#ifdef DEBUG
    fprintf(stdout,"Found value above/under star : %i\n",*value);
#endif
    (*num)++;
  }
}

void check_line(char *str, ptrdiff_t position, int max, int *val1, int* val2, int *val3,  int *num)
{
  if (str){ //first check character above
    if(isdigit(str[position])){ //there is a number just above this star
      get_up_or_down(str, position, max, val1, num);
    } else { // No number just above, maybe in diagonal?
      check_left(str, position, val2, num);	
      check_right(str, position, max, val3, num);
    }
  }
}
  
long int get_gears_line(char *curr, char *prev, char *next, int max)
{
  long int ratio = 0;
  char *addr = curr; //keep base addr;
  
  // manage all stars in string curr
  do {
    long int gear = 1;
    char *star = NULL;
    ptrdiff_t position = 0;
    int values[NUM_DIRS] = {0};
    int num = 0;
    
    star = strpbrk(addr,"*"); // find first star in string
    if (!star) break;         // no star found 
    position = (star - curr); // position of * in line
   
    check_left(curr,  position,      values,                       &num);
    check_right(curr, position, max, values+1,                     &num);
    check_line(prev,  position, max, values+2, values+3, values+4, &num);
    check_line(next,  position, max, values+5, values+6, values+7, &num);

    if(2 == num){
      for(int index = 0 ; index < NUM_DIRS ; index++){
	if (values[index] != 0){
	  gear *= values[index];
	}
      }
#ifdef DEBUG
      fprintf(stdout,"====================== FOUND possible gear: %li \n",gear);      
#endif            
      ratio += gear;      
    }          
    addr = star + 1;    
  } while( addr < (curr + strlen(curr)));

  return ratio;
}

long int get_parts_line(char *curr, char *prev, char *next, int max)
{
  long int value = 0;
  char *addr = curr; //keep base addr;
  
  // manage all numbers in string curr
  do {
    int num  = 0;
    char *digit = NULL;
    ptrdiff_t first_pos = 0;
    // find first digit in string
    digit = strpbrk(addr,"0123456789");
    if (!digit) break; // no more numbers to check 
    // get the number of digits for this number
    first_pos = digit - curr;
    
    num  = num_digits(digit,addr,strlen(addr));
    // check left ((digit - curr) - 1) and right ((digit - curr) + num) positions in curr line
    if (((first_pos - 1) >= 0) && (IS_SYMBOL(curr[first_pos-1]))){
      value += get_num_value(digit,num);
      goto end;
    }
    else if (((first_pos + num) < max) && (IS_SYMBOL(curr[first_pos+num]))){
      value += get_num_value(digit,num);
      goto end;
    }
    else {
      //check prev line
      if(prev){
	for(ptrdiff_t index = -1 ; index < num + 1 ; index++){
	  if((((first_pos + index) >= 0) && ((first_pos+index) < max))
	     && (IS_SYMBOL(prev[first_pos+index]))){
	    value += get_num_value(digit,num);
	    goto end;
	  }
	}
      }
      //check next line      
      if (next){
	for(ptrdiff_t index = -1 ; index < num + 1 ; index++){
	  if((((first_pos + index) >= 0) && ((first_pos+index) < max))
	     && (IS_SYMBOL(next[first_pos+index]))){
	    value += get_num_value(digit,num);
	    goto end;
	  }
	}
      }
    }
  end:
    addr = (digit + num);    
  } while( addr < (curr + strlen(curr)));

  return value;
}


int main(int argc, char *argv[])
{
  if (argc < 3){
    fprintf(stdout,"usage: ./day3 <input_file> <mode 1/2>\n");
    exit(EXIT_FAILURE);
  } else {
    char *buffer = NULL;
    size_t size = 0;
    ssize_t ret = 0;
    long long int sum = 0;        
    FILE *file = fopen(argv[1],"r");
    assert(file);
    int mode = atoi(argv[2]);
    long int(* f_ptr[2])(char *curr, char *prev, char *next, int max) = { get_parts_line, get_gears_line};
    
#ifdef DEBUG 
    while ((ret = getline(&buffer,&size,file)) != -1) {
      buffer[strlen(buffer)-1] = '\0'; //remove EOL character      
      fprintf(stdout,"Line(%li): %s\n",strlen(buffer),buffer);
    }
    rewind(file);
#endif
    
    ret = getline(&buffer,&size,file);
    long int line_size = strlen(buffer)-1; 
    buffer[line_size] = '\0'; //remove EOL character      

    char prev[line_size];
    explicit_bzero(prev,0);
    char curr[line_size];
    explicit_bzero(curr,0);
    char next[line_size];
    explicit_bzero(next,0);
    
    memcpy(prev,buffer,line_size+1);
    
    ret = getline(&buffer,&size,file);
    buffer[line_size] = '\0'; //remove EOL character      
    memcpy(curr,buffer,line_size+1);

#ifdef DEBUG 
    fprintf(stdout,"=================================================================================================================================\n");
    fprintf(stdout,"Prev:\n");
    fprintf(stdout,"Curr: %s\n",prev);
    fprintf(stdout,"Next: %s\n",curr);
    fprintf(stdout,"=================================================================================================================================\n");
#endif

    long int val = 0;
    // handle first line (prev)
    val = (f_ptr[mode-1])(prev,NULL,curr,line_size);
      
#ifdef DEBUG
    fprintf(stdout,"=============== Val : %li\n",val);
#endif
    sum += val;
    
    while ((ret = getline(&buffer,&size,file)) != -1) {
      buffer[line_size] = '\0'; //remove EOL character      
      memcpy(next,buffer,line_size+1);

#ifdef DEBUG 
      fprintf(stdout,"=================================================================================================================================\n");
      fprintf(stdout,"Prev: %s\n",prev);
      fprintf(stdout,"Curr: %s\n",curr);
      fprintf(stdout,"Next: %s\n",next);
      fprintf(stdout,"=================================================================================================================================\n");
#endif
      val = (f_ptr[mode-1])(curr,prev,next,line_size);
      //val = get_sum_line(curr,prev,next,line_size);
#ifdef DEBUG
      fprintf(stdout,"=============== Val : %li\n",val);
#endif
      sum += val;
      
      memcpy(prev,curr,line_size+1);
      memcpy(curr,next,line_size+1);
    }

#ifdef DEBUG 
    fprintf(stdout,"=================================================================================================================================\n");
    fprintf(stdout,"Prev: %s\n",prev);
    fprintf(stdout,"Curr: %s\n",curr);
    fprintf(stdout,"Next: \n");
    fprintf(stdout,"=================================================================================================================================\n");
#endif

    // handle last line (prev)
    val = (f_ptr[mode-1])(curr,prev,NULL,line_size);

#ifdef DEBUG
    fprintf(stdout,"=============== Val : %li\n",val);
#endif
    sum += val;
    
    fprintf(stdout,"Result = %lli\n",sum);    
    free(buffer);
    fclose(file);    
  }
  
  exit(EXIT_SUCCESS);
}

  
