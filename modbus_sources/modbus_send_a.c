/**
 * send_letter_A_fixed.c
 * Компиляция: gcc send_letter_A_fixed.c -o send_letter_A -lmodbus
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <modbus/modbus.h>
#include <unistd.h>
#include <errno.h>

// Адреса регистров из вашей карты
#define STRING_TYPE_REGISTER 4107      // Тип переменной
#define STRING_DATA_START 4208         // Начало данных строки
#define STRING_LENGTH_REGISTER 4249    // Длина строки (возможно read-only)

int main() {
    const char *port = "/dev/ttyUSB0";
    int baud = 115200;
    int slave_id = 1;
    const char *letter = "A";
    char parity = 'N';
    int data_bits = 8;
    int stop_bits = 1;

    printf("🚀 Отправка буквы '%s' на дисплей\n", letter);
    printf("📡 Порт: %s\n", port);
    printf("⚡ Скорость: %d бод\n", baud);
    printf("🎛️  Параметры: %d%c%d\n", data_bits, parity, stop_bits);
    printf("📟 Адрес устройства: %d\n", slave_id);
    printf("-----------------------------------------\n");

    // 1. Создаем Modbus контекст
    modbus_t *ctx = modbus_new_rtu(port, baud, parity, data_bits, stop_bits);
    if (ctx == NULL) {
        fprintf(stderr, "❌ Ошибка создания Modbus контекста\n");
        return 1;
    }

    modbus_set_slave(ctx, slave_id);
    modbus_set_response_timeout(ctx, 2, 0);

    // 2. Подключаемся к устройству
    printf("🔗 Подключаемся к устройству...\n");
    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "❌ Ошибка подключения: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return 1;
    }
    printf("✅ Подключение установлено\n");

    // 3. Устанавливаем строковый режим
    printf("🔄 Устанавливаем строковый режим...\n");
    if (modbus_write_register(ctx, STRING_TYPE_REGISTER, 5) == -1) {
        fprintf(stderr, "❌ Ошибка установки строкового режима\n");
        modbus_close(ctx);
        modbus_free(ctx);
        return 1;
    }
    printf("✅ Строковый режим установлен\n");

    // 4. Пропускаем установку длины строки (возможно не нужна)
    printf("⚠️  Пропускаем установку длины строки (регистр 4249)\n");
    printf("⚠️  Возможно длина устанавливается автоматически\n");

    // 5. Отправляем букву 'A' с нулевым терминатором
    printf("📤 Отправляем букву 'A'...\n");
    
    // Подготавливаем данные: 'A' + нулевой терминатор
    uint16_t string_data[2] = {0};
    string_data[0] = (uint16_t)letter[0];  // 'A'
    string_data[1] = 0x0000;               // Нулевой терминатор

    printf("   Данные: 0x%04X 0x%04X\n", string_data[0], string_data[1]);
    printf("   Адрес: %d\n", STRING_DATA_START);

    // 6. Отправляем 2 регистра (символ + терминатор)
    if (modbus_write_registers(ctx, STRING_DATA_START, 2, string_data) == -1) {
        fprintf(stderr, "❌ Ошибка отправки данных строки\n");
        
        // Пробуем отправить только 1 регистр
        printf("🔄 Пробуем отправить только символ 'A'...\n");
        if (modbus_write_registers(ctx, STRING_DATA_START, 1, string_data) == -1) {
            fprintf(stderr, "❌ Также ошибка при отправке одного регистра\n");
            modbus_close(ctx);
            modbus_free(ctx);
            return 1;
        }
        printf("✅ Символ отправлен (1 регистр)\n");
    } else {
        printf("✅ Данные отправлены (2 регистра)\n");
    }

    printf("🎯 На дисплее должна отобразиться буква: A\n");

    // 7. Альтернативный метод: читаем текущую длину строки
    printf("📖 Читаем текущую длину строки...\n");
    uint16_t current_length;
    if (modbus_read_registers(ctx, STRING_LENGTH_REGISTER, 1, &current_length) == -1) {
        printf("⚠️  Не удалось прочитать длину строки (регистр 4249)\n");
    } else {
        printf("✅ Текущая длина строки: %d\n", current_length);
    }

    sleep(2);
    modbus_close(ctx);
    modbus_free(ctx);

    printf("\n✨ Программа завершена!\n");
    return 0;
}