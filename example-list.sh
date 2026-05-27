#!/bin/bash

# Приложение подключается к /tmp/dbus-proxy.sock как к обычной шине
export DBUS_SESSION_BUS_ADDRESS=unix:path=/tmp/dbus-proxy.sock
busctl --user list
