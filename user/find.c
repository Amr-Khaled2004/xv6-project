#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find the first character after the last slash
  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // If the name is short enough, return it
  if (strlen(p) < DIRSIZ)
    return p;

  // Otherwise copy into a buffer
  memmove(buf, p, DIRSIZ);
  buf[DIRSIZ] = 0;
  return buf;
}

void
find(char *path, char *filename)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  fd = open(path, 0);
  if (fd < 0) {
    printf("find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    printf("find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  // Case 1: path is a FILE
  if (st.type == T_FILE) {
    if (strcmp(fmtname(path), filename) == 0)
      printf("%s\n", path);
    close(fd);
    return;
  }

  // Case 2: path is a DIRECTORY
  if (st.type == T_DIR) {
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
      printf("find: path too long\n");
      close(fd);
      return;
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0)
        continue;

      if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;

      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;  // null terminate

      find(buf, filename);   // recursion
    }
  }

  close(fd);
}

int
main(int argc, char *argv[])
{
  if (argc == 2 && strcmp(argv[1], "?") == 0) {
    printf("Usage: find directory filename\n");
    exit(0);
  }

  if (argc != 3) {
    printf("find: usage: find <directory> <filename>\n");
    exit(0);
  }

  find(argv[1], argv[2]);
  exit(0);
}
