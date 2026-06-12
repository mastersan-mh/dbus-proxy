#!/bin/bash

export DBUS_SESSION_BUS_ADDRESS=unix:path=/tmp/dbus-proxy1.sock
busctl --user list
