#include <stdio.h>
#include <math.h>

static double a;
static double b;
static double c;
static double x1;
static double x2;

int main(void)
{
  printf("a = ");
  scanf("%lf", &a);

  printf("b = ");
  scanf("%lf", &b);

  printf("c = ");
  scanf("%lf", &c);

  double d = sqrt(b * b - 4 * a * c);

  x1 = (-b + d)/(2 * a);
  x2 = (-b - d)/(2 * a);

  printf("x1 = %lf\n", x1);
  printf("x2 = %lf\n", x2);
}
