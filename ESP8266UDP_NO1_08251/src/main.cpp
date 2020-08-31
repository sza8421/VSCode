//2020年8月25日13:59:38 修改消息格式 改成 判断卡号；发送毫秒值
/*************************************************************/
#include "Arduino.h"
#include "config.h"
#include "ntp.h"
#include "rfid.h"
#include "udpdata.h"
/*************************************************************/
// WIFI HTTPClient examples
// https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266HTTPClient/examples
/*************************************************************/
char ReplyBuffer[12]; 
char recv_buff[12];
//char phone     = '1';  //干什么的 这几个最后去掉初值
//char areaId    = '1';  //区域号
int rfid      = 0;  //RFID卡号 内部需要做处理，读卡器的几种读卡情况
char tfp[20];        //存储时间
//char stateType =  '2'; //状态码
//char addString1= '1';  //第一种
char jsonbuf[128];  
/*************************************************************/
unsigned char USART_RX_BUF[64],USART_RX_STA=0;//定义数组储存数据
unsigned char USART_SIGN=0;
unsigned char RX_LEN = 0;

unsigned char rfid_flag[13]={0};

unsigned char area1_rfid1[24][2] = {
  {0x41,0x53},
  {0xB9,0x53},
  {0x5F,0x53},
  {0x24,0x53},
  {0x4E,0x50},
  {0x9B,0x53},
  {0x1B,0x7A},
};






int RFID_HIGH = 0;
int RFID_LOW  = 0;
int RFID_DEC = 0;

int gettime[20]  = {0}; //拿起的时长
int gettime1[20] = {0}; //拿起的时间
int gettime2[20] = {0}; //放下的时间

/*************************************************************/
String input = "{\"areaId\":\"1\",\"rfid\":\"1\",\"postTime\":\"1\",\"stateType\":\"1\"}"; 
/*************************************************************/
void wifi_disconnect_callback(void (*callback)()) { (*callback)(); }
/*************************************************************/
extern uint32_t system_get_time(void);
/*************************************************************/
void serialEvent()
{
  while(Serial.available())
  {
    USART_RX_BUF[USART_RX_STA]=(unsigned char)Serial.read();
    if(USART_RX_STA==0 && USART_RX_BUF[0]!=0xAA ) {return;}
    USART_RX_STA++;
    if(USART_RX_STA>=3)
    { 
      if(USART_RX_BUF[2]==0x05 || USART_RX_BUF[2]==0x0D)
      {
        RX_LEN = USART_RX_BUF[2] + 3; 
      }else{
        USART_RX_STA|=0x80;
      }
      if(USART_RX_STA == RX_LEN) {USART_RX_STA|=0x80;}
    }
    if(USART_RX_STA==64) {USART_RX_STA=0;USART_SIGN=1;}
  }
}
/*************************************************************/
void wifi_dis_cb(void) {
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.mode(WIFI_STA);
        WiFi.begin(STASSID, STAPSK);
        while (WiFi.status() != WL_CONNECTED) {
#if DEBUGPRINRF
            //Serial.print('.');
#endif
            delay(500);
        }
    }
}

void setup() {
#if DEBUGPRINRF
    Serial.begin(115200);
#else
    Serial.begin(115200);
#endif

    WiFi.mode(WIFI_STA);
    WiFi.begin(STASSID, STAPSK);
    while (WiFi.status() != WL_CONNECTED) {
#if DEBUGPRINRF
        //Serial.print('.');
#endif
        delay(500);
    }
#if DEBUGPRINRF
    //Serial.println();
    //Serial.print("Connected! IP address: ");
    //Serial.println(WiFi.localIP());
#endif
    //wifiUDP_Init();             // udp 初始化
    NTP_Init();                 // ntp服务器初始化
    RFID_init();                // rfid初始化
}

void loop() {
    wifi_disconnect_callback(wifi_dis_cb);
    /*************************************************************/
    serialEvent();
    /*************************************************************/
    if(USART_SIGN) {USART_SIGN = 0;}
    //可以改为判断这次接收缓存区数据的长度
    if(USART_RX_STA&0x80 && USART_RX_BUF[0]==0xAA && USART_RX_BUF[1]==0xAB) //接收完本次消息
    {
      switch(USART_RX_BUF[2])
      {
        case 0x05:      //拿开
        //rfid = USART_RX_BUF[5];
        gettime1[USART_RX_BUF[5]] = system_get_time(); 
        rfid_flag[USART_RX_BUF[5]] = 1;
		      break;
					
		    case 0x0D: //放下 识别到了
        if(rfid_flag[USART_RX_BUF[5]]==1)
        {
          rfid = USART_RX_BUF[5]; 
          gettime2[USART_RX_BUF[5]] = system_get_time(); 
          rfid_flag[USART_RX_BUF[5]] = 2; //放下了
        }
	     	  break;
					
		    case 0x15: //放下 识别到了 两组			
		    break;
					
		    default: 
        rfid = 0;
        break;
      }
      //清空相关
      USART_RX_STA=0;
      RX_LEN = 0;
      for(uint8_t i=0;i<64;i++) 
      { USART_RX_BUF[i] = 0;}
  }

  if(rfid!=0)
  {
      /*************************************************************/
      //北京在东八区，获取的小时需要+7
      //sprintf((char *)tfp, "%d-%d-%d %02d:%02d:%02d", year(), month(), day(),hour() + 7,minute(), second());
      /*************************************************************/
      //此部分为ArduinoJSON代码
      //参考https://github.com/bblanchon/ArduinoJson.git

      gettime[rfid] = int(gettime2[rfid] - gettime1[rfid])/1000;


      DynamicJsonDocument jsonBuffer(256);
      deserializeJson(jsonBuffer, input);
      JsonObject obj = jsonBuffer.as<JsonObject>();
      
      obj["areaId"]    = "1";

      if(rfid==1) obj["rfid"] = "1";
      if(rfid==2) obj["rfid"] = "3";
      if(rfid==3) obj["rfid"] = "4";
      if(rfid==4) obj["rfid"] = "5";
      if(rfid==5) obj["rfid"] = "6";
      if(rfid==6) obj["rfid"] = "7";
      if(rfid==7) obj["rfid"] = "8";
      if(rfid==8) obj["rfid"] = "9";
      if(rfid==9) obj["rfid"] = "10";
      if(rfid==10) obj["rfid"] = "11";
      if(rfid==11) obj["rfid"] = "12";
      if(rfid==12) obj["rfid"] = "14";



      obj["postTime"]  = gettime[rfid];
      obj["postTime"]  = gettime[rfid];
      obj["stateType"] = "2";
      //char jsonbuf[128];
      serializeJson(jsonBuffer, jsonbuf, sizeof(jsonbuf));
      /*************************************************************/
  #if DEBUGPRINRF
      //Serial.println(jsonbuf);
  #endif
      wifiUDPdataSend(jsonbuf, strlen(jsonbuf));  //数据发送函数

      
      rfid_flag[rfid]=0;
      rfid = 0;

  }
}     

/*
 * 2020年8月23日08:33:22 拿开的时候，识别不了卡号啊；还是单纯检测通道啊
 * 2020年8月27日17:59:22 回来补坑了
 *
 *
 *
 *
 */