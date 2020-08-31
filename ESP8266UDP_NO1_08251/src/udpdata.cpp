#include "udpdata.h"

WiFiUDP Udp;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1];
uint16_t localPort = 8000; //这是哪个的端口 本地端口

void wifiUDP_Init(void) {
    Udp.begin(localPort);
#if DEBUGPRINRF
    Serial.printf("UDP server on port %d\n", localPort);
#endif
}

void wifiUDPdataSend(char *pBuf, size_t size) {
#if (DEBUGPRINRF == 1)
    Udp.parsePacket();
    Udp.beginPacket(remote_IP, remote_port);
    //Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
#else
    Udp.parsePacket();
    Udp.beginPacket(remote_IP, remote_port);
    // Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
#endif
    //Udp.write(pBuf, size);  // send data
    Udp.write(pBuf);
    Udp.endPacket();
    // udp_status = UDP_SEDDING;
}
