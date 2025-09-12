#!/bin/bash

for letter in {A..Z}; do
    ../utils/modbus_send_letter "$letter"
    sleep 3
done