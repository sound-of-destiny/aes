#include <stdio.h>
#include <time.h>
#include "rijndael.h"

#define KEYBITS 256

int main(int argc, char **argv)
{
  unsigned long rk[RKLENGTH(KEYBITS)];
  //unsigned char key[KEYLENGTH(KEYBITS)];
  int i;
  int nrounds;
  char *password;

  FILE *output,*input;
  if (argc < 2)
  {
    fputs("Missing argument\n", stderr);
    return 1;
  }
  
  struct timespec start, end;
  timespec_get(&start, TIME_UTC);
  
  unsigned char key[32] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 };
  
  output = fopen(argv[2], "w");
  if (output == NULL)
  {
    fputs("File write error", stderr);
    return 1;
  }
  nrounds = rijndaelSetupEncrypt(rk, key, 256);
  input = fopen(argv[1], "r");
  
  while (!feof(input))
  {
    unsigned char plaintext[16];
    unsigned char ciphertext[16];
    int j;
    for (j = 0; j < sizeof(plaintext); j++)
    {
      int c = getc(input);
      if (c == EOF)
        break;
      plaintext[j] = c;
    }
    if (j == 0)
      break;
    for (; j < sizeof(plaintext); j++)
      plaintext[j] = ' ';
    rijndaelEncrypt(rk, nrounds, plaintext, ciphertext);
    if (fwrite(ciphertext, sizeof(ciphertext), 1, output) != 1)
    {
      fclose(output);
      fputs("File write error", stderr);
      return 1;
    }
  }
  fclose(output);
  
  timespec_get(&end, TIME_UTC);
  
  time_t d_sec = end.tv_sec - start.tv_sec;
  long d_nsec = end.tv_nsec - start.tv_nsec;
  double duration = d_sec + d_nsec/1000000000.0;
  printf("duration: %.9lf \n", duration);
  
}
