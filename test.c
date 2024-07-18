#include <stdio.h>
#include <stdarg.h>

int sum1(int x, ...) {
  va_list ap;
  va_start(ap, x);

  for (;;) {
    int y = va_arg(ap, int);
    if (y == 0)
      return x;
    x += y;
  }
}

int main()
{
    int x = sum1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0);
    printf("%d",x);
}