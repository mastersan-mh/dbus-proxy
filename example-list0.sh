#!/bin/bash

export DBUS_SESSION_BUS_ADDRESS=unix:path=/tmp/dbus-proxy0.sock
busctl --user list
