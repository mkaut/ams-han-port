/******************************************************************************
 * Processing messages from the HAN port - implementation file
 *****************************************************************************/

#include "han-reader.h"

#include "rs232.h"
#include "aes.h"

#include <stdlib.h>
#include <time.h>


/**********************************************************
 * FUNCTION: verifyMessage()
 *********************************************************/
int verifyMessage(unsigned char *buf,
		  unsigned int buf_len)
{
  /* warning killers */
  buf = buf;
  buf_len = buf_len;
  
  return 0;
}


/*
7e                                                     : Flag (0x7e)
a0 87                                                  : Frame Format Field
01 02                                                  : Source Address
01                                                     : Destination Address
10                                                     : Control Field = R R R P/F S S S 0 (I Frame)
9e 6d                                                  : HCS
e6 e7 00                                               : DLMS/COSEM LLC Addresses
0f 40 00 00 00                                         : DLMS HEADER?
09 0c 07 d0 01 03 01 0e 00 0c ff 80 00 03              : Information
02 0e                                                  : Information
09 07 4b 46 4d 5f 30 30 31                             : Information
09 10 36 39 37 30 36 33 31 34 30 30 30 30 30 39 35 30  : Information
09 08 4d 41 31 30 35 48 32 45                          : Information
06 00 00 00 00                                         : Information
06 00 00 00 00                                         : Information
06 00 00 00 00                                         : Information
06 00 00 00 00                                         : Information
06 00 00 00 0e                                         : Information
06 00 00 09 01                                         : Information
09 0c 07 d0 01 03 01 0e 00 0c ff 80 00 03              : Information
06 00 00 00 00                                         : Information
06 00 00 00 00                                         : Information
06 00 00 00 00                                         : Information
06 00 00 00 00                                         : Information
97 35                                                  : FCS
7e                                                     : Flag
*/


/**********************************************************
 * FUNCTION: decodeMessage()
 *********************************************************/
int decodeMessage(unsigned char *buf,
		  int buf_len,
		  HanMsg *msg)
{
  /* warning killers */
  buf_len = buf_len;
  
  memset(msg, 0, sizeof(HanMsg)); /* clear/init data */
  
  if (buf[17] == 0x09) {

    msg->year = buf[19]<<8 | buf[20];
    msg->month = buf[21];
    msg->day = buf[22];
    msg->hour = buf[24];
    msg->min = buf[25];
    msg->sec = buf[26];
    
    sprintf(msg->tm, "%d-%02d-%02d %02d:%02d:%02d", msg->year, msg->month, msg->day, msg->hour, msg->min, msg->sec);

    int offset = 17 + 2 + buf[18];
    if (buf[offset]== 0x02) {
      msg->num_items = buf[offset+1];
    }
    offset+=2;
    if (msg->num_items == 1) { /* Num Records: 1 */
      if (buf[offset]==0x06) { /* Item 1 */
	msg->msg1.act_pow_pos = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
    }
    else if (msg->num_items==9) { /* Num records: 9 */
      unsigned int num_bytes=0;
      if (buf[offset]==0x09) { /* Item 1 */
	num_bytes = buf[offset+1];
	strncpy(msg->msg9.obis_list_version, (const char *) buf+offset+2, num_bytes);
	msg->msg9.obis_list_version[num_bytes] = '\0';
      }
      offset+=2+num_bytes;
      if (buf[offset]==0x09) { /* Item 2 */
	num_bytes = buf[offset+1];
	strncpy(msg->msg9.gs1, (const char *) buf+offset+2, num_bytes);
	msg->msg9.gs1[num_bytes] = '\0';
      }
      offset+=2+num_bytes;
      if (buf[offset]==0x09) { /* Item 3 */
	num_bytes = buf[offset+1];
	strncpy(msg->msg9.meter_model, (const char *) buf+offset+2, num_bytes);
	msg->msg9.meter_model[num_bytes] = '\0';
      }
      offset+=2+num_bytes;
      if (buf[offset]==0x06) { /* Item 4 */
	msg->msg9.act_pow_pos = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 5 */
	msg->msg9.act_pow_neg = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 6 */
	msg->msg9.react_pow_pos = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 7 */
	msg->msg9.react_pow_neg = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 8 */
	msg->msg9.curr_L1 = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 9 */
	msg->msg9.volt_L1 = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
    }
    else if (msg->num_items==13) { /* Num records: 13 */
      unsigned int num_bytes = 0;
      if (buf[offset]==0x09) { /* Item 1 */
	num_bytes = buf[offset+1];
	strncpy(msg->msg13.obis_list_version, (const char *) buf+offset+2, num_bytes);
	msg->msg13.obis_list_version[num_bytes] = '\0';
      }
      offset+=2+num_bytes;
      if (buf[offset]==0x09) { /* Item 2 */
	num_bytes = buf[offset+1];
	strncpy(msg->msg13.gs1, (const char *) buf+offset+2, num_bytes);
	msg->msg13.gs1[num_bytes] = '\0';
      }
      offset+=2+num_bytes;
      if (buf[offset]==0x09) { /* Item 3 */
	num_bytes = buf[offset+1];
	strncpy(msg->msg13.meter_model, (const char *) buf+offset+2, num_bytes);
	msg->msg13.meter_model[num_bytes] = '\0';
      }
      offset+=2+num_bytes;
      if (buf[offset]==0x06) { /* Item 4 */
	msg->msg13.act_pow_pos = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 5 */
	msg->msg13.act_pow_neg = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 6 */
	msg->msg13.react_pow_pos = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 7 */
	msg->msg13.react_pow_neg = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 8 */
	msg->msg13.curr_L1 = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 9 */
	msg->msg13.curr_L2 = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 10 */
	msg->msg13.curr_L3 = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 11 */
	msg->msg13.volt_L1 = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 12 */
	msg->msg13.volt_L2 = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 13 */
	msg->msg13.volt_L3 = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
    }
    else if (msg->num_items==14) { /* Num records: 14 */
      unsigned int num_bytes = 0;
      if (buf[offset]==0x09) { /* Item 1 */
	num_bytes = buf[offset+1];
	strncpy(msg->msg14.obis_list_version, (const char *) buf+offset+2, num_bytes);
	msg->msg14.obis_list_version[num_bytes] = '\0';
      }
      offset+=2+num_bytes;
      if (buf[offset]==0x09) { /* Item 2 */
	num_bytes = buf[offset+1];
	strncpy(msg->msg14.gs1, (const char *) buf+offset+2, num_bytes);
	msg->msg14.gs1[num_bytes] = '\0';
      }
      offset+=2+num_bytes;
      if (buf[offset]==0x09) { /* Item 3 */
	num_bytes = buf[offset+1];
	strncpy(msg->msg14.meter_model, (const char *) buf+offset+2, num_bytes);
	msg->msg14.meter_model[num_bytes] = '\0';
      }
      offset+=2+num_bytes;
      if (buf[offset]==0x06) { /* Item 4 */
	msg->msg14.act_pow_pos = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 5 */
	msg->msg14.act_pow_neg = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 6 */
	msg->msg14.react_pow_pos = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 7 */
	msg->msg14.react_pow_neg = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 8 */
	msg->msg14.curr_L1 = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 9 */
	msg->msg14.volt_L1 = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }      
      offset+=1+4;
      if (buf[offset]==0x09) { /* Item 10 */
	num_bytes = buf[offset+1];

	int year = buf[offset+2]<<8 | buf[offset+3];
	int month = buf[offset+4];
	int day = buf[offset+5];
	int hour = buf[offset+7];
	int min = buf[offset+8];
	int sec = buf[offset+9];
    
	sprintf(msg->msg14.date_time, "%d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, min, sec);

      }
      offset+=2+num_bytes;
      if (buf[offset]==0x06) { /* Item 11 */
	msg->msg14.act_energy_pos = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 12 */
	msg->msg14.act_energy_neg = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }      
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 13 */
	msg->msg14.react_energy_pos = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }      
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 14 */
	msg->msg14.react_energy_neg = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }      
    }
    else if (msg->num_items==18) { /* Num records: 18 */
      unsigned int num_bytes = 0;
      if (buf[offset]==0x09) { /* Item 1 */
	num_bytes = buf[offset+1];
	strncpy(msg->msg18.obis_list_version, (const char *) buf+offset+2, num_bytes);
	msg->msg18.obis_list_version[num_bytes] = '\0';
      }
      offset+=2+num_bytes;
      if (buf[offset]==0x09) { /* Item 2 */
	num_bytes = buf[offset+1];
	strncpy(msg->msg18.gs1, (const char *) buf+offset+2, num_bytes);
	msg->msg18.gs1[num_bytes] = '\0';
      }
      offset+=2+num_bytes;
      if (buf[offset]==0x09) { /* Item 3 */
	num_bytes = buf[offset+1];
	strncpy(msg->msg18.meter_model, (const char *) buf+offset+2, num_bytes);
	msg->msg18.meter_model[num_bytes] = '\0';
      }
      offset+=2+num_bytes;
      if (buf[offset]==0x06) { /* Item 4 */
	msg->msg18.act_pow_pos = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 5 */
	msg->msg18.act_pow_neg = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 6 */
	msg->msg18.react_pow_pos = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 7 */
	msg->msg18.react_pow_neg = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 8 */
	msg->msg18.curr_L1 = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 9 */
	msg->msg18.curr_L2 = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }      
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 10 */
	msg->msg18.curr_L3 = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }      
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 11 */
	msg->msg18.volt_L1 = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }      
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 12 */
	msg->msg18.volt_L2 = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }      
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 13 */
	msg->msg18.volt_L3 = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }      
      offset+=1+4;
      if (buf[offset]==0x09) { /* Item 14 */
	num_bytes = buf[offset+1];

	int year = buf[offset+2]<<8 | buf[offset+3];
	int month = buf[offset+4];
	int day = buf[offset+5];
	int hour = buf[offset+7];
	int min = buf[offset+8];
	int sec = buf[offset+9];
    
	sprintf(msg->msg18.date_time, "%d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, min, sec);

      }
      offset+=2+num_bytes;
      if (buf[offset]==0x06) { /* Item 15 */
	msg->msg18.act_energy_pa = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 16 */
	msg->msg18.act_energy_ma = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }      
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 17 */
	msg->msg18.act_energy_pr = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }      
      offset+=1+4;
      if (buf[offset]==0x06) { /* Item 18 */
	msg->msg18.act_energy_mr = buf[offset+1]<<24 | buf[offset+2]<<16 | buf[offset+3]<<8 | buf[offset+4];
      }      
    }
    else {
      return 1; /* Error - Unknown msg size */
    }
    return 0; /* Message OK */
  }
  
  return 1; /* Error - Unknown msg type */
}


/**********************************************************
 * FUNCTION: my_read()
 *********************************************************/
int my_read(int fd,
	    unsigned char *buf,
	    int len)
{
  int n;
  
  while (1) {
    n = read(fd, buf, len);

    if (n>=0) {
      break;
    }
    if (errno == EAGAIN) {
      usleep(1000);
    }
  }
  
  return n;
}

/**********************************************************
 * FUNCTION: readMessage()
 *********************************************************/
int readMessage(int read_fd,
		unsigned char *buf,
		FILE *rec_f,
		int print_hex_msg)
{
  int n;
  int debug;
  int length;

  debug=0;

  if (rec_f) {
    while(0) { /* dump data and exit */
      static int count=0;
      /* next byte */
      n = my_read(read_fd, buf, 4096);
      if (n>0) {
	fprintf(stderr, "*");
	fwrite(buf, n, 1, rec_f);
	fflush(rec_f);
      }
      count+=n;
      if (count>500) {
	fclose(rec_f);
	exit(0);
      }
    }
  }
    
  debug=0;
  while(1) { /* find start of header */
    /* next byte */
    n = my_read(read_fd, buf, 1);
    if (n==-1) {
      printf("read()==-1: errno: %s\n", strerror(errno));
    }
    if (n<=0) return 0; /* EOF or ERROR */

    if (debug) { printf(" A(%02x)", buf[0]); fflush(stdout);}

    if (buf[0] == 0x7e) { /* good, frame start delimiter candidate */
      /* next byte */
      n = my_read(read_fd, buf+1, 1);
      if (debug) {printf(" B(%02x)", buf[1]); fflush(stdout);};
      if (n<=0) return 0; /* EOF or ERROR */

      
      /* 2 Byte Frame Format Field
	 16 BITS: "TTTTSLLLLLLLLLLL"
	 - T=Type bits: TTTT = 0101 (0xa0) = Type 3
	 - S=Segmentation=0 (Segment = 1)
	 - L=11 Length Bits
      */
      if ((buf[1] & 0xa0) == 0xa0) { /* 16 bit frame format field = "0101SLLLLLLLLLLL" - Type 3 = 0101 (0xa0), S=Segmentation, L=Length bits */
	n = my_read(read_fd, buf+2, 1);
	if (debug) {printf(" C(%02x)", buf[2]); fflush(stdout);}
	if (n<=0) return 0; /* EOF or ERROR */
	length = ((buf[1] & 0x07) << 8) + buf[2]; /* length is 11 bits */
	break;
      }
    }
  }

  /* read rest of message */
  int bytes_read = 3;
  int frame_delimiters = 2; /* start byte 0x7e + end byte 0x7e*/
  int bytes_left = length + frame_delimiters - bytes_read;
  
  while (bytes_left > 0) {
    n = my_read(read_fd, buf+bytes_read, bytes_left);
    if (debug) { printf(" M()"); fflush(stdout);}
    if (n<=0) return 0; /* EOF or ERROR */

    bytes_read += n;
    bytes_left -= n;
  }

  if (print_hex_msg) {
    int i;
    for (i=0; i<bytes_read; i++) {
      printf("%02x ", buf[i]);
    }
    printf("\n");
  }
  
  return(bytes_read);
}


/************************************************************************
 * FUNCTION: decryptMessage()
 ***********************************************************************/
int decryptMessage(uint8_t *msg,
		   int msg_len,
		   uint8_t *key)
{
  int debug=0;
  int first = 12;
  int last = msg_len - 3;

  int offset=first;

  uint8_t decrypted[16]; 
  
  while (offset<last)  {
    if (debug) printf("### remains: %1d\n", last-offset);
    AES128_ECB_decrypt(msg+offset, key, decrypted);
    memcpy(msg+offset, decrypted, 16);
    offset += 16;
  }
  
  return 0;
}


/**********************************************************
 * FUNCTION: printMessage()
 *********************************************************/
int printMessage(unsigned char *buf,
		 int buf_len,
		 HanMsg *msg,
		 char *tm_str,
		 FILE *rec_file)
{
  int i;
  int debug_hex = 0;
  
  for(i=0; i<buf_len; i++) {
    if (i>0) {
      if (debug_hex) printf(" ");
    }
    if (debug_hex) printf("%02x", buf[i]);
    if (rec_file) fputc(buf[i], rec_file);
  }
  
  if (rec_file) fflush(rec_file);

  printf("{\"Date_Time\":\"%s\",\n", msg->tm);
  if (tm_str) {
    int year, month, day;
    int hour, min, sec;
    
    /* FORMAT = "2017-06-07 22:25:30" */
    if (sscanf(msg->tm, "%4d-%2d-%2d %2d:%2d:%2d",
	       &year, &month, &day,
	       &hour, &min, &sec) == 6) {

      struct tm info;
      time_t tm_sec;
      
      info.tm_year = year - 1900;
      info.tm_mon = month - 1;
      info.tm_mday = day;
      info.tm_hour = hour;
      info.tm_min = min;
      info.tm_sec = sec;
      info.tm_isdst = -1;
      
      tm_sec = mktime(&info);
      
      printf("\"Meter_Time\":%1ld,\n", tm_sec);
    }
    printf("\"Host_Time\":%s,\n", tm_str);
  }

  switch (msg->num_items) {
  case 1:
    printf("\"Act_Pow_P_Q1_Q4\":%1d}\n", msg->msg1.act_pow_pos);
    break;
  case 9:
    printf("\"OBIS_List_Version\":\"%s\",\n", msg->msg9.obis_list_version);
    printf("\"GS1\":\"%s\",\n", msg->msg9.gs1);
    printf("\"Meter_Model\":\"%s\",\n", msg->msg9.meter_model);
    printf("\"Act_Pow_P_Q1_Q4\":%1d,\n", msg->msg9.act_pow_pos);
    printf("\"Act_Pow_M_Q2_Q3\":%1d,\n", msg->msg9.act_pow_neg);
    printf("\"React_Pow_P\":%1d,\n", msg->msg9.react_pow_pos);
    printf("\"React_Pow_M\":%1d,\n", msg->msg9.react_pow_neg);
    printf("\"Curr_L1\":%1d,\n", msg->msg9.curr_L1);
    printf("\"Volt_L1\":%1d}\n", msg->msg9.volt_L1);
    break;
  case 13:
    printf("\"OBIS_List_Version\":\"%s\",\n", msg->msg13.obis_list_version);
    printf("\"GS1\":\"%s\",\n", msg->msg13.gs1);
    printf("\"Meter_Model\":\"%s\",\n", msg->msg13.meter_model);
    printf("\"Act_Pow_P_Q1_Q4\":%1d,\n", msg->msg13.act_pow_pos);
    printf("\"Act_Pow_M_Q2_Q3\":%1d,\n", msg->msg13.act_pow_neg);
    printf("\"React_Pow_P_Q1_Q2\":%1d,\n", msg->msg13.react_pow_pos);
    printf("\"React_Pow_M_Q3_Q4\":%1d,\n", msg->msg13.react_pow_neg);
    printf("\"Curr_L1\":%1d,\n", msg->msg13.curr_L1);
    printf("\"Curr_L2\":%1d,\n", msg->msg13.curr_L2);
    printf("\"Curr_L3\":%1d,\n", msg->msg13.curr_L3);
    printf("\"Volt_L1\":%1d,\n", msg->msg13.volt_L1);
    printf("\"Volt_L2\":%1d,\n", msg->msg13.volt_L2);
    printf("\"Volt_L3\":%1d}\n", msg->msg13.volt_L3);
    break;
  case 14:
    printf("\"OBIS_List_Version\":\"%s\",\n", msg->msg14.obis_list_version);
    printf("\"GS1\":\"%s\",\n", msg->msg14.gs1);
    printf("\"Meter_Model\":\"%s\",\n", msg->msg14.meter_model);
    printf("\"Act_Pow_P_Q1_Q4\":%1d,\n", msg->msg14.act_pow_pos);
    printf("\"Act_Pow_M_Q2_Q3\":%1d,\n", msg->msg14.act_pow_neg);
    printf("\"React_Pow_P_Q1_Q2\":%1d,\n", msg->msg14.react_pow_pos);
    printf("\"React_Pow_M_Q3_Q4\":%1d,\n", msg->msg14.react_pow_neg);
    printf("\"Curr_L1\":%1d,\n", msg->msg14.curr_L1);
    printf("\"Volt_L1\":%1d,\n", msg->msg14.volt_L1);
    printf("\"Date_Time2\":\"%s\",\n", msg->msg14.date_time);
    printf("\"Act_Energy_P\":%1d,\n", msg->msg14.act_energy_pos);
    printf("\"Act_Energy_M\":%1d,\n", msg->msg14.act_energy_neg);
    printf("\"React_Energy_P\":%1d,\n", msg->msg14.react_energy_pos);
    printf("\"React_Energy_M\":%1d}\n", msg->msg14.react_energy_neg);
    break;
  case 18:
    printf("\"OBIS_List_Version\":\"%s\",\n", msg->msg18.obis_list_version);
    printf("\"GS1\":\"%s\",\n", msg->msg18.gs1);
    printf("\"Meter_Model\":\"%s\",\n", msg->msg18.meter_model);
    printf("\"Act_Pow_P_Q1_Q4\":%1d,\n", msg->msg18.act_pow_pos);
    printf("\"Act_Pow_M_Q2_Q3\":%1d,\n", msg->msg18.act_pow_neg);
    printf("\"React_Pow_P_Q1_Q2\":%1d,\n", msg->msg18.react_pow_pos);
    printf("\"React_Pow_M_Q3_Q4\":%1d,\n", msg->msg18.react_pow_neg);
    printf("\"Curr_L1\":%1d,\n", msg->msg18.curr_L1);
    printf("\"Curr_L2\":%1d,\n", msg->msg18.curr_L2);
    printf("\"Curr_L3\":%1d,\n", msg->msg18.curr_L3);
    printf("\"Volt_L1\":%1d,\n", msg->msg18.volt_L1);
    printf("\"Volt_L2\":%1d,\n", msg->msg18.volt_L2);
    printf("\"Volt_L3\":%1d,\n", msg->msg18.volt_L3);
    printf("\"Date_Time2\":\"%s\",\n", msg->msg18.date_time);
    printf("\"Act_Energy_P\":%1d,\n", msg->msg18.act_energy_pa);
    printf("\"Act_Energy_M\":%1d,\n", msg->msg18.act_energy_ma);
    printf("\"React_Energy_P\":%1d,\n", msg->msg18.act_energy_pr);
    printf("\"React_Energy_M\":%1d}\n", msg->msg18.act_energy_mr);
    break;
  }

  return 0;
}


// HELPER FUNCTIONS FOR THE JSON MESSAGE

char * json_print_int(char * startPos, char key[], int value, JsonFormat * fmt, bool last) {
	char * endPos = startPos;
	endPos += sprintf(endPos, "%s\"%s\":%s%d%s%s", fmt->indent, key, fmt->space, value, last ? "" : ",", fmt->eol);
	return endPos;
}

char * json_print_lng(char * startPos, char key[], long value, JsonFormat * fmt, bool last) {
	char * endPos = startPos;
	endPos += sprintf(endPos, "%s\"%s\":%s%ld%s%s", fmt->indent, key, fmt->space, value, last ? "" : ",", fmt->eol);
	return endPos;
}

char * json_print_dbl(char * startPos, char key[], double value, JsonFormat * fmt, bool last) {
	char * endPos = startPos;
	endPos += sprintf(endPos, "%s\"%s\":%s%g%s%s", fmt->indent, key, fmt->space, value, last ? "" : ",", fmt->eol);
	return endPos;
}

char * json_print_str(char * startPos, char key[], char value[], JsonFormat * fmt, bool last) {
	char * endPos = startPos;
	endPos += sprintf(endPos, "%s\"%s\":%s\"%s\"%s%s", fmt->indent, key, fmt->space, value, last ? "" : ",", fmt->eol);
	return endPos;
}


char * add_to_json_1(Items1 * msg, char * startPos, JsonFormat * fmt, bool raw, bool last) {
	char * endPos = startPos;
	if (raw) 
		endPos = json_print_int(endPos, "Act_Pow_P_Q1_Q4", msg->act_pow_pos, fmt, last); 
	else
		endPos = json_print_dbl(endPos, "Act_Pow_P_Q1_Q4", msg->act_pow_pos / 1000.0, fmt, last);  // W -> kW 
	return endPos;
}

char * add_to_json_13(Items13 * msg, char * startPos, JsonFormat * fmt, bool raw, bool last) {
	char * endPos = startPos;
	endPos = json_print_str(endPos, "OBIS_List_Version", msg->obis_list_version, fmt, false); 
	endPos = json_print_str(endPos, "GS1", msg->gs1, fmt, false); 
	endPos = json_print_str(endPos, "Meter_Model", msg->meter_model, fmt, false); 
	if (raw) {
		endPos = json_print_int(endPos, "Act_Pow_P_Q1_Q4", msg->act_pow_pos, fmt, false); 
		endPos = json_print_int(endPos, "Act_Pow_M_Q2_Q3", msg->act_pow_neg, fmt, false);
		endPos = json_print_int(endPos, "React_Pow_P_Q1_Q2", msg->react_pow_pos, fmt, false);
		endPos = json_print_int(endPos, "React_Pow_M_Q3_Q4", msg->react_pow_neg, fmt, false);
		endPos = json_print_int(endPos, "Curr_L1", msg->curr_L1, fmt, false);
		endPos = json_print_int(endPos, "Curr_L2", msg->curr_L2, fmt, false);
		endPos = json_print_int(endPos, "Curr_L3", msg->curr_L3, fmt, false);
		endPos = json_print_int(endPos, "Volt_L1", msg->volt_L1, fmt, false);
		endPos = json_print_int(endPos, "Volt_L2", msg->volt_L2, fmt, false);
		endPos = json_print_int(endPos, "Volt_L3", msg->volt_L3, fmt, last);
	} else {
		// power is in W -> konvert to kW
		// current is in mA -> convert to A
		// voltage is in 0.1V -> convert to V
		endPos = json_print_dbl(endPos, "Act_Pow_P_Q1_Q4", msg->act_pow_pos / 1000.0, fmt, false);
		endPos = json_print_dbl(endPos, "Act_Pow_M_Q2_Q3", msg->act_pow_neg / 1000.0, fmt, false);
		endPos = json_print_dbl(endPos, "React_Pow_P_Q1_Q2", msg->react_pow_pos / 1000.0, fmt, false);
		endPos = json_print_dbl(endPos, "React_Pow_M_Q3_Q4", msg->react_pow_neg / 1000.0, fmt, false);
		endPos = json_print_dbl(endPos, "Curr_L1", msg->curr_L1 / 1000.0, fmt, false);
		endPos = json_print_dbl(endPos, "Curr_L2", msg->curr_L2 / 1000.0, fmt, false);
		endPos = json_print_dbl(endPos, "Curr_L3", msg->curr_L3 / 1000.0, fmt, false);
		endPos = json_print_dbl(endPos, "Volt_L1", msg->volt_L1 / 10.0, fmt, false);
		endPos = json_print_dbl(endPos, "Volt_L2", msg->volt_L2 / 10.0, fmt, false);
		endPos = json_print_dbl(endPos, "Volt_L3", msg->volt_L3 / 10.0, fmt, last);
	}
	return endPos;
}

char * add_to_json_18(Items18 * msg, char * startPos, JsonFormat * fmt, bool raw, bool last) {
	char * endPos = startPos;
	endPos = json_print_str(endPos, "OBIS_List_Version", msg->obis_list_version, fmt, false); 
	endPos = json_print_str(endPos, "GS1", msg->gs1, fmt, false); 
	endPos = json_print_str(endPos, "Meter_Model", msg->meter_model, fmt, false); 
	if (raw) {
		endPos = json_print_int(endPos, "Act_Pow_P_Q1_Q4", msg->act_pow_pos, fmt, false); 
		endPos = json_print_int(endPos, "Act_Pow_M_Q2_Q3", msg->act_pow_neg, fmt, false);
		endPos = json_print_int(endPos, "React_Pow_P_Q1_Q2", msg->react_pow_pos, fmt, false);
		endPos = json_print_int(endPos, "React_Pow_M_Q3_Q4", msg->react_pow_neg, fmt, false);
		endPos = json_print_int(endPos, "Curr_L1", msg->curr_L1, fmt, false);
		endPos = json_print_int(endPos, "Curr_L2", msg->curr_L2, fmt, false);
		endPos = json_print_int(endPos, "Curr_L3", msg->curr_L3, fmt, false);
		endPos = json_print_int(endPos, "Volt_L1", msg->volt_L1, fmt, false);
		endPos = json_print_int(endPos, "Volt_L2", msg->volt_L2, fmt, false);
		endPos = json_print_int(endPos, "Volt_L3", msg->volt_L3, fmt, false);
		endPos = json_print_str(endPos, "Date_Time2", msg->date_time, fmt, false);
		endPos = json_print_int(endPos, "Act_Energy_P", msg->act_energy_pa, fmt, false);
		endPos = json_print_int(endPos, "Act_Energy_M", msg->act_energy_ma, fmt, false);
		endPos = json_print_int(endPos, "React_Energy_P", msg->act_energy_pr, fmt, false);
		endPos = json_print_int(endPos, "React_Energy_M", msg->act_energy_mr, fmt, last);
	} else {
		// power is in W -> konvert to kW
		// current is in mA -> convert to A
		// voltage is in 0.1V -> convert to V
		// energy is in Wh -> convert to kWh
		endPos = json_print_dbl(endPos, "Act_Pow_P_Q1_Q4", msg->act_pow_pos / 1000.0, fmt, false);
		endPos = json_print_dbl(endPos, "Act_Pow_M_Q2_Q3", msg->act_pow_neg / 1000.0, fmt, false);
		endPos = json_print_dbl(endPos, "React_Pow_P_Q1_Q2", msg->react_pow_pos / 1000.0, fmt, false);
		endPos = json_print_dbl(endPos, "React_Pow_M_Q3_Q4", msg->react_pow_neg / 1000.0, fmt, false);
		endPos = json_print_dbl(endPos, "Curr_L1", msg->curr_L1 / 1000.0, fmt, false);
		endPos = json_print_dbl(endPos, "Curr_L2", msg->curr_L2 / 1000.0, fmt, false);
		endPos = json_print_dbl(endPos, "Curr_L3", msg->curr_L3 / 1000.0, fmt, false);
		endPos = json_print_dbl(endPos, "Volt_L1", msg->volt_L1 / 10.0, fmt, false);
		endPos = json_print_dbl(endPos, "Volt_L2", msg->volt_L2 / 10.0, fmt, false);
		endPos = json_print_dbl(endPos, "Volt_L3", msg->volt_L3 / 10.0, fmt, false);
		endPos = json_print_str(endPos, "Date_Time2", msg->date_time, fmt, false);
		endPos = json_print_dbl(endPos, "Act_Energy_P", msg->act_energy_pa / 1000.0, fmt, false);
		endPos = json_print_dbl(endPos, "Act_Energy_M", msg->act_energy_ma / 1000.0, fmt, false);
		endPos = json_print_dbl(endPos, "React_Energy_P", msg->act_energy_pr / 1000.0, fmt, false);
		endPos = json_print_dbl(endPos, "React_Energy_M", msg->act_energy_mr / 1000.0, fmt, last);
	}
	return endPos;
}


int msg_as_json(
	HanMsg *msg,
	char *buf,
	JsonFormat * fmt,
	bool raw,
	char *tm_str)
{
	char * endPos = buf;

	endPos += sprintf(endPos, "{%s", fmt->eol);
	endPos = json_print_str(endPos, "Date_Time", msg->tm, fmt, false);
	if (tm_str) {
		int year, month, day;
		int hour, min, sec;
		
		// TODO: store time_t value in msg in decodeMessage, so we do not need to do this!
		// FORMAT ex.: "2017-06-07 22:25:30" 
		if (sscanf(msg->tm, "%4d-%2d-%2d %2d:%2d:%2d", &year, &month, &day, &hour, &min, &sec) == 6) {
			struct tm info;
			info.tm_year = year - 1900;
			info.tm_mon = month - 1;
			info.tm_mday = day;
			info.tm_hour = hour;
			info.tm_min = min;
			info.tm_sec = sec;
			info.tm_isdst = -1;
			
			time_t tm_sec = mktime(&info);
			endPos = json_print_lng(endPos, "Meter_Time", tm_sec, fmt, false);
		}
		endPos = json_print_str(endPos, "Host_Time", tm_str, fmt, false);
	}

	switch (msg->num_items) {
	case 1:
		endPos = add_to_json_1(&(msg->msg1), endPos, fmt, raw, true);
		break;
	case 9:
		break;
	case 13:
		endPos = add_to_json_13(&(msg->msg13), endPos, fmt, raw, true);
		break;
	case 14:
		break;
	case 18:
		endPos = add_to_json_18(&(msg->msg18), endPos, fmt, raw, true);
		break;
	}

	endPos += sprintf(endPos, "}%s", fmt->eol);
	
	return endPos - buf;
}
