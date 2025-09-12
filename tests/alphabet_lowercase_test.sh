#!/bin/bash

for letter in {a..z}; do
    ../utils/modbus_send_letter "$letter"
    sleep 3
done