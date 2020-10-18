# rshutdown

rshutdown is a lightweight remote shutdown HTTP service for Linux.

It is primarily intended for shutting down headless [Raspberry Pi]()s cleanly when SSH access is impractical.



### Installation (Debian-based Linux)

Packages are available for:

- x64 Debian-based Linux (`*-amd64.deb`), e.g. Ubuntu.
- ARM Debian-based Linux (`*-armhf.deb`), e.g. [Raspberry Pi OS](https://www.raspberrypi.org/downloads/) (formerly known as Raspbian) and [DietPi](https://dietpi.com/) for Raspberry Pi.



Download a .deb package file for your architecture from the Releases page.



Install the package using:

```
$ sudo apt install ./rshutdown-<version>-<arch>.deb
```

rshutdown is set up as a service (named `rshutdown`) when installed from one of these packages, and it will automatically start on boot.



To test that the service is working, ping it by sending it any HTTP request on the default HTTP port (3000), e.g.

```
$ curl localhost:3000
{"status": 1}
```



A random shutdown security token – which is needed to trigger the shutdown (see *Usage* section below) – is created when the package is installed. You can read and change its value in `/etc/default/rshutdown`.

**Note** – This file should be readable only by the `rshutdown` user to prevent malevolent shutdowns of the host by users that have read access to the file.

**Note** – After changing the variables in `/etc/default/rshutdown`, restart the `rshutdown` service with `sudo service rshutdown restart` for the changes to take effect.



Logs can be found in `/var/log/rshutdown`.



### Build and run (all flavours of Linux)

Building rshutdown from source requires [CMake](https://cmake.org/).



Clone the Git repository and install its submodules (the [facil.io C web application framework](http://facil.io/), and the [log.c logging library](https://github.com/rxi/log.c)):

```
$ git clone --recurse-submodules https://github.com/spujadas/rshutdown.git
```



Set up the make process and compile with:

```
$ mkdir build && cd build
$ cmake ..
$ make -j4
```

**Note** – For testing purposes, you can compile rshutdown with the `-DNO_SHUTDOWN` option. When it receives an HTTP POST request with the shutdown security token, rshutdown will display a message but will not actually shut down the host.

**Note** – This process may require setting up a cross-compilation toolchain if compiling for another target OS, e.g. https://github.com/abhiTronix/raspberry-pi-cross-compilers for Raspberry Pi, and https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html#cross-compiling for guidance on how to set it up with CMake.



Optionally install the `rshutdown` binary to `/usr/bin` with:

```
$ sudo make install
```



Now define a shutdown security token by setting the `RSHUTDOWN_TOKEN` environment variable, e.g.

```
$ export RSHUTDOWN_TOKEN=lIRsbdfVYnWbWfNCtZxw8GFs6PLBJ6zU
```

**Note** – The token should remain a secret, don't use this specific value.



Optionally set the `RSHUTDOWN_HTTP_PORT` environment variable if you want to run the service on a specific port (by default: 3000), e.g.

```
$ export RSHUTDOWN_HTTP_PORT=3001
```



Then start rshutdown with a user that has the permission to run `/sbin/shutdown` as `root`:

```
$ ./rshutdown
```



For ideas on how to set up rshutdown as a service, have a look at the pre- and post-install scripts and supporting files in the `packaging` directory of the project.



### Usage

To shutdown a host that is running rshutdown, send it an HTTP POST request on the port that rshutdown is listening to, with the following JSON object:

```
{
	"token": "YOUR_SECURITY_TOKEN_HERE"
}
```

For example, here is how you would do this in a Windows Command Prompt using [cURL](https://curl.haxx.se/):

```
> curl -X POST -d "{\"token\": \"YOUR_SECURITY_TOKEN_HERE\"}" http://HOSTNAME_OR_IP:3000
```

Here's the equivalent command on Linux:

```
$ curl -X POST -d '{"token": "YOUR_SECURITY_TOKEN_HERE"}' http://HOSTNAME_OR_IP:3000
```



If the token is valid, then the response will be JSON object `{"status": 0}`.

**Note** – If the request is invalid, then the `status` field will have another value: `1` for non-POST requests, and `-1` for invalid POST requests (e.g. containing the wrong security token).



### Alternatives

Hardware-based alternatives to trigger the `shutdown` command include:

- [Using a USB device](https://hackernoon.com/shut-down-your-raspberry-pi-the-easy-way-9c7be0b4fff8).
- Using a [piece of wire](https://www.instructables.com/Simple-Raspberry-Pi-Shutdown-Button/) or a [button](https://core-electronics.com.au/tutorials/how-to-make-a-safe-shutdown-button-for-raspberry-pi.html) with two GPIO pins.



### About

Written by [Sébastien Pujadas](https://pujadas.net/), released under the [MIT license](https://github.com/spujadas/departure-python/blob/master/LICENSE).





