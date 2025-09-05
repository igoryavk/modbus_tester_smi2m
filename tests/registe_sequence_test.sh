#!/bin/bash

# Адрес регистра
REGISTER=4250

# Значения в десятичной форме
VALUES=(
    $((0x000000FF))    # 255
    $((0x0000FF00))    # 65280
    $((0x00FF0000))    # 16711680
    $((0xFF000000))    # 4278190080
    $((0x00FF0000))    # 16711680
    $((0x0000FF00))    # 65280
)

echo "Запуск скрипта для записи значений в регистр $REGISTER"
echo "Цикл значений:"
echo "0x000000FF = 255"
echo "0x0000FF00 = 65280"
echo "0x00FF0000 = 16711680"
echo "0xFF000000 = 4278190080"
echo "0x00FF0000 = 16711680"
echo "0x0000FF00 = 65280"
echo ""
echo "Для остановки нажмите Ctrl+C"
echo ""

while true; do
    for value in "${VALUES[@]}"; do
        echo "$(date '+%Y-%m-%d %H:%M:%S') - Запись значения $value в регистр $REGISTER"
        ../utils/modbus_write_32register $REGISTER $value
        
        # Пауза между записями (1 секунда)
        sleep 1
    done
done