// Feature: printf and scanf (multiple format args, char arrays, expressions as args)
#include <stdio.h>
int main() {
    int a;
    int b;
    float f;
    char name[20];
    printf("Enter two numbers: ");
    scanf("%d %d", &a, &b);
    scanf("%f", &f);
    scanf("%s", name);
    printf("Sum = %d, Product = %d\n", a + b, a * b);
    printf("Float: %f Name: %s\n", f, name);
    printf("Done\n");
    return 0;
}
