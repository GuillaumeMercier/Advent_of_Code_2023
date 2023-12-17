#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>


// define a sparse vector
// tag is coded through the index in the array
typedef int el_t[2];
#define L (0)
#define R (L+1)
static int val_max = 0;

// Number format:
// [A-Z]*10000 + [A-Z]*100 + [A-Z] 0 -> 262626
int tag2idx(char tag[])
{
  return ((tag[0] - 'A')*10000 + (tag[1] - 'A')*100 + (tag[2] - 'A')) ;
} 

//parcours (++index)%(val_max+1)

int main(int argc, char *argv[])
{
  if (argc < 2){
    fprintf(stdout,"usage: ./day8_part1 <input_file>\n");
    exit(EXIT_FAILURE);
  } else {
    char *buffer = NULL;
    size_t size = 0;
    ssize_t ret = 0;

    FILE *file = fopen(argv[1],"r");
    assert(file);

    //determine max value
    val_max = tag2idx("ZZZ");

    //Allocate data
    el_t data[val_max];
    memset(data,0,val_max*sizeof(el_t));
    
    //get directions string
    ret = getline(&buffer,&size,file);
    buffer[strlen(buffer)-1] = '\0'; //remove EOL character
    char directions[strlen(buffer)+1];
    memcpy(directions,buffer,strlen(buffer)+1);
#ifdef DEBUG
    fprintf(stdout,"Directions : %s\n",directions);
#endif
    //Convert string into ints
    int dirs[strlen(buffer)];
    for(int idx = 0 ; idx < strlen(buffer) ; idx ++){
      if( directions[idx] == 'L') dirs[idx] = L;
      if( directions[idx] == 'R') dirs[idx] = R;
    }
#ifdef DEBUG
    fprintf(stdout,"Directions as ints: ");
    for(int idx = 0 ; idx < strlen(buffer) ; idx ++){
      fprintf(stdout,"%i-",dirs[idx]);
    }
    fprintf(stdout,"\n");
#endif
    
    //remove empty line
    ret = getline(&buffer,&size,file);

    //get rest of input
    while ((ret = getline(&buffer,&size,file)) != -1) {
       buffer[strlen(buffer)-1] = '\0'; //remove EOL character
       char *temp = calloc(strlen(buffer)+1,sizeof(char));
       memcpy(temp,buffer,strlen(buffer)+1);    
#ifdef DEBUG
       fprintf(stdout,"Input: %s\n",temp);
#endif
       temp = strpbrk(temp,"ABCDEFGHIJKLMNOPQRSTUVWXYZ");

       int indice =  tag2idx(temp);
#ifdef DEBUG
       fprintf(stdout,"============= indice:%i\n",indice);
#endif
       temp +=3;
       temp = strpbrk(temp,"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
       (data[indice])[L] = tag2idx(temp);
#ifdef DEBUG
       fprintf(stdout,"============= L = %i\n", (data[indice])[L]);
#endif
       temp +=3;
       temp = strpbrk(temp,"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
       (data[indice])[R] = tag2idx(temp);
#ifdef DEBUG
       fprintf(stdout,"============= R = %i\n", (data[indice])[R]);
#endif
    }

    //compute number of steps
    int parcours  = tag2idx("AAA"); //stating point
    int num_steps = 0;
    int dir_index = 0;
    while( parcours != val_max){
      parcours = (data[parcours])[dirs[dir_index]];
      dir_index = (dir_index + 1)%(strlen(directions));
      num_steps++;
    }

    fprintf(stdout,"============= Number steps = %i\n", num_steps);

    free(buffer);
    fclose(file);    
  }
  
  exit(EXIT_SUCCESS);
}

  
