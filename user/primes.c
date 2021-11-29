#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char* argv[]) {
  int p[2];
  int buf[36];
  int pid = 0;
  // initialize.
  int index = 0;
  for (int i = 2; i <= 35; ++i) {
    buf[index++] = i;
  }
  // use the sieve function 埃氏筛法
  while (index > 0) {
    pipe(p);  // Create pipe every valid loop.
    if ((pid = fork()) == 0) {     // child
      // sieve operation.
      int prime = 0;
      int index = -1;
      int temp = 0;
      close(p[1]);
      while (read(p[0], &temp, sizeof(temp)) != 0) {
        // handle to sieve prime numbers.
        if (index < 0) {
          prime = temp;
          index++;
        } else {
          if (temp % prime != 0) buf[index++] = temp;
        }
      }
      printf("prime %d\n", prime);
      close(p[0]);
    } else if (pid > 0) {  // parent
      close(p[0]);
      for (int i = 0; i < index; ++i) {
        if (write(p[1], &buf[i], sizeof(buf[i])));
      }
      close(p[1]);
      wait(0);   // main process is waiting all child processes to quit.
      exit(0); 
    } else {
      fprintf(2, "fork failure.\n");
      exit(1);
    }
  }
  exit(0);
}