#!/bin/bash

echo "setting baud rate..."
stty -F /dev/ttyUSB0 4800 raw
