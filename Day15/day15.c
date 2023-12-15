#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <unistd.h>
#include <math.h>

#define MAX (10)
#define NBOXES (256)

struct el {
  char label[MAX+1];
  int  focal;
  struct el *next;
};

struct el *head = NULL;

struct el *enqueue_or_replace(struct el *head, char *label,int focal)
{
  if (head == NULL){
    struct el *tmp = calloc(1,sizeof(struct el));
    memcpy(tmp->label,label,strlen(label));
    tmp->focal = focal;    
    tmp->next  = NULL;
    return tmp;
  } else {
    struct el *curr = head;
    struct el *prev = NULL;    
    while(curr){
      // replace lens with new focal value
      if(strcmp(curr->label, label) == 0){
	curr->focal = focal;
	return head;
      }
      prev = curr;
      curr = curr->next;
    }
    // no lens was found, enqueue
    struct el *tmp = calloc(1,sizeof(struct el));
    memcpy(tmp->label,label,strlen(label));
    tmp->focal = focal;    
    tmp->next  = NULL;
    prev->next = tmp;
  }
  return head;
}

struct el *remove_el(struct el *head, char *label)
{
  if (head){
    struct el *curr = head;
    struct el *prev = NULL;    
    while(curr){
      // remove label from box
      if(strcmp(curr->label, label) == 0){
	// last element, just remove it
	if(curr->next == NULL){
	  free(curr);
	  if(prev != NULL){
	    prev->next = NULL;
	    return head;
	  } else { //it was the single element in the list!
	    assert(curr == head);
	    return NULL;
	  }
	} else {
	  if (prev != NULL){
	    prev->next = curr->next;
	    free(curr);
	  } else { // removing head
	    assert(curr == head);
	    prev = curr->next;
	    free(curr);
	    return prev;
	  }
	}
      }
      prev = curr;
      curr = curr->next;
    }
  }
  // no element was found, do nothing
  return head;
}

int focus_box(struct el *head, int num)
{
  int slot  = 0;
  int power = 0;
  
  if (head) {
    struct el *curr = head;
    while(curr){
      power += ((num+1) * (++slot) * curr->focal);   
      curr = curr->next;
    }
  }
  return power;
}

  
void print_box(struct el *head, int num)
{
  fprintf(stdout,"======================= Box[%i]: ",num);
  if(head == NULL){
    fprintf(stdout," is empty !!!!!!!! \n");
  } else {
    struct el *curr = head;
    while(curr){
      fprintf(stdout,"[ %3s | %2i ]",curr->label,curr->focal);      
      curr = curr->next;
    }
    fprintf(stdout,"\n");
  }
}

void print_boxes(struct el **array)
{
  fprintf(stdout,"===========================\n");
  for(int i = 0 ; i < NBOXES ; i++){
    if (array[i])
      print_box(array[i],i);
  }
  fprintf(stdout,"===========================\n");
}
 
int myhash(char *token)
{
  int val = 0;
  int temp = 0;
  assert(token);
  while(*token){
    temp = *token;
    val += temp;
    val *= 17;
    val %= 256;    
    token++;
  }
  return val;
}

long int convert(char *label){
  assert(label);

  long int val = 0;
  while(*label){
    val = val*100 +((*label) - 'a');
    label++;
  }
  return val;
}

int main(int argc, char *argv[])
{
  if (argc < 2){
    fprintf(stdout,"usage: ./day15_part1 <input_file>\n");
    exit(EXIT_FAILURE);
  } else {    
    char *buffer = NULL;
    size_t size = 0;
    ssize_t ret = 0;
    
    FILE *file = fopen(argv[1],"r");
    assert(file);

    ret = getline(&buffer,&size,file);
    buffer[strlen(buffer)-1] = '\0'; //remove EOL character

    fprintf(stdout,"Line is %li long\n",strlen(buffer));


    struct el *boxes[NBOXES];
    memset(boxes,0,NBOXES*sizeof(struct el *));    
    
    int value = 0;
    char *token = NULL;
    while((token = strsep(&buffer,","))){
      int hash = myhash(token);
      value += hash;
      fprintf(stdout,"token = %10s | val = %4i | Cumulative %8i\n",token,hash,value);

      //try for operation =      
      char *label = strsep(&token,"=");

      if(token != NULL){  //= case     
	assert(label);
	int box_num = myhash(label);
	assert((box_num >= 0) && (box_num < NBOXES));
	int focal = atoi(token);	
	boxes[box_num] = enqueue_or_replace(boxes[box_num],label,focal);
#ifdef DEBUG
	print_boxes(boxes);
#endif
	
      } else {	// - case 
	label = strsep(&label,"-");
	assert(label);
	int box_num = myhash(label);
	assert((box_num >= 0) && (box_num < NBOXES));
	boxes[box_num] = remove_el(boxes[box_num],label);
#ifdef DEBUG
	print_boxes(boxes);
#endif
      }
    }

    value = 0;
    for(int i = 0 ; i < NBOXES ; i++){
      value += focus_box(boxes[i], i);
    }

    fprintf(stdout,"==================== focusing power total = %i\n",value);
    
    free(buffer);
    fclose(file);    
  }
  
  exit(EXIT_SUCCESS);
}

  
