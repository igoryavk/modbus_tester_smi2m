#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <modbus/modbus.h>
#include <unistd.h>
#include <errno.h>

#define STATUS_REGISTER 4100

int main(int argc, char **argv)
{
    const char* port="/dev/ttyUSB0";
    int speed= 115200;
    int data_bits=8;
    int stop_bits=1;
    char parity='N';

    modbus_t *modbus;

    if (argc!=2)
    {
        printf("Использование: %s <Скорость>\n",argv[0]);
        return -1;
    }
    else
    {
        speed=atoi(argv[1]);
    }

    modbus=modbus_new_rtu(port,speed,parity,data_bits,stop_bits);
    if (modbus==NULL)
    {
        fprintf(stderr, "Ошибка создания модбас контекста\n");
        return -1;
    }

    modbus_set_response_timeout(modbus,2,0);

    modbus_set_slave(modbus,1);

    if (modbus_connect(modbus)==-1)
    {
        fprintf(stderr,"Ошибка соединения\n");
        modbus_free(modbus);
        return -1;
    }


    printf("Соединение установлено\n");

    u_int16_t status;

    if (modbus_read_registers(modbus,STATUS_REGISTER,1,&status)==-1)
    {
        fprintf(stderr,"Не удалось прочесть регистр: %s\n",modbus_strerror(errno));
        modbus_close(modbus);
        modbus_free(modbus);
        return -1;
    }
    
    printf("Регистр успешно прочитан : 0x%04X\n",status);

    modbus_close(modbus);
    modbus_free(modbus);
    

    return 0;
}