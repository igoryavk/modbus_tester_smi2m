#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <modbus/modbus.h>
#include <unistd.h>
#include <errno.h>

#define VARTYPE_REGISTER 4107
#define STRING_DATA_REGISTER 4208
#define RUN_STRING_REGISTER 4106
#define RUN_STRING_MODE 1
#define STRLENGTH_REGISTER 4249
#define STRLENGTH 20

#define STRING_MODE 5

int main(int argc,char** argv)
{
    const char *port="/dev/ttyUSB0";
    int baud=115200;
    int slave_id=1;
    const char *string_to_send=":::::::::\0";
    char parity='N';
    int data_bits=8;
    int stop_bits=1;

    //1. Cоздаем Модбас контекст
    modbus_t *modbus=modbus_new_rtu(port,baud,parity,data_bits,stop_bits);

    if (modbus==NULL)
    {
        fprintf(stderr,"\033[31mОшибка создания модбас контекста: %s\n\033[0m",modbus_strerror(errno));
        return 1;
    }

    // 2. Задаем адрес слейва  
    modbus_set_slave(modbus,slave_id);

    // 3. Устанавливаем таймаут ответа
    modbus_set_response_timeout(modbus,2,0);

    // 4. Подключаемся к устройству
    printf("Установка соединения с прибором\n");
    if (modbus_connect(modbus)==-1)
    {
        fprintf(stderr,"\033[31mОшибка подключения: %s \n\033[0m",modbus_strerror(errno));
        modbus_free(modbus);
        return 1;
    }
    printf("Соединение установлено\n");

    // 5. Устанавливаем строковый режим
    if (modbus_write_register(modbus,VARTYPE_REGISTER,STRING_MODE)==-1)
    {
        fprintf(stderr,"\033[31mОшибка установки строкового режима:%s\n\033[0m",modbus_strerror(errno));
        modbus_close(modbus);
        modbus_free(modbus);
        return 1;
    }
    printf("Установлен строковый режим \n");

    // 6. Устанавливаем  режим бегушей строки
    if (modbus_write_register(modbus,RUN_STRING_REGISTER,RUN_STRING_MODE)==-1)
    {
        fprintf(stderr,"\033[31mОшибка установки режима бегущей строки:%s\n\033[0m",modbus_strerror(errno));
        modbus_close(modbus);
        modbus_free(modbus);
        return 1;
    }
    printf("Установлен режим бегущей строки \n");

  
    // 6. Устанавливаем  длину строки 
    if (modbus_write_register(modbus,STRLENGTH_REGISTER,STRLENGTH)==-1)
    {
        fprintf(stderr,"\033[31mОшибка установки длины строки:%s\n\033[0m",modbus_strerror(errno));
        modbus_close(modbus);
        modbus_free(modbus);
        return 1;
    }
    printf("Длина строки установлена в значение %d симовола\n", STRLENGTH);

  


    // // 6. Подготавливаем данные для отправки 
    // u_int16_t string_data[4]={0};
    // string_data[0]=(u_int16_t)(string_to_send[0]|string_to_send[1]<<8);
    // string_data[1]=(u_int16_t)(string_to_send[2]|string_to_send[3]<<8);
    // string_data[2]=(u_int16_t)(string_to_send[4]|string_to_send[5]<<8);
    // string_data[3]=(u_int16_t)(string_to_send[6]|'\0'<<8);
    

    uint16_t string_data[STRLENGTH/2]={0};

    for (int i=0; i<STRLENGTH;i+=2)
    {
        string_data[i/2]=(u_int16_t)(string_to_send[i]|string_to_send[i+1]<<8);
    }


    // 7. Отправляем данные 
    if (modbus_write_registers(modbus,STRING_DATA_REGISTER,STRLENGTH/2,string_data)==-1)
    {
        fprintf(stderr,"\033[31mОшибка отправки данных:%s\n\033[0m",modbus_strerror(errno));
        modbus_close(modbus);
        modbus_free(modbus);
        return 1;
    }
    printf("Данные успешно отправлены \n");
    
    // 8. Закрываем cоединение и освобождаем ресурсы
    modbus_close(modbus);
    modbus_free(modbus);
    return 0;
}

