#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAXLINE 1024

int
main(int argc, char** argv) {
  if (argc > MAXARG) {
    fprintf(2, "xargs: arg is too much\n");
    return 0;
  }
  char* param[MAXARG];
  char line[MAXLINE];

  int pid;
  int n;
  int arg_param = 0;
  // the information of argv array is the param behind "|"
  char* cmd = argv[1];
  for (int i = 1; i < argc; ++i)
    param[arg_param++] = argv[i];
  if ((pid=fork()) > 0) {   // parent.
    wait(0);
  } else if (pid == 0) {    // child.
    int index = 0;  // index the each line.
    char* arg = (char*)malloc(sizeof(char));
    // read the STDIN that is before the "|" to param array.
    while((n = read(0, line, MAXLINE)) > 0) {
      for (int i = 0; i < n; ++i) {
        if (line[i] == ' ' || line[i] == '\n') {
          arg[index] = 0;  // '\0'
          printf("%s\n", arg);
          index = 0;
          param[arg_param++] = arg;
          arg = (char*)malloc(sizeof(char));
        } else {
          arg[index++] = line[i];
        }
      }
      exec(cmd, param);
    }
  } else {
    fprintf(2, "fork error\n");
    exit(1);
  }
  exit(0);
}