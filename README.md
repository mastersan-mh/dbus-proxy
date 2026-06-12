# dbus-proxy

## What is dbus-proxy?

dbus-proxy is an simple open source client/server application
to proxy DBUS messages via TCP tunnel.

### How To Compile dbus-proxy

In order to compile this project just do

* ./build-release.sh

to build release version of binary, or

* ./build-debug.sh

to build debug version.

### Options

#### --dbus-socket BUSSES

Set busses connections.

Up to 256 channels supported: from 0 to 255.

Has different meaning on server and cliend side.

On a server side mean busses to connect to.

On a client side mean busses to create and linten to client application.

Client should use only enumerated server channels id,
otherwise stream will be rejected silently.

On sever:
```
<channel-id>:<bus>
0:/tmp/sv-bus0
1:/tmp/sv-bus1
2:/tmp/sv-bus2
3:/tmp/sv-bus2
```

On client:
```
1:/tmp/cl-bus0
2:/tmp/cl-bus1
6:/tmp/cl-bus2
```
If user will write into `/tmp/cl-bus0`,
it means that user writes into channel 1,
and on the server side it will passed to `/tmp/sv-bus1`.

But, if client will try to write into the channel 6 (bus `/tmp/cl-bus2`),
server will rejects such request on server side silently.

On client side we can register not all channels.

_Example_:

1.
```
dbus-proxy \
    --mode server \
    --addr 0.0.0.0 \
    --port 5555 \
    --dbus-socket "0:/run/dbus/system_bus_socket"
```

```
dbus-proxy \
    --mode client \
    --addr 192.168.56.100 \
    --port 5555 \
    --dbus-socket "0:/path/to/bus/0"
```

Bus `/path/to/bus/0` will proxy to bus `0:/run/dbus/system_bus_socket`.

2.
```
dbus-proxy \
    --mode server \
    --addr 0.0.0.0 \
    --port 5555 \
    --dbus-socket "1:/tmp/bus1;2:/tmp/bus2;3:/tmp/bus3"
```

```
dbus-proxy \
    --mode client \
    --addr 192.168.56.100 \
    --port 5555 \
    --dbus-socket "1:/tmp/listen1;3:/tmp/listen3;4:/tmp/listen4;"
```

* Bus `/tmp/listen1` will proxy to bus `/tmp/bus1` (channel 1)
* Bus `/tmp/listen3` will proxy to bus `/tmp/bus3` (channel 3)
* Bus `/tmp/listen4` (channel 4) stream will be dropped silently on a server-side.
* Bus `/tmp/bus2` (channel2) on server side wont be used by this client.
