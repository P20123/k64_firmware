#!/usr/bin/env bash
WHERE=$(dirname $0)
source $WHERE/openocd_server.sh
start_oocd
(echo "init;kinetis mdm mass_erase 0;reset halt;flash write_image build/main.bin 0 bin;reset run;"; sleep 1; echo "exit") | telnet localhost 4444

