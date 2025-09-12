#!/bin/bash

for number in {0..9}; do
    ../utils/modbus_send_letter "$number"
    sleep 3
done