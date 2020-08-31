#include "ntp.h"

/**
 * 参考代码 https://github.com/PaulStoffregen/Time.git
 */
static const char ntpServerName[] = "ntp1.aliyun.com";
const int NTP_PACKET_SIZE = 48;  // NTP time is in the first 48 bytes of message
byte ntppacketBuffer[NTP_PACKET_SIZE];  // buffer to hold incoming & outgoing
                                        // packets
const int timeZone = 1;                 // Central European Time

WiFiUDP Udp_NTP;
uint16_t localPort_NTP = 8888;

void NTP_Init() {
    Udp_NTP.begin(localPort_NTP);
    setSyncProvider(getNtpTime);
    setSyncInterval(300);
}

time_t getNtpTime() {
    IPAddress ntpServerIP;  // NTP server's ip address

    while (Udp_NTP.parsePacket() > 0)
        ;  // discard any previously received packets
#if DEBUGPRINRF
    //Serial.println("Transmit NTP Request");
#endif
    // get a random server from the pool
    WiFi.hostByName(ntpServerName, ntpServerIP);
#if DEBUGPRINRF
    //Serial.print(ntpServerName);
    //Serial.print(": ");
    //Serial.println(ntpServerIP);
#endif
    sendNTPpacket(ntpServerIP);
    uint32_t beginWait = millis();
    while (millis() - beginWait < 1500) {
        int size = Udp_NTP.parsePacket();
        if (size >= NTP_PACKET_SIZE) {
#if DEBUGPRINRF
            //Serial.println("Receive NTP Response");
#endif
            Udp_NTP.read(ntppacketBuffer,
                         NTP_PACKET_SIZE);  // read packet into the buffer
            unsigned long secsSince1900;
            // convert four bytes starting at location 40 to a long integer
            secsSince1900 = (unsigned long)ntppacketBuffer[40] << 24;
            secsSince1900 |= (unsigned long)ntppacketBuffer[41] << 16;
            secsSince1900 |= (unsigned long)ntppacketBuffer[42] << 8;
            secsSince1900 |= (unsigned long)ntppacketBuffer[43];
            return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
        }
    }
#if DEBUGPRINRF
    Serial.println("No NTP Response :-(");
#endif
    return 0;  // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address) {
    // set all bytes in the buffer to 0
    memset(ntppacketBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    ntppacketBuffer[0] = 0b11100011;  // LI, Version, Mode
    ntppacketBuffer[1] = 0;           // Stratum, or type of clock
    ntppacketBuffer[2] = 6;           // Polling Interval
    ntppacketBuffer[3] = 0xEC;        // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    ntppacketBuffer[12] = 49;
    ntppacketBuffer[13] = 0x4E;
    ntppacketBuffer[14] = 49;
    ntppacketBuffer[15] = 52;
    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    Udp_NTP.beginPacket(address, 123);  // NTP requests are to port 123
    Udp_NTP.write(ntppacketBuffer, NTP_PACKET_SIZE);
    Udp_NTP.endPacket();
}
