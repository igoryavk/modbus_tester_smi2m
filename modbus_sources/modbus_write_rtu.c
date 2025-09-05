#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <modbus/modbus.h>

// КОНФИГУРАЦИЯ - МЕНЯЙТЕ ЭТИ ЗНАЧЕНИЯ ПОД ВАШЕ УСТРОЙСТВО
#define SERIAL_PORT "/dev/ttyUSB1"  // COM порт
#define BAUDRATE 115200               // Скорость обмена
#define PARITY 'N'                  // Четность: 'N' - none, 'E' - even, 'O' - odd
#define DATA_BITS 8                 // Биты данных
#define STOP_BITS 1                 // Стоповые биты
#define SLAVE_ID 1                  // ID ведомого устройства
#define REGISTER_ADDRESS 4102      // Адрес регистра для записи
#define REGISTER_VALUE 1         // Значение для записи

void print_config() {
    printf("=== Конфигурация Modbus RTU ===\n");
    printf("Serial port: %s\n", SERIAL_PORT);
    printf("Baudrate: %d\n", BAUDRATE);
    printf("Parity: %c\n", PARITY);
    printf("Data bits: %d\n", DATA_BITS);
    printf("Stop bits: %d\n", STOP_BITS);
    printf("Slave ID: %d\n", SLAVE_ID);
    printf("================================\n\n");
}

int main(int argc, char** argv) {

    int register_address=0;
    int register_value=0;
    if (argc !=3)
    {
        printf("Использование: %s <Register address> <Register value>",argv[0]);
        return 1;
    }
    else 
    {
        register_address=atoi(argv[1]);
        register_value=atoi(argv[2]);
    }

    modbus_t *ctx = NULL;
    int rc;

    // Выводим конфигурацию
    print_config();

    // Создание Modbus RTU контекста
    ctx = modbus_new_rtu(SERIAL_PORT, BAUDRATE, PARITY, DATA_BITS, STOP_BITS);
    if (ctx == NULL) {
        fprintf(stderr, "Не удалось создать Modbus RTU контекст\n");
        return 1;
    }

    // Установка режима отладки (опционально)
    modbus_set_debug(ctx, 1);

    // Установка таймаута (1 секунда)
    modbus_set_response_timeout(ctx, 1, 0);

    // Установка slave ID
    modbus_set_slave(ctx, SLAVE_ID);

    // Подключение к устройству
    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Ошибка подключения к %s: %s\n", 
                SERIAL_PORT, modbus_strerror(errno));
        modbus_free(ctx);
        return 1;
    }

    printf("Успешно подключились к Modbus RTU устройству\n");

    // Запись одного регистра
    printf("Записываем значение %d в регистр %d...\n", 
           register_value, register_address);
    
    rc = modbus_write_register(ctx, register_address, register_value);
    if (rc == -1) {
        fprintf(stderr, "Ошибка записи регистра %d: %s\n", 
                register_address, modbus_strerror(errno));
        modbus_close(ctx);
        modbus_free(ctx);
        return 1;
    }

    printf("✓ Значение успешно записано в регистр %d\n", register_address);

    // Чтение обратно для проверки
    uint16_t read_value;
    rc = modbus_read_registers(ctx, register_address, 1, &read_value);
    if (rc == -1) {
        fprintf(stderr, "Ошибка чтения регистра для проверки: %s\n", 
                modbus_strerror(errno));
    } else {
        printf("✓ Проверка: значение в регистре %d = %d\n", 
               register_address, register_value);
        
        if (register_value == read_value) {
            printf("✓ Запись и проверка успешны!\n");
        } else {
            printf("⚠ Значение не совпадает: записано %d, прочитано %d\n",
                   register_value, read_value);
        }
    }

    // Закрытие соединения
    modbus_close(ctx);
    modbus_free(ctx);

    printf("\nОперация завершена!\n");
    return 0;
}