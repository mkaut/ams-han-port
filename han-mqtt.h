/******************************************************************************
 * Sending a HAN-message via MQTT - header file
 *****************************************************************************/

#ifndef _HAN_MQTT_H_
#define _HAN_MQTT_H_

#include "han-reader.h"

#include <mosquitto.h>


// send the HAN-message via MQTT
int sendMqttMessage(
  HanMsg *msg,             /* the message to send */
  char *tm_str,            /* if not null, include time info from tm_str */
  struct mosquitto * mosq, /* the MQTT sender */
  char *root               /* MQTT root topic to send to */
);

#endif // _HAN_MQTT_H_
