#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdarg.h>

int main() { 
	char *s[] = {"screen", "./b.out", NULL};
	execvp(s[0], s);
	return 0;
}