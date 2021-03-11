#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int main(int argc, char*argv[])
{
  time_t t;
  int arg_num = atoi(argv[1]);
  char chars[27]={'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',' '};
  srand((unsigned) time(&t));
  int randomNum = 0;



  for(int i = 0; i < arg_num; i++)
  {
    randomNum = rand() % 27;
    printf("%c",chars[randomNum]);
  }
  printf("\n");

  return 0;

}
