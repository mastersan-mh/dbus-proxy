#!/bin/bash

# Клиент (на машине с приложением)
./build/dbus-proxy \
    --mode client \
    --tcp 127.0.0.1:5555 \
    --listen-socket /tmp/dbus-proxy.sock \
    $*
