# ams-han-port
Read HAN port on Norwegian AMS Smart Meters

This is a fork of https://github.com/per-erik-nordbo/ams-han-port, with added MQTT functionality.
It is in a very early stage, but the MQTT sending works. Tested on a Raspberry Pi.

## Main changes to the parent project:

### New files

- `han-reader.c` reimplements the HAN-reading functionality from `read.c`, in a more reusable way
	- in the long run, the original serial-output code could be rewritten to use this
- `han-mqtt.c` implements the MQTT functionality
- `han2mqtt.c` is the main driver
	- currently a bare minimum, no command-line options etc.

### Future possibilities

There are two main options:

- keep `han2mqtt` as a separate binary, with only MQTT functionality
- add the MQTT with the original binary, as one of possible outputs
	- note that this option would imply dependence on a MQTT library for all users

Independent on this, the MQTT part needs the following:

- some way of controlling the parameters: either command-line parameters, or a config file
- possibly a more "standard" format of the MQTT messages, for example using the Homie format


## Dependencies

The code requires Mosquitto MQTT library.
In Debian, this can be installed using `apt install libmosquitto-dev`.


## Building `han2mqtt`

- Using the Makefile
	1. edit the `SERIAL_PORT` and `MQTT_SERVER` values in the `han2mqtt` line in the Makefile
	2. run `make han2mqtt`

- Direct gcc command:
	- run `gcc -Wall -Wextra -O2 -D SERIAL_PORT='"<SERIAL_PORT>"' -D MQTT_SERVER='"<MQTT_SERVER>"' han2mqtt.c han-mqtt.c han-reader.c rs232.c aes.c -lmosquitto -lm -o han2mqtt`
	- on Raspberry Pi with UART adapter, the SERIAL_PORT tends to be `/dev/ttyUSB0`
	- at the moment, the MQTT broker port is hard-coded in `han2mqtt.c`


## Make it a services that starts on boot

This assumes a linux system with systemd (almost any modern linux).

- As a root (sudo), create `/etc/systemd/system/ams-han.service` with the following content:

	```
	[Unit]
	Description=ams-han
	After=network.target
	
	[Service]
	ExecStart=/opt/ams-han/han2mqtt
	WorkingDirectory=/opt/ams-han
	StandardOutput=inherit
	StandardError=inherit
	Restart=always
	User=pi
	
	[Install]
	WantedBy=multi-user.target
	```

	- replace `/opt/ams-han` by the path to the code and `pi` by your user name
	- make sure that the user has access to the folder

- Run `sudo systemctl daemon-reload` to refresh the service list
- Run `sudo systemctl start ams-han.service`
- Run `sudo systemctl status ams-han.service` to check everything is OK
- If everything looks OK, enable the service: `sudo systemctl enable ams-han.service`

At any later point, you can see the current output using `journalctl -u ams-han -f`
