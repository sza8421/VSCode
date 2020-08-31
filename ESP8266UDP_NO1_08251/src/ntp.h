#ifndef __NTP_H_
#define __NTP_H_
#include "config.h"
extern "C++" {

void NTP_Init();          // ntp服务器初始化
time_t getNtpTime(void);  //获取NTP时间，如果使用一周校准一次
void sendNTPpacket(IPAddress &address);
}
#endif
