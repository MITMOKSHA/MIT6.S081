#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

/**
 * @brief transmit the complete filename path to the filename eradicated dir.
 * 
 * @param path complete filename path.
 * @return return the filename eradicated part behind the last slash "/".
 */
char*
fmtname(char* path) {
  static char buf[DIRSIZ+1];
  char* p;

  for (p = path+strlen(path); p >= path && *p != '/'; p--);
  p++;  // jump the slash.

  if (strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  /* fill the white space into the rest of the DIRSIZ that
   * guarantee the correctness of strcmp func.
  */
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  buf[strlen(p)] = 0;   // null-terminated string
  return buf;
}

void
find(char* path, char* name) {
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    close(fd);
    return;
  }

  switch (st.type)
  {
  case T_FILE:
    if (!strcmp(name, fmtname(path)))
      printf("%s\n", path);
    break;
  
  case T_DIR:
    if (strlen(path)+1+DIRSIZ+1 > sizeof(buf)) {  // the two "1" represent the null placeholder.
      printf("find: path too long\n");
      break;
    }

    /* buf store the complete path and hold the buf pointer in front of it that
     * is convenient to print the complete path which we expect to output.
    */
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';   // i.e. *(p++) = '/'.
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0)  // inode number == 0.
        continue;
      memmove(p, de.name, DIRSIZ);  // move the dirname to p.
      p[DIRSIZ] = 0;
      if (!strcmp(de.name, ".") || !strcmp(de.name, "..")) {
        continue;
      }
      find(buf, name);  // recursive.
    }
    break;
  }
  close(fd);
}

int
main(int argc, char** argv) {
  if (argc < 3) {
    fprintf(2, "usage: find path name...\n");
    return 0;
  }
  for (int i = 2; i < argc; ++i) {
    find(argv[1], argv[i]);
  }
  exit(0);
}