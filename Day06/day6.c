#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <math.h>


#define NUM_TEST (3)
long double Time_test[NUM_TEST] = {7.0, 15.0, 30.0};
long double Distance_test[NUM_TEST] = {9.0, 40.0, 200.0};

#define NUM (4)
long double Time[NUM] = {48.0, 93.0, 84.0, 66.0};
long double Distance[NUM] = {261.0, 1192.0, 1019.0, 1063.0};

long double Time_2 = 48938466.0;
long double Distance_2 = 261119210191063.0;


//The problem boils down to the solving of the equation
// F(x) = -x^2 + Tx - D (1)
// The number of ways is the number of integers 
// between x1 and x2  where x1 and x2 are the solutions to eq. (1)
// Therefore we need to "round" x1 and x2 in the right way,
// especially when x1 and x2 are integers AND solutions
// to eq. (1) (e.g case where T = 30 and D = 200)
// so the nearest integer superior to x1 I so that 
// F(I) > 0 is floorl(x1 + 1.0) and the nearest integer
// J inferior to x2 so that F(J) > 0 is ceill(x2 - 1.0)
//
// Also, a good approximation of the solution can be found
// with computing delta^(1/2) for equation (1) and rounding it.

long int approx(long double time, long double distance)
{
  return (long int)(nearbyint(sqrt(time*time - 4.0*distance)));
}

long int calcul(long double time, long double distance)
{
  long double val_inf = ((time - sqrt(time*time - 4.0*distance))/2.0);
  long double val_sup = ((time + sqrt(time*time - 4.0*distance))/2.0);
  return ((long int)ceill(val_sup-1.0) - (long int)floorl(val_inf+1.0) + 1);
}

int main(int argc, char *argv[])
{
  long int power = 1;
  for(int race_num = 0; race_num < NUM_TEST ; race_num++){
    long int res_approx = approx(Time_test[race_num],Distance_test[race_num]);
    long int res        = calcul(Time_test[race_num],Distance_test[race_num]);
#ifdef DEBUG
    fprintf(stdout,"Approx = %li | Calcul = %li\n",res_approx,res);
#endif
    power *= res;
  }
  fprintf(stdout,"TEST  ================== %li\n",power);

  power = 1;
  for(int race_num = 0; race_num < NUM ; race_num++){
    long int res_approx = approx(Time[race_num],Distance[race_num]);
    long int res        = calcul(Time[race_num],Distance[race_num]);
#ifdef DEBUG
    fprintf(stdout,"Approx = %li | Calcul = %li\n",res_approx,res);
#endif
    power *= res;
  }
  fprintf(stdout,"PART1 ================== %li\n",power);
 
  long int res_approx = approx(Time_2,Distance_2);
  long int res        = calcul(Time_2,Distance_2);
#ifdef DEBUG
  fprintf(stdout,"Approx = %li | Calcul = %li\n",res_approx,res);
#endif
  fprintf(stdout,"PART2 ================== %li\n",res);
  
  exit(EXIT_SUCCESS);
}

  
