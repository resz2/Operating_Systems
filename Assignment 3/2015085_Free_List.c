#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void issue_malloc(int );
void issue_free(int );

void* free_list[100001];
int size[100001];
int count = 0;

int main() {
	char s[10];
	int n;
	while (1) {
		scanf ("%s",s);
		if (strcmp(s,"print") == 0) {
			int k = 0;
			if (count > 0) {
				int flag = 0;
				for (int i=0; i<count; i++) {
					if (size[i] > 0) {
						flag = 1;
						printf ("%d. malloc %4d %p\n", ++k, size[i], free_list[i]);
					}
				}
				if (flag == 0) {
					printf("No memory allocated.\n");
				}
			}
			else{
				printf("No malloc calls yet.\n");
			}
		}
		else if (strcmp (s, "malloc") == 0 || strcmp(s, "free") == 0) {
			scanf ("%d",&n);
			if (strcmp(s, "malloc") == 0) {
				issue_malloc(n);
			}
			else if (strcmp(s, "free") == 0) {
				issue_free(n-1);
			}
		}
	}
	return 0;
}

void issue_malloc(int n) {
	free_list[count] = malloc(n);
	size[count] = n;
	count++;
	printf ("Memory of size %d allocated at %p\n", size[count-1], free_list[count-1]);
}

void issue_free (int n) {
	if (n <= count && n >= 0) {
		if (size[n] > 0) {
			printf("Memory freed at %p\n", free_list[n]);
			free(free_list[n]);
			size[n] = 0;
		}
		else {
			printf("Memory already freed.\n");
		}
	}
	else {
		printf("Memory to be freed not allocated.\n");
	}
}