#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

char buf[1];

int
main(int argc, char* argv[]) {
  int p[2];
  pipe(p);
  int pid = fork();
  if (pid > 0) {  // parent
    close(p[0]);
    if (write(p[1], buf, sizeof(buf)) > 0) {
      wait(0);
      printf("%d: received pong\n", getpid());
    } else {
      return 0;
    }
    close(p[1]);
  } else if (pid == 0) {  // child
    close(p[1]);
    if (read(p[0], buf, sizeof(buf)) > 0) {
      printf("%d: received ping\n", getpid());
    } else {
      return 0;
    }
    close(p[0]);
  } else {  // error
    fprintf(2, "error\n");
    exit(1);
  }
  exit(0);
}