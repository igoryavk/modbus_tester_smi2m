#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <modbus/modbus.h>
#include <errno.h>


//Определяем регистры для записи настроек
#define SPEED_REGISTER 521


#define CON_SPEED 115200
#define CON_STOPBITS 1
#define CON_PARITY 'N'
#define CON_DATABITS 8


int main(int argc, char** argv)
{
    int speed=10;
    int data_bits=0;
    int stop_bits=0;
    int parity=0;

    modbus_t *modbus;

    const char* port="/dev/ttyUSB1";

    if (argc!=2)
    {
        printf("\033[32mИспользование %s: <speed> <data_bits> <stop_bits> <parity>\033[0m\n\
            Скорость:\n\
            3 - 2400 бит\\c\n\
            4 - 4800 бит\\c\n\
            5 - 9600 бит\\c\n\
            6 - 14400 бит\\c\n\
            7 - 19200 бит\\c\n\
            8 - 38400 бит\\c\n\
            9 - 57600 бит\\c\n\
            10 - 115200 бит\\c\n",argv[0]);
    }
    else
    {
        speed=atoi(argv[1]);
    }

    // Создаем контекст модбас
    modbus=modbus_new_rtu(port,CON_SPEED,CON_PARITY,CON_DATABITS,CON_STOPBITS);
    if (modbus==NULL)
    {
            fprintf(stderr,"\033[31mОшибка создания модбас контекста\n\033[0m");
            return 1;
    }

    // Устанавливаем таймаут ответа
    modbus_set_response_timeout(modbus,1,0);
    // Устанавливаем slave id
    modbus_set_slave(modbus,1);


    if (modbus_connect(modbus)==-1)
    {
        fprintf(stderr,"Ошибка установки соединения\n");
        modbus_free(modbus);
        return -1;
    }

    printf("Соединение установлено\n");

    if (modbus_write_register(modbus,SPEED_REGISTER,speed)==-1)
    {
        fprintf(stderr,"Ошибка записи параметра скорости: %s \n",modbus_strerror(errno));
        modbus_close(modbus);
        modbus_free(modbus);
        return -1;
    }

    modbus_close(modbus);
    modbus_free(modbus);

    return 0;
}