#!/bin/bash
# Сервер (на машине с dbus-daemon)
sudo ./build/dbus-proxy \
    --mode server \
    --tcp 0.0.0.0:5555 \
    --dbus-socket /run/dbus/system_bus_socket \
    $*
