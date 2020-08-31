#ifndef __CONFIG_H_
#define __CONFIG_H_
#include "ArduinoJson.h"
#include "ESP8266WiFi.h"
#include "TimeLib.h"
#include "WiFiUdp.h"
#include "ntp.h"

// 1使用测试  0正式使用
#define DEBUGPRINRF 1

#ifndef STASSID
#if DEBUGPRINRF

#define STASSID "ZJ4G"
#define STAPSK "1111122222"

//#define STASSID "HUAWEI_B316_B56F"
//#define STAPSK "zhongjia.123"

//#define STASSID "ISZA"
//#define STAPSK "11223344"
#else
#define STASSID "HUAWEI-B311-DAB4"
#define STAPSK "21898904"
#endif
#endif

#if DEBUGPRINRF
#define remote_IP "192.168.4.178"
#define remote_port 6666
#else
#define remote_IP "192.168.8.124"
#define remote_port 6666
#endif

#endif
