#!/bin/bash

# Клиент (на машине с приложением)
./build/dbus-proxy \
    --mode client \
    --addr 127.0.0.1 \
    --port 5555 \
    --dbus-socket /tmp/dbus-proxy.sock \
    $*
