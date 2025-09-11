#include <stdio.h>
#include <modbus/modbus.h>
#include <errno.h>
#include <stdlib.h>

#define SERIAL_PORT "/dev/ttyUSB0"
#define BAUD_RATE 115200
#define PARITY 'N'
#define DATA_BITS 8
#define STOP_BITS 1
#define REGISTER_VARTYPE 4107
#define REGISTER_STRING 4208
#define REGISTER_LEN 4249

int main(int argc,char** argv)
{
    modbus_t *modbus;
    int vartype=0;
    modbus= modbus_new_rtu(SERIAL_PORT,BAUD_RATE,PARITY,DATA_BITS,STOP_BITS);

    if (argc==2)
    {
        vartype=atoi(argv[1]);
    }

    if (modbus==NULL)
    {
        fprintf(stderr,"Не удалось создать контекст модбас: %s \n",modbus_strerror(errno));
        return 1;
    }

    modbus_set_debug(modbus,1);

    modbus_set_response_timeout(modbus,3,0);

    modbus_set_slave(modbus,1);


    if (modbus_connect(modbus)==-1)
    {
        fprintf(stderr,"Ошибка подключения: %s \n", modbus_strerror(errno));
        modbus_free(modbus);
        return 1;
    }

    if (modbus_write_register(modbus,REGISTER_VARTYPE,5)==-1)
    {
        fprintf(stderr,"\033[31mОшибка записи регистра: %s \n\033[0m",modbus_strerror(errno));
        modbus_close(modbus);
        modbus_free(modbus);
        return 1;
    }
    modbus_write_register(modbus,REGISTER_LEN,1);
    char sym='a';
    modbus_write_register(modbus,REGISTER_STRING,(u_int16_t)sym);

    printf("\033[32mРегистр успешно записан \n \033[0m");

    modbus_close(modbus);
    modbus_free(modbus);

    return 0;
}
