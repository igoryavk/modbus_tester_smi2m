#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <modbus/modbus.h>

// КОНФИГУРАЦИЯ - МЕНЯЙТЕ ЭТИ ЗНАЧЕНИЯ ПОД ВАШЕ УСТРОЙСТВО
#define SERIAL_PORT "/dev/ttyUSB1"  // COM порт
#define BAUDRATE 115200             // Скорость обмена
#define PARITY 'N'                  // Четность: 'N' - none, 'E' - even, 'O' - odd
#define DATA_BITS 8                 // Биты данных
#define STOP_BITS 1                 // Стоповые биты
#define SLAVE_ID 1                  // ID ведомого устройства

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

// Функция для преобразования uint32 в два uint16 (big-endian)
void uint32_to_registers(uint32_t value, uint16_t *reg_high, uint16_t *reg_low) {
    *reg_high = (value >> 16) & 0xFFFF;
    *reg_low = value & 0xFFFF;
}

// Функция для преобразования двух uint16 в uint32 (big-endian)
uint32_t registers_to_uint32(uint16_t reg_high, uint16_t reg_low) {
    return ((uint32_t)reg_high << 16) | reg_low;
}

int main(int argc, char** argv) {
    int register_address = 0;
    uint32_t register_value = 0;
    
    if (argc != 3) {
        printf("Использование: %s <Register address> <Register value>\n", argv[0]);
        printf("Пример: %s 4102 1234567890\n", argv[0]);
        return 1;
    } else {
        register_address = atoi(argv[1]);
        register_value = strtoul(argv[2], NULL, 10);
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

    // Преобразуем uint32 в два регистра
    uint16_t reg_high, reg_low;
    uint32_to_registers(register_value, &reg_high, &reg_low);
    
    printf("Записываем 32-битное значение %u (0x%08X) в регистры %d и %d...\n", 
           register_value, register_value, register_address, register_address + 1);
    printf("Старшее слово: %u (0x%04X)\n", reg_high, reg_high);
    printf("Младшее слово: %u (0x%04X)\n", reg_low, reg_low);

    // Записываем два регистра
    uint16_t write_registers[2] = {reg_high, reg_low};
    rc = modbus_write_registers(ctx, register_address, 2, write_registers);
    if (rc == -1) {
        fprintf(stderr, "Ошибка записи регистров %d-%d: %s\n", 
                register_address, register_address + 1, modbus_strerror(errno));
        modbus_close(ctx);
        modbus_free(ctx);
        return 1;
    }

    printf("✓ 32-битное значение успешно записано в регистры %d-%d\n", 
           register_address, register_address + 1);

    // Чтение обратно для проверки
    uint16_t read_registers[2];
    rc = modbus_read_registers(ctx, register_address, 2, read_registers);
    if (rc == -1) {
        fprintf(stderr, "Ошибка чтения регистров для проверки: %s\n", 
                modbus_strerror(errno));
    } else {
        uint32_t read_value = registers_to_uint32(read_registers[0], read_registers[1]);
        
        printf("✓ Проверка:\n");
        printf("  Прочитано старшее слово: %u (0x%04X)\n", read_registers[0], read_registers[0]);
        printf("  Прочитано младшее слово: %u (0x%04X)\n", read_registers[1], read_registers[1]);
        printf("  Собранное 32-битное значение: %u (0x%08X)\n", read_value, read_value);
        
        if (register_value == read_value) {
            printf("✓ Запись и проверка успешны!\n");
        } else {
            printf("⚠ Значение не совпадает:\n");
            printf("  Записано: %u (0x%08X)\n", register_value, register_value);
            printf("  Прочитано: %u (0x%08X)\n", read_value, read_value);
            printf("  Разница: %d\n", (int)(register_value - read_value));
        }
    }

    // Закрытие соединения
    modbus_close(ctx);
    modbus_free(ctx);

    printf("\nОперация завершена!\n");
    return 0;
}