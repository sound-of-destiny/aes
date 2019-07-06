#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <omp.h>

#include "rijndael.h"


#define KEYBITS 256

int main(int argc, char **argv)
{
  struct timespec start, end, ready, stop;
  timespec_get(&start, TIME_UTC);
  unsigned long rk[RKLENGTH(KEYBITS)];
  unsigned char key[32] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 };
  int lSize = 0;
  int nrounds;

  FILE *output,*input;
  if (argc < 3)
  {
    fputs("Missing argument\n", stderr);
    return 1;
  }
  
  if ((input = fopen(argv[1], "r")) == NULL)
  {
    fputs("File open error", stderr);
    return 1;
  }
  
  fseek (input, 0, SEEK_END);
  lSize = ftell (input);
  rewind (input);

  unsigned char *iobuf = (unsigned char *)malloc(sizeof(unsigned char)*lSize);
  int retfread = fread (iobuf, 1, lSize, input);
  fclose (input);
  
  nrounds = rijndaelSetupEncrypt(rk, key, 256);
  
  timespec_get(&ready, TIME_UTC);
  
  int paddedSize = ceil (lSize / 16.0) * 16;  
  for (int k = lSize; k < paddedSize; k++)
            iobuf[k] = ' ';
  int dataPos;
  #pragma omp parallel for
  for (dataPos = 0; dataPos < paddedSize; dataPos += 16)
  {  
    rijndaelEncrypt (rk, nrounds, iobuf + dataPos, iobuf + dataPos);
    //printf("hello from thread %d",omp_get_thread_num());
  }
  
  timespec_get(&stop, TIME_UTC);
  
  if ((output = fopen(argv[2], "w")) == NULL)
  {
    fputs("File write error", stderr);
    return 1;
  }
  fwrite (iobuf, 1, lSize, output);
  fclose(output);
  
  timespec_get(&end, TIME_UTC);
  
  time_t d_sec = end.tv_sec - start.tv_sec;
  long d_nsec = end.tv_nsec - start.tv_nsec;
  time_t l_sec = stop.tv_sec - ready.tv_sec;
  long l_nsec = stop.tv_nsec - ready.tv_nsec;
  double duration = d_sec + d_nsec/1000000000.0;
  double lessduration = l_sec + l_nsec/1000000000.0;
  printf("duration: %.9lf  %.9lf \n", duration, lessduration);
  
}
