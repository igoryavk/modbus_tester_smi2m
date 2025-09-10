#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <modbus/modbus.h>
#include <unistd.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include <errno.h>

#define STRING_REGISTER_START 4208
#define STRING_LENGTH_REGISTER 4249
#define MAX_STRING_LENGTH 256
#define REGISTERS_PER_CHAR 1

int send_string(modbus_t * modbus_context, const char* str)
{
    int text_len=strlen(str);
    int registers_needed=(text_len+1)/2;
    if (registers_needed>128)
    {
        fprintf(stderr,"Строка слишком длинная \n");
        return 1;
    }

    u_int16_t string_data[]={0};

    for (int i=0;i<text_len;i=+2)
    {
        u_int16_t reg_value=str[i];
        if (i+1<text_len){
            reg_value|=(str[i+1]<<8);
        }
        string_data[i/2]=reg_value;
    }

      // Устанавливаем длину строки
    if (modbus_write_register(modbus_context, STRING_LENGTH_REGISTER, text_len) == -1) {
        fprintf(stderr, "❌ Ошибка записи длины строки\n");
        return 1;
    }

    // Записываем саму строку
    if (modbus_write_registers(modbus_context, STRING_REGISTER_START, registers_needed, string_data) == -1) {
        fprintf(stderr, "❌ Ошибка записи строки\n");
        return 1;
    }

}


void test_basic_strings(modbus_t *modbus_context)
{
    printf("Тест базовых строк \n");
    printf("===================\n");

    const char* strings[]={
        "Hello world",
        "Текст на русском",
        "1234567890"
    };

    for (int i=0;i<3;i++)
    {
        send_string(modbus_context,strings[i]);
    }



}


int main(int argc,char** argv)
{
    const char* port="/dev/ttyUSB1";
    int baud = 115200;
    int slave_id = 1;


    printf("\033[32m\nТест отправки  строк на Modbus индикатор Овен СМИ2-М\033[0m\n");

    modbus_t  *modbus= modbus_new_rtu(port,baud,'N',8,1);

    if (modbus==NULL){
        fprintf(stderr,"Не удалось создать контекст Модбас устройства: %s \n", modbus_strerror(errno));
        return 1;
    }

    modbus_set_slave(modbus,1);
    modbus_set_response_timeout(modbus,1,0);

    if (modbus_connect(modbus)==-1)
    {
        fprintf(stderr,"Не получилось установить Modbus соединение: %s \n",modbus_strerror(errno));
        modbus_free(modbus);
        return 1;
    }

    printf("\033[32mПодключение к СМИ2-М установлено \033[0m\n");

    //test_basic_strings(modbus);
    char symbol1='a';
    u_int16_t wr_reg=(u_int16_t)symbol1;

    printf("Регистр для записи: %08X\n",wr_reg);
    u_int16_t reg;
    
    if (modbus_write_register(modbus,4209,wr_reg)!=1)
    {
        fprintf(stderr,"Ошибка записи регистра: %s \n",modbus_strerror(errno));
    }
    modbus_read_registers(modbus,4208,1,&reg);

    printf("Прочитанный регистр: 0x%08X\n",reg);

    modbus_close(modbus);
    modbus_free(modbus);

    return 0;
}