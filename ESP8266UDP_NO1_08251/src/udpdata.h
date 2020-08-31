#ifndef __UDPDATA_H_
#define __UDPDATA_H_
#include "config.h"

void wifiUDP_Init(void);                           // WiFi UDP 通讯初始化
void wifiUDPdataSend(char *pBuf, size_t size);  // UDP 数据发送函数

#endif
