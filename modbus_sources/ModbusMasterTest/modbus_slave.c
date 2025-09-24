#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <modbus/modbus.h>
#include <errno.h>


#define SERIAL_PORT "/dev/ttyUSB5"
#define BAUD_RATE 115200
#define PARITY 'N'
#define DATA_BITS 8
#define STOP_BITS 1

int main()
{
    modbus_t *modbus=NULL;
    int slave_id=1;
    modbus_mapping_t *mb_mapping;
    int rc;
    uint8_t query[MODBUS_RTU_MAX_ADU_LENGTH];


    printf("Запуск RTU slave \n");
    
    //Создаем модбас контекст для RTU
    modbus=modbus_new_rtu(SERIAL_PORT,BAUD_RATE,PARITY,DATA_BITS,STOP_BITS);
    
    if (modbus==NULL)
    {
        fprintf(stderr,"Не удалось создать модбас контекст \n");
        return 1;
    }
    //Устанавливаем адрес слейва
    modbus_set_slave(modbus,slave_id);


    //Открываем порт для прослушивания
    if (modbus_connect(modbus)==-1)
    {
        fprintf(stderr,"Не удалось открыть порт для modbus RTU: %s \n",modbus_strerror(errno));
        modbus_free(modbus);
        return 1;
    }

    //Создаем маппинг переменные для слейва
    mb_mapping=modbus_mapping_new(100,100,100,100);

    if (mb_mapping==NULL)
    {
        fprintf(stderr,"Не удалось создать mapping: %s \n", modbus_strerror(errno));
        modbus_close(modbus);
        modbus_free(modbus);
        return 1;
    }

    int count=0;
    while(1)
    {
        rc=modbus_receive(modbus,query);
        if (rc>0)
        {
            modbus_reply(modbus,query,rc,mb_mapping);
            mb_mapping->tab_registers[0]=count;
            count++;
            if (count==10)
            {
                sleep(10);
            }
        }
    }



    
    
    return 0;
}