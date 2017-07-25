#!/bin/bash
D=$(cd $(dirname $0); pwd)
O=$D/../../openocd-code
F=$1
$O/src/openocd -s $O/tcl -f $D/nrf52.cfg -c "program $F"
