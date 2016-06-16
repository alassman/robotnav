#!/bin/bash

echo "setting baud rate..."
stty -F /dev/ttyUSB0 115200 raw
