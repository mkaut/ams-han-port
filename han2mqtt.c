/******************************************************************************
 * Dummy driver - only for testing
 *****************************************************************************/

#include "han-reader.h"
#include "han-mqtt.h"
#include "rs232.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <mosquitto.h>

int main()
{
	int debug = 1;
	bool print_msg = true;  // whether to print the message to stdout

	/******************************
	 * data for the HAN connection
	 * ****************************/
	#ifndef SERIAL_PORT
		char serial_device[100] = "/dev/ttyUSB0";
	#else
		char serial_device[100] = SERIAL_PORT;
	#endif

	int read_serial = 1;
	int print_hex = 0;
	int decrypt = 0;
	int read_fd = 0;
	int bdrate = 2400; /* 2400 baud */
	char mode[] = {
		'8', /* bits in byte */
		'E', /* parity - E(ven), O(dd) or N(one) */
		'1', /* num stop bits */
		 0}; /* end of string */
	uint8_t key[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
	                   0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

	/* compact JSON formatting for sending */
	JsonFormat compactJson;
	strcpy(compactJson.indent, "");
	strcpy(compactJson.space, "");
	strcpy(compactJson.eol, "");

	/* print-friendly JSON formatting */
	JsonFormat prettyJson;
	strcpy(prettyJson.indent, "  ");
	strcpy(prettyJson.space, " ");
	strcpy(prettyJson.eol, "\n");

	/***************************
	 * data for MQTT connection
	 * *************************/
	// TODO: input this either via command-line or a config file (json parsed with json-c?)
	#ifndef MQTT_SERVER
		char mqttServerAddress[] = "localhost";
	#else
		char mqttServerAddress[] = MQTT_SERVER;
	#endif
	int mqttServerPort = 1883;
	char mqttRootTopic[] = "ams-han";


	HanMsg msg;
	struct mosquitto * mosq;
	unsigned char buf[4096];
	int buf_len;


	// connect to the serial port
	// TODO: implement the same input as in read.c?
	if (read_serial) {
		if ((read_fd = open_serial(serial_device, bdrate, mode)) == -1) {
			printf("Can not open rs232 port %s\n", serial_device);
			return -1;
		}
		else {
			if (debug) printf("open_serial OK: %1d\n", read_fd);
		}
	}
	else {
		fprintf(stderr, "The MQTT sender implements only reading from the serial port\n");
		exit(1);
	}

	// initialize MQTT connection
	mosquitto_lib_init();
	mosq = mosquitto_new(NULL, true, NULL);
	if (!mosq) {
		fprintf(stderr, "Error: Could not allocate the mosquitto object.\n");
		exit(1);
	}
	if (mosquitto_connect(mosq, mqttServerAddress, mqttServerPort, 60) > 0) {
		fprintf(stderr, "Unable to connect.\n");
		exit(1);
	}

	while (true) {
		if ((buf_len = readMessage(read_fd, buf, NULL, print_hex)) <= 0) {
			break;
		}

		if (verifyMessage(buf, buf_len)) {
			printf("verify failed\n");
			continue;
		}

		if (decrypt) {
			decryptMessage(buf, buf_len, key);
		}

		char *tm_str = NULL;

		if (true) { /* was: if (read_serial) { */
			struct timespec spec;
			clock_gettime(CLOCK_REALTIME, &spec); /* OR: CLOCK_MONOTONIC */
			time_t s  = spec.tv_sec;
			long ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds

			char tmstr64[100];
			char ms_str[10];
			strftime(tmstr64, 20, "%Y-%m-%d %H:%M:%S", localtime(&s));
			sprintf(ms_str, "%.ld", ms);
			strcat(tmstr64, ms_str);
			// TODO: the below line overwrites tmstr64, so both tmstr64 and ms_str from above are lost!
			sprintf(tmstr64, "%ld.%03ld", s, ms);

			tm_str = (char*) &tmstr64;
		}

		if (decodeMessage(buf, buf_len, &msg) != 0) {
			if (debug) printf("decode failed\n");
			fprintf(stderr, "%s ERROR: Message decode failed.\n", tm_str);
			continue;
		}

		if (print_msg) {
			printMessage(buf, buf_len, &msg, tm_str, NULL);
		}

		if (debug) printf("publishing to MQTT .. ");
		int res = sendMqttMessage(&msg, tm_str, mosq, mqttRootTopic);
		if (res == MOSQ_ERR_SUCCESS) {
			printf("OK\n");
		}
		else {
			switch(res) {
				case MOSQ_ERR_NO_CONN:
					printf("FAILED: no connection to the broker\n");
					break;
				default:
					printf("FAILED with error code %d", res);
			}
		}
		printf("\n");
		fflush(stdout);
	}

	return 0;
}
