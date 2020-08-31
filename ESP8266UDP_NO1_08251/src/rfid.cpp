#include "rfid.h"

#include <Ticker.h>
/**
 * RFID放上去：AA AB 0D 00 90 09 01 AB 67 CC D7 50 01 04 E0 F7
 *            AA AB 0D 00 90 0C 01 7E 5A CC D7 50 01 04 E0 1A
 * RFID拿起来：AA AB 05 00 90 09 FF 63
 *            AA AB 05 00 90 0C FF 66
 * 偶尔会有可能放下去的瞬间检测到2次
 */

String cmd = "";
uint8 RX_STA_STATUS = 0;
Ticker tickerRead;  //声明Ticker对象

void RFID_init(void) {
    delay(5000);
    rfid_sendData((char *)rfid_close_LED, sizeof(rfid_close_LED));
}

void rfid_sendData(char *pbuf, int len) {
    for (int i = 0; i < len; i++) {
        Serial.write(pbuf[i]);
    }
}
/*
void callback()  //回调函数 接收完成
{
    tickerRead.detach();  //停止ticker1
}*/

String Read_RFID_UP_DOWN(void) {
    cmd = "";
    while (Serial.available() > 0) {
        // tickerRead.attach(0.1, callback);  //每0.1秒调用callback1
        char res = char(Serial.read());
        RX_STA_STATUS++;
        cmd += res;
        if (RX_STA_STATUS >= 128) {
            RX_STA_STATUS = 0;
            cmd = "";
            // tickerRead.detach();  //停止ticker1
            return cmd;
        }
    }
    return cmd;
}

uint8_t CRC8_SUM(uint8_t *ptr, uint8_t len) {
    unsigned char i;
    unsigned char checksum;
    checksum = 0;
    for (i = 0; i < len; i++) {
        checksum ^= ptr[i + 2];
    }
    Serial.printf("checksum = %02x\r\n", checksum);
    return checksum;
}
