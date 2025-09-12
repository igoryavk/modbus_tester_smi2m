#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <modbus/modbus.h>
#include <unistd.h>
#include <errno.h>

#define COLOR_REGISTER 4100



int main(int argc,char **argv)
{
    modbus_t *modbus;
    const char* port="/dev/ttyUSB0";
    int speed=115200;
    const char parity='N';
    int data_bits=8;
    int stop_bits=1;

    int slave_id=1;
    int color=0;

    if (argc!=2)
    {
        printf("Укажите цвет!!!\n\
            0- зеленый\n\
            1- красный\n\
            2- желтый\n");
        return 1;
    }    
    else
    {
        color=atoi(argv[1]);
    }

    modbus=modbus_new_rtu(port,speed,parity,data_bits,stop_bits);
    if (modbus==NULL)
    {
        fprintf(stderr,"Ошибка создания контекста Модбас: %s\n",modbus_strerror(errno));
        return 1;
    }

    modbus_set_slave(modbus,slave_id);

    modbus_set_response_timeout(modbus,2,0);

    if (modbus_connect(modbus)==-1)
    {
        fprintf(stderr,"\033[31mОшибка открытия соединения:%s\n\033[0m",modbus_strerror(errno));
        modbus_free(modbus);
        return 1;
    }

    printf("\033[32mСоединение установлено\n\033[0m");

    if (modbus_write_register(modbus,COLOR_REGISTER,color)==-1)
    {
        fprintf(stderr,"\033[31mОшибка записи параметра:%s\n\033[0m",modbus_strerror(errno));
        modbus_close(modbus);
        modbus_free(modbus);
        return 1;
    }


    modbus_close(modbus);
    modbus_free(modbus);

    return 0;
}