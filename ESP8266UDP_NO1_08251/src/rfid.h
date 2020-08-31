#ifndef __RFID_H_
#define __RFID_H_
#include "Arduino.h"
#include "config.h"

const char rfid_close_LED[11] = {0xAA, 0xAB, 0x08, 0x00, 0x01, 0xFF,
                                 0x07, 0x01, 0x00, 0x00, 0xF0};

//const char rfid_close_LED[11] = {0xAA, 0xAB, 0x08, 0x00, 0x01, 0xFF,
//                                 0x01, 0x00, 0x00, 0x00, 0xF7};

void RFID_init(void);                     // RFID init
String Read_RFID_UP_DOWN(void);           //读取数据
void rfid_sendData(char *pbuf, int len);  //发送数据

#endif
