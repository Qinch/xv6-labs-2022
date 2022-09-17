#include "kernel/types.h"
#include "user/user.h"

#define MAXARGS 10
char whitespace[] = " \t\r\n\v";

// find . b | xargs grep hello
int getxargs(char *buf, int nbuf)
{
  memset(buf, 0, nbuf);
  int i;
  for(i = 0; i < nbuf - 1;) {
    char c;
    int cc = read(0, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
    if(c == '\n')
      break;
  }
  buf[i] = 0;

  if(buf[0] == 0) {
    return -1;
  }
  return 0;
}

int
main(int argc, char *argv[])
{
  // 读取xargs自带的参数
  char *params[MAXARGS];
  int i = 0, j;
  for(j = 1; j < argc; j++, i++) {
    params[i] = argv[j];
  }

  // 从标准输入0读取input,直到\n
  char buf[100];
  while(getxargs(buf, sizeof(buf)) >= 0) { // 循环读,以\n作为分行
    i = j - 1;
    // 对buf参数进行解析
    char *s = buf;
    char *es = buf + strlen(buf); // [s, es], buf[es]为0
    while(s < es) {
      // 过滤空格
      while(s < es && strchr(whitespace, *s))
        s++;
      if(s >= es)
        break;
      params[i++] = s;

      // 获取当前参数
      while(s < es && !strchr(whitespace, *s))
        s++;
      *(s++) = 0;
    }
    params[i] = 0;

    // 创建子进行去执行xargs命令
    int pid= fork();
    if(pid==0) {
      exec(params[0], params);
      printf("exec failed\n");
      exit(1);
    }
    wait(0);
  }
  exit(0);
}
