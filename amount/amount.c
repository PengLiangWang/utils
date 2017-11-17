#include <stdio.h>
#include "abround.h"

int main()
{

    //double转long型(如果double型等于 3.9, long型为 3,  所以要加0.5再转)
    long Amount = 0;
    double setamount = 3.9;
    Amount = setamount + 0.5;
    printf("Amount: %d\n", Amount);

    double amount = 59.85;   //小数点后是9， 或4舍五入后是9，  AB_ROUND_01 也会进位， 其他情况舍去(满0.85进1)
    AB_ROUND_01(&amount, 0);
    printf("Amount: %f\n", amount);


    //直接全舍去  用double赋值给long就可以了
    double amount1 = 59.85;
    long amount2 = amount1;
    printf("amount2: %d\n", amount2);



    /* AB_ROUND 函数*/
    double money1 = 1.45456; 
    AB_ROUND(&money1, 2);    //4舍5入
    printf("money1: %f\n", money1);

    return 0;
}
