#!/bin/bash

echo "setting baud rate..."
stty -F /dev/ttyUSB1 115200 raw
