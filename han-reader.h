/******************************************************************************
 * Processing messages from the HAN port - header file
 *****************************************************************************/

#ifndef _HAN_READER_H_
#define _HAN_READER_H_

#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>


// DATA STRUCTURES FOR STORING THE DIFFERENT MESSAGES

typedef struct {
  int act_pow_pos; /* OBIS Code 1.0.1.7.0.255 - Active Power + (Q1+Q4) */
} Items1;

typedef struct {
  char obis_list_version[1000]; /* OBIS Code 1.1.0.2.129.255 - OBIS List Version Identifier */
  char gs1[1000]; /* OBIS Code 0.0.96.1.0.255 - Meter-ID(GIAI GS1 - 16 digits */
  char meter_model[1000]; /* OBIS Code 0.0.96.1.7 - Meter Type */
  int act_pow_pos; /* Active Power + */
  int act_pow_neg; /* Actve Power - */
  int react_pow_pos; /* Reactive Power + */
  int react_pow_neg; /* Reactive Power - */
  int curr_L1; /* Current Phase L1 */
  int volt_L1; /* Voltage L1 */
} Items9;

typedef struct {
  char obis_list_version[1000]; /* OBIS Code 1.1.0.2.129.255 - OBIS List Version Identifier */
  char gs1[1000]; /* OBIS Code 0.0.96.1.0.255 - Meter-ID(GIAI GS1 - 16 digits */
  char meter_model[1000]; /* OBIS Code 0.0.96.1.7.255 - Meter Type */
  int act_pow_pos;  /* Active Power + */
  int act_pow_neg;  /* Active Power - */
  int react_pow_pos;  /* Reactive Power + */
  int react_pow_neg;  /* Reactive Power - */
  int curr_L1;  /* Current Phase L1 mA */
  int curr_L2;  /* Current Phase L2 mA */
  int curr_L3; /* Current Phase L3 mA */
  int volt_L1; /* Voltage L1 */
  int volt_L2; /* Voltage L2 */
  int volt_L3; /* Voltage L3 */
} Items13;

typedef struct {
  char obis_list_version[1000]; /* OBIS Code 1.1.0.2.129.255 - OBIS List Version Identifier */
  char gs1[1000]; /* OBIS Code 0.0.96.1.0.255 - Meter-ID(GIAI GS1 - 16 digits */
  char meter_model[1000]; /* OBIS Code 0.0.96.1.7.255 - Meter Type */
  int act_pow_pos; /* Active Power + */
  int act_pow_neg; /* Active Power - */
  int react_pow_pos; /* Reactive Power + */
  int react_pow_neg; /* Reactive Power - */
  int curr_L1;
  int volt_L1;
  char date_time[1000]; /* OBIS Code: 0.0.1.0.0.255 - Clock and Date in Meter */
  int act_energy_pos;
  int act_energy_neg;
  int react_energy_pos;
  int react_energy_neg;  
} Items14;

typedef struct {
  char obis_list_version[1000]; /* OBIS Code 1.1.0.2.129.255 - OBIS List Version Identifier */
  char gs1[1000]; /* OBIS Code 0.0.96.1.0.255 - Meter-ID(GIAI GS1 - 16 digits */
  char meter_model[1000]; /* OBIS Code 0.0.96.1.7.255 - Meter Type */
  int act_pow_pos; /* Active Power + */
  int act_pow_neg; /* Active Power - */
  int react_pow_pos; /* Reactive Power + */
  int react_pow_neg; /* Reactive Power - */
  int curr_L1; /* Current L1 */
  int curr_L2; /* Current L2 */
  int curr_L3; /* Current L3 */
  int volt_L1; /* Voltage L1 */
  int volt_L2; /* Voltage L2 */
  int volt_L3; /* Voltage L3 */
  char date_time[1000]; /* OBIS Code: 0.0.1.0.0.255 - Clock and Date in Meter */
  int act_energy_pa; /* Active Energy +A */
  int act_energy_ma; /* Active Energy -A */
  int act_energy_pr; /* Active Energy +R */
  int act_energy_mr; /* Active Energy -R */
} Items18;

typedef struct {
  int year;
  int month;
  int day;
  int hour;
  int min;
  int sec;
  char tm[100];
  int num_items;
  union {
    Items1 msg1;   /* List 1: 1-and-3-phase */
    Items9 msg9;   /* List 2: 1-phase */
    Items13 msg13; /* List 2: 3-phase */
    Items14 msg14; /* List 3: 1-phase */ 
    Items18 msg18; /* List 3: 3-phase */
  };
} HanMsg;


// METHODS

// verify the message - send 0 on success
int verifyMessage(
  unsigned char *buf,  /* the message buffer */
  unsigned int buf_len /* buffer length */
);

// decode the message
int decodeMessage(
  unsigned char *buf, /* the message buffer */
  int buf_len,        /* buffer length */
  HanMsg *msg         /* pointer to the decoded object */
);

// read the raw message (buffer) from a file
int readMessage(
  int read_fd,
  unsigned char *buf, /* the message buffer */
  FILE *rec_f,        /* file to read from */
  int print_hex_msg   /* if nonzero, print the message in hex */
);

// decrypt the message
int decryptMessage(
  uint8_t *msg, /* the message buffer */
  int msg_len,  /* messsage length */
  uint8_t *key  /* decrypt key */
);

// print the message to terminal; optionally store the buffer to a file
int printMessage(
  unsigned char *buf, /* the message buffer */
  int buf_len,        /* buffer length */
  HanMsg *msg,        /* the processed HAN message */
  char *tm_str,       /* if not null, include time info from tm_str */
  FILE *rec_file      /* if not null, the buffer will be saved here */
);


// JSON OUTPUT

// formatting of the JSON messages
typedef struct {
	char indent[10]; /* indent string - usually tab or a couple of spaces */
	char space[5];   /* space between keys and values */
	char eol[5];     /* end of line - typically either enter or nothing */
} JsonFormat;

// format the HAN-message as JSON; returns the message length
int msg_as_json(
	HanMsg *msg,      /* the HAN-message object */
	char *buf,        /* buffer to write to - must be allocated! */
	JsonFormat * fmt, /* JSON format */
	bool raw,         /* whether to output raw (unconverted) values */
	char *tm_str      /* if not null, include time info from tm_str */
);

#endif // _HAN_READER_H_
