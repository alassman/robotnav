#!/bin/bash

echo "setting baud rate..."
stty -F /dev/ttyUSB0 9600 raw
