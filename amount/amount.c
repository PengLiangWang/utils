#include <stdio.h>
#include "abround.h"

int main()
{
    /*金额以分为单位*/
    /*
    long orderamount=1289;
    long Amount;

    double fee = (double)orderamount/100 * 0.0005;

    printf("orderamount: %d\n", orderamount);
    printf("fee: %f\n", fee);
    AB_ROUND(&fee, 2);    //保留小数点后2位有效值, 4舍五入

    printf("fee: %f\n", fee);

    double setamount = (double)orderamount/100 - fee;
    printf("setamount: %f\n", setamount);

    AB_ROUND_01(&setamount, 1);     //小数点后保留几位有效值(不4舍五入), 参数不能为0
    printf("setamount: %f\n", setamount);

    //double转long型(如果double型等于 3.9, long型为 3,  所以要加0.5再转)
    Amount = setamount*100 + 0.5;
    printf("Amount: %d\n", Amount);
    */
    double amount = 59.84;   //小数点后是9， 或4舍五入后是9，  AB_ROUND_01 也会进位， 其他情况舍去
    AB_ROUND_01(&amount, 0);
    printf("Amount: %f\n", amount);


    //直接全舍去  用double赋值给long就可以了
    long amount2 = amount;
    printf("amount2: %d\n", amount2);

    return 0;
}
