#!/bin/bash
# Сервер (на машине с dbus-daemon)
sudo ./build/dbus-proxy \
    --mode server \
    --addr 0.0.0.0 \
    --port 5555 \
    --dbus-socket \
"\
0:/run/dbus/system_bus_socket;\
" \
    $*
