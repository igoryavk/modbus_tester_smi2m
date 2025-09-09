#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <modbus/modbus.h>
#include <stdbool.h>

#define SERIAL_PORT "/dev/ttyUSB1"
#define BAUD_RATE 115200
#define PARITY 'N'
#define DATA_BITS 8
#define STOP_BITS 1
#define SLAVE_ID 1
#define REGISTER_ADDRESS 4252


void uint32_to_registers(uint32_t value,uint16_t *register1,uint16_t *register2, bool substitute_regs,bool substitute_bytes)
{

    if (substitute_regs)
    {
        *register1=(uint16_t)(value>>16);
        *register2=(u_int16_t)(value);
    }
    else
    {
        *register1=(uint16_t)(value);
        *register2=(u_int16_t)(value>>16);
    }

    if (substitute_bytes)
    {
        *register1=(*register1>>8)|(*register1<<8);
        *register2=(*register2>>8)|(*register2<<8);
    }
}


int main(int argc,char** argv)
{
    modbus_t* modbus;
    int rc=0;
    uint32_t register_value=0; 
    int mode=0;

    if (argc!=3)
    {
        printf("Использование: %s <Целочисленное_значение> <Режим перестановки байтов/регистров>\n",argv[0]);
        printf("Опции режима перестановки:\
            \n 0 - не менять порядок байтов регистров\
            \n 1 - инверсия байтов\
            \n 2 - инверсия регистров\
            \n 3 - инверсия и байтов и регистров\n");
        return 1;
    }
    else
    {
        register_value=atoi(argv[1]);
        mode=atoi(argv[2]);

        printf("Режим %d\n",mode);
    }

    //Создание Modbus RTU контекста
    modbus=modbus_new_rtu(SERIAL_PORT,BAUD_RATE,PARITY,DATA_BITS,STOP_BITS);

    if  (modbus==NULL)
    {
        fprintf(stderr,"Не удалось создать Modbus RTU контекст\n");
        return 1;
    }

    //Установка режима отладки 
    modbus_set_debug(modbus,1);

    //Установка таймаута 1 секунда
    modbus_set_response_timeout(modbus,1,0);

    //Установка Slave ID
    modbus_set_slave(modbus,SLAVE_ID);

    if (modbus_connect(modbus)==-1)
    {
        fprintf(stderr,"Ошибка подключения к Modbus устройству: %s \n",modbus_strerror(errno));
        modbus_free(modbus);
        return 1;
    }

    printf("Modbus устройство успешно подключено \n");

    //Преобразуем 32-битное число в два регистра для записи в зависимости 
    //от режима перестановки байт/регистров
    u_int16_t register1,register2;
    
    switch(mode)
    {
        case 0: // Не меняем порядок
            uint32_to_registers(register_value,&register1,&register2,false,false);
            break;
        case 1: // Меняем порядок байтов
            uint32_to_registers(register_value,&register1,&register2,false,true);
            break;
        case 2: // Меняем порядок регистров
            uint32_to_registers(register_value,&register1,&register2,true,false);
            break;
        case 3: // Меняем порядок и байтов и регистров
            uint32_to_registers(register_value,&register1,&register2,true,true);
            break;
    }
    //Выводим информацию о записываемых регистрах
    printf("Записываем 32-битное значение %u в регистры %d и %d \n",register_value,REGISTER_ADDRESS,REGISTER_ADDRESS+1);
    printf("Первый регистр: 0x%04X\n",register1);
    printf("Второй регистр: 0x%04X\n",register2);

    //Записываем регистры
    uint16_t write_registers[2]={register1,register2};

    rc=modbus_write_registers(modbus,REGISTER_ADDRESS,2,write_registers);

    if (rc==-1)
    {
        fprintf(stderr, "Ошибка записи регистров: %s",modbus_strerror(errno));
        modbus_close(modbus);
        modbus_free(modbus);
        return 1;
    }
    
    printf("Регистры успешно записаны\n");

    return 0;
}