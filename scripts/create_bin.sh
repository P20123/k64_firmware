#!/bin/sh
arm-none-eabi-objcopy -O binary -R debug_strs $1 $2
