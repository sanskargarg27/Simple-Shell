#include <stdio.h>
#include <stdlib.h>

int main(){
	printf("Name: ");
	char* name;
	fgets(name, sizeof(name), stdin);
	printf("My name is %s\n", name);
	return 0;
}
