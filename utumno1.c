#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define DEFAULTELFBASE 0x8048000

int main(int argc, char **argv) {
  long word;
  int outfp, devfp;
  int status = 0, ctr = 0, addr = DEFAULTELFBASE;
  pid_t pid = 0;
  pid = fork();
  devfp = open("/dev/null", O_RDONLY);
  if (pid) {
    wait(&status);
    if (WIFSTOPPED(status)) {
      close(devfp);
      outfp = open("dumped", O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
      while(1) {	
	errno = 0;
	word = ptrace(PTRACE_PEEKTEXT, pid, addr, NULL);
	if (errno) {
	  exit(-1);
	}
	addr += 4;
	write(outfp, &word, 4);
      }
      close(outfp);
    }
  }
  else {
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    dup2(devfp, 1);
    dup2(devfp, 2);
    execl("/wargame/level1", "level1", NULL);
  }
  return 0;
}
  
