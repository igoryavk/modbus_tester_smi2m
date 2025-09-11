#include <stdio.h>
#include <modbus/modbus.h>
#include <errno.h>
#include <unistd.h>

#define SERIAL_PORT "/dev/ttyUSB0"
#define BAUD_RATE 115200
#define PARITY 'N'
#define DATA_BITS 8
#define STOP_BITS 1

int main()
{
    modbus_t *modbus;
    uint16_t tab_reg[10];
    int i;

    modbus = modbus_new_rtu(SERIAL_PORT, BAUD_RATE, PARITY, DATA_BITS, STOP_BITS);
    if (modbus == NULL) {
        fprintf(stderr, "Не удалось создать контекст: %s\n", modbus_strerror(errno));
        return 1;
    }

    // Увеличиваем таймауты
    modbus_set_response_timeout(modbus, 5, 0);
    modbus_set_byte_timeout(modbus, 5, 0);
    
    modbus_set_debug(modbus, 1);
    modbus_set_slave(modbus, 1);

    if (modbus_connect(modbus) == -1) {
        fprintf(stderr, "Ошибка подключения: %s\n", modbus_strerror(errno));
        modbus_free(modbus);
        return 1;
    }

    // Пробуем прочитать регистры вместо записи
    printf("Пробуем прочитать регистры...\n");
    int rc = modbus_read_registers(modbus, 0, 10, tab_reg);
    
    if (rc == -1) {
        fprintf(stderr, "Ошибка чтения: %s\n", modbus_strerror(errno));
    } else {
        printf("Успешно прочитано %d регистров:\n", rc);
        for (i = 0; i < rc; i++) {
            printf("Регистр[%d] = %d (0x%X)\n", i, tab_reg[i], tab_reg[i]);
        }
    }

    modbus_close(modbus);
    modbus_free(modbus);
    return 0;
}