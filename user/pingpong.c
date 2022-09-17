#include "kernel/types.h"
#include "user/user.h"

int
main()
{
  int pid;
  int fds1[2], fds2[2];
  char buf[100];
  
  // create a pipe, with two FDs in fds[0], fds[1].
  pipe(fds1);
  pipe(fds2);

  pid = fork();
  if (pid == 0) {
    close(fds1[1]);
    close(fds2[0]);
    read(fds1[0], buf, sizeof(buf));
    printf("%d: received ping\n", getpid());
    write(fds2[1], "$", 1);
    exit(0);
  } 
  
  close(fds1[0]);
  close(fds2[1]);
  write(fds1[1], "$", 1);
  read(fds2[0], buf, sizeof(buf));
  printf("%d: received pong\n", getpid());
  
  exit(0);
}
