#include <stdio.h>

int fibonacci(int n) {
	if (n == 1 || n == 2)
		return 1;
	else
		return fibonacci(n - 1) + fibonacci(n - 2);
}

int n;
int m;

void main() {
	printf("n: ");
	scanf("%d", &n);

	m = fibonacci(n);

	printf("m = %d\n", m);
}
