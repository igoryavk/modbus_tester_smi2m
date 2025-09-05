#!/bin/bash

# Скрипт для записи значений в Modbus регистр 4101
# Записывает значения от 0 до 100 с интервалом 0.5 секунды

MODBUS_WRITER="../utils/modbus_write_register"

for ((value=0; value<=100; value++))
do
    ${MODBUS_WRITER} 4101 $value
    sleep 0.5
done