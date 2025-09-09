#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

int main()
{
    uint32_t number;
 
    printf("\033[32m Программа перевода целочисленных данных в шестнадцатиричное представление\n\n \033[0m");
    printf("Введите целочисленное число:\n");
    scanf("%" SCNu32, &number);

    printf("Шестнадцатиричное представление числа: 0x%08" PRIx32 "\n\n", number);

    printf("Разбиение на регистры: \n");

    uint16_t high_register=(uint16_t)(number>>16);
    
    char high_byte=0;
    
    char low_byte=0;

    printf("Старший регистр: 0x%04X\n",high_register);

    high_byte=(char)(high_register>>8);
    low_byte=(char)(high_register);

    printf("Старший байт старшего регистра: 0x%02X\n",high_byte);
    printf("Младший байт старшего регистра: 0x%02X\n",low_byte);
    

    uint16_t low_register=(uint16_t)(number);

    printf("Младший регистр: 0x%04X\n",low_register);

    
    high_byte=(char)(low_register>>8);
    low_byte=(char)(low_register);

    printf("Старший байт младшего регистра: 0x%02X\n",high_byte);
    printf("Младший байт младшего регистра: 0x%02X\n",low_byte);
    



    
    return 0;
}