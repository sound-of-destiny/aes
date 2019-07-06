#include <stdio.h>
#include "rijndael.h"

#define KEYBITS 256

int main(int argc, char **argv)
{
  unsigned long rk[RKLENGTH(KEYBITS)];
  unsigned char key[32] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 };
  int i;
  int nrounds;
  char *password;
  FILE *input,*output;
  if (argc < 3)
  {
    fputs("Missing argument", stderr);
    return 1;
  }
  output = fopen(argv[2], "w");
  input = fopen(argv[1], "r");
  if (input == NULL)
  {
    fputs("File read error", stderr);
    return 1;
  }
  nrounds = rijndaelSetupDecrypt(rk, key, 256);
  while (1)
  {
    unsigned char plaintext[16];
    unsigned char ciphertext[16];
    int j;
    if (fread(ciphertext, sizeof(ciphertext), 1, input) != 1)
      break;
    rijndaelDecrypt(rk, nrounds, ciphertext, plaintext);
    fwrite(plaintext, sizeof(plaintext), 1, output);
  }
  fclose(input);
}
