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

// nombre X finit par A ssi (X%100 == 0)
// nombre X finit par Z ssi ((X%25 == 0) && (X%100 != 0))

int check_for_Z(int tab[], int num)
{
  int ok = 1;
  for(int idx = 0 ; idx < num ; idx++){
    if(!((tab[idx]%25 == 0) && (tab[idx]%100))){
      ok = 0;
      break;
    }
  }
  return ok;
}

int main(int argc, char *argv[])
{
  if (argc < 2){
    fprintf(stdout,"usage: ./day8_part1 <input_file>\n");
    exit(EXIT_FAILURE);
  } else {
    char *buffer = NULL;
    size_t size = 0;
    ssize_t ret = 0;

    int numA = 0;
    int numZ = 0;
    
    FILE *file = fopen(argv[1],"r");
    assert(file);

    //determine max value
    val_max = tag2idx("ZZZ");

    //Allocate data
    el_t data[val_max];
    //memset(data,0,val_max*sizeof(el_t));
    for(int idx = 0 ; idx < val_max ; idx++){
      data[idx][L] = -1;
      data[idx][R] = -1;
    }
    
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
       fprintf(stdout,"============= indice:%i (%c%c%c)\n",indice,temp[0],temp[1],temp[2]);
#endif
       if((indice%100) == 0){
	 numA++;
	 //#ifdef DEBUG
	 fprintf(stdout,"==================%c%c%c (%i) finit par A\n",temp[0],temp[1],temp[2],indice);
	 //#endif
       }
       if(((indice%25) == 0) && (indice%100)){
	 numZ++;
#ifdef DEBUG
	 fprintf(stdout,"==================%c%c%c finit par Z\n",temp[0],temp[1],temp[2]);
#endif
       }
       
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
    assert(numA == numZ);
       
    //compute number of steps
    //init array of starting points
    int parcours[numA];
    int parcours_index = 0;
    for(int idx = 0 ; idx < val_max ; idx++){
      if ((idx%100 == 0) && ((data[idx][L] != -1) && (data[idx][R] != -1))){
	parcours[parcours_index++] = idx;
	fprintf(stdout,"============= starting point %i\n", idx);
      }
    }
    
   
    int num_steps[numA];
    for(int idx = 0 ; idx < numA ; idx++){
      num_steps[idx] = 0;
    }
    int dir_index = 0;
    int done = 0;
    while(check_for_Z(parcours, numA) == 0) {
      for(int idx = 0 ; idx < numA ; idx++){      
	if(((parcours[idx]%25 == 0) && (parcours[idx]%100))){
	  fprintf(stdout,"======================> Ghost %i done in %i\n",idx,num_steps[idx]);
	  if(++done == numA){

	    fprintf(stdout,"=== OK, now you need to compute the LCM of the %i numbers ...", numA); //(LCM is 8245452805243)
	    exit(EXIT_FAILURE);
	  }
	}
	parcours[idx] = (data[parcours[idx]])[dirs[dir_index]];
      }
#ifdef DEBUG
      for(int idx = 0 ; idx < numA ; idx++){      
	fprintf(stdout,"== %6i",parcours[idx]);
      }
      fprintf(stdout,"\n");
#endif
      dir_index = (dir_index + 1)%(strlen(directions));
      for(int idx = 0 ; idx < numA ; idx++){      
	num_steps[idx]++;
      }
    }
    
      //fprintf(stdout,"============= Number steps = %lli\n", num_steps);

    free(buffer);
    fclose(file);    
  }
  
  exit(EXIT_SUCCESS);
}

  
