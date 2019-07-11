/******************************************************************************
 * Sending a HAN-message via MQTT - implementation file
 *****************************************************************************/

#include "han-mqtt.h"
#include "han-reader.h"

//#include <time.h>
#include <string.h>
#include <assert.h>


// HELPER FUNCTIONS FOR THE MQTT SENDER

// Get an MQTT list number, based on message type
int list_number(HanMsg * msg) {
	switch (msg->num_items) {
	case 1:
		return 1;
	case 9:
	case 13:
		return 2;
	case 14:
	case 18:
		return 3;
	}
	return -1;  // should never happen
}


/**********************************************************
 * FUNCTION: sendMqttMessage()
 *********************************************************/
int sendMqttMessage(
		 HanMsg *msg,             /* the message to send */
		 char *tm_str,            /* if not null, include time info from tm_str */
		 struct mosquitto * mosq, /* the MQTT sender */
		 char *root)              /* MQTT root topic to send to */
{
	// TODO: check connection, reconnect if needed!

	/* compact JSON formatting for sending */
	JsonFormat compactJson;
	strcpy(compactJson.indent, "");
	strcpy(compactJson.space, "");
	strcpy(compactJson.eol, "");

	char mqttMsg[1024];
	int msgLen = msg_as_json(
		msg,
		mqttMsg,
		&compactJson,
		false, /* convert values to standard units */
		tm_str
	);
	assert(msgLen == strlen(mqttMsg) && "checking message length");

	int hanList = list_number(msg);

	char topic[512];
	sprintf(topic, "%s/list_%d", root, hanList);

	int qos = 0;
	mosquitto_publish(mosq, NULL, topic, msgLen, mqttMsg, qos, false);

	// TODO: does mosquitto_publish return something we could return?
	return msgLen;
}
