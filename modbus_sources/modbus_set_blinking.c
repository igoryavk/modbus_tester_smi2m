#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <modbus/modbus.h>
#include <errno.h>

#define BLINKING_REGISTER 4102

int main()
{
    const char* port="/dev/ttyUSB0";
    int speed=115200;
    char parity='N';
    int stop_bits=1;
    int data_bits=8;
    
    modbus_t *modbus;
    int slave_id=1;

    modbus=modbus_new_rtu(port,speed,parity,data_bits,stop_bits);
    if (modbus==NULL)
    {
        fprintf(stderr,"Ошибка создания контекста: %s \n",modbus_strerror(errno));
        return 1;
    }

    modbus_set_slave(modbus,slave_id);

    modbus_set_response_timeout(modbus,2,0);

    if (modbus_connect(modbus)==-1)
    {
        fprintf(stderr,"Ошибка соединения: %s \n",modbus_strerror(errno));
        modbus_free(modbus);
        return 1;
    }

    printf("Соединение установлено\n");

    int blink=0;

    if (modbus_write_register(modbus,BLINKING_REGISTER,blink)==-1)
    {
        fprintf(stderr,"Не удалось записать регистр : %s \n",modbus_strerror(errno));
        modbus_close(modbus);
        modbus_free(modbus);
    }

    modbus_close(modbus);
    modbus_free(modbus);

    return 0;
}