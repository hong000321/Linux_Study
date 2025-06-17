#include <stdio.h>
#include <stdint.h>

int main(void){
    int32_t num1 = 2100000000;
    int64_t num2 = 2200000000;

    printf("int32 : %d\n",num1);
    printf("int64 d: %d\n",num2);
    printf("int64 ld: %ld\n",num2);
    printf("int64 lld: %lld\n",num2);
    return 0;

}