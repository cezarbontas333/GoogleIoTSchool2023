#include <stdio.h>

int main()
{
    int numbers[3] = {18, 24, 96}, r1, r2, res;
    r1 = numbers[0] % numbers[1];
    while(r1 != 0)
    {
      numbers[0] = numbers[1];
      numbers[1] = r1;
      r1 = numbers[0] % numbers[1];
    }
    r2 = numbers[2] % numbers[0];
    while(r2 != 0)
    {
      numbers[2] = numbers[0];
      numbers[0] = r2;
      r2 = numbers[2] % numbers[0];
    }
    res = numbers[0];
    printf("%d \n", res);
}