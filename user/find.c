#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path, char *name) {
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if(st.type != T_DIR) {
    return;
  }
  
  if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      return;
  }
  strcpy(buf, path);
  p = buf+strlen(buf);
  *p++ = '/';
  while(read(fd, &de, sizeof(de)) == sizeof(de)){
    if(de.inum == 0)
      continue;

    //printf("de.name:%s\n", de.name);
    if(strcmp(de.name, ".")==0)
      continue;
    
     if(strcmp(de.name, "..")==0)
      continue;

    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;
    if(stat(buf, &st) < 0){
      printf("ls: cannot stat %s\n", buf);
      continue;
    }

    if(st.type == T_DIR) { // 递归搜索
        find(buf, name);
        continue;
    }

    if((st.type == T_FILE) && (strcmp(de.name, name) == 0)) {
      printf("%s\n", buf);
      continue;
    }
  }
  return;
}

int
main(int argc, char *argv[])
{
  if(argc < 3){
    printf("find argc err\n");
    exit(0);
  }
  find(argv[1], argv[2]);
  exit(0);
}

