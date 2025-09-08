#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>


int main()
{
    uint32_t number;

    printf("\033[31m Введите число uint32_t:\n \033[0m");
    scanf("%" SCNu32, &number);


    printf("\033[32mШестнадцатеричное представление: 0x%08" PRIX32 "\033[0m\n", number);
    return 0;
}