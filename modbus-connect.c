#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <modbus/modbus.h>
#include <errno.h>

#define SLAVE_ADDRESS 1
#define REGISTER_ADDRESS 4102
#define NUMBER_OF_REGISTERS 1
#define SERIAL_PORT "/dev/ttyUSB1"
#define BAUD_RATE 115200
#define PARITY 'N'
#define DATA_BITS 8
#define STOP_BITS 1

int main() {
    modbus_t *ctx = NULL;
    uint16_t tab_reg[1] = {0};
    int rc;
    
    // Создание Modbus контекста для RTU
    ctx = modbus_new_rtu(SERIAL_PORT, BAUD_RATE, PARITY, DATA_BITS, STOP_BITS);
    if (ctx == NULL) {
        fprintf(stderr, "Не удалось создать Modbus контекст\n");
        return -1;
    }
    
    // Установка адреса slave устройства
    modbus_set_slave(ctx, SLAVE_ADDRESS);
    
    // Установка таймаутов
    modbus_set_response_timeout(ctx, 1, 0); // 1 секунда
    
    // Подключение к устройству
    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Ошибка подключения: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }
    
    printf("Подключение к Modbus SLAVE установлено\n");
    printf("Опрос регистра %d каждые 2 секунды...\n", REGISTER_ADDRESS);
    
    // Бесконечный цикл опроса
    while (1) {
        // Чтение holding регистра
        rc = modbus_read_registers(ctx, REGISTER_ADDRESS, NUMBER_OF_REGISTERS, tab_reg);
        
        if (rc == -1) {
            fprintf(stderr, "Ошибка чтения: %s\n", modbus_strerror(errno));
        } else {
            printf("Регистр %d: %d (0x%04X)\n", 
                   REGISTER_ADDRESS, tab_reg[0], tab_reg[0]);
        }
        
        sleep(2); // Пауза 2 секунды между опросами
    }
    
    // Закрытие соединения (этот код никогда не выполнится из-за бесконечного цикла)
    modbus_close(ctx);
    modbus_free(ctx);
    
    return 0;
}