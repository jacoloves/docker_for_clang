#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void swapWithPointers(int* pnum1, int* pnum2) {
    int tmp;
    tmp = *pnum1;
    *pnum1 = *pnum2;
    *pnum2 = tmp;
}

int main()
{
    
    int n1 = 5;
    int n2 = 10;
    swapWithPointers(&n1, &n2);

    printf("%d\n", n1);
    printf("%d\n", n2);

    return 0;
}