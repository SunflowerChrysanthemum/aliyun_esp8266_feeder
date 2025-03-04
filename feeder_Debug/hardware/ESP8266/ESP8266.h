#ifndef __ESP8266_H
#define	__ESP8266_H

#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"

extern uint8_t start_flag;
extern uint8_t ok_flag;

#define WIFI_NAME			"box123"
#define WIFI_KEY			"244466666"

/* dynamic */
#define	CLIENTLD			"k1lw9sOkV7x.esp8266|securemode=2\\,signmethod=hmacsha256\\,timestamp=1740046724043|"
#define	PASSWD				"65a403e4b91d97911336269ad610476046a881aff7ec45a419e8a68cd833a3a3"

/* static */
#define	USERNAME			"esp8266&k1lw9sOkV7x"
#define	MQTHOSTURL			"iot-06z00e5p5s5m01p.mqtt.iothub.aliyuncs.com"
#define	PORT				"1883"
#define	SUBSCRIBE			"/sys/k1lw9sOkV7x/esp8266/thing/service/property/set"

#define RULE_ESP8266		"/k1lw9sOkV7x/esp8266/user/esp8266"


void ESP8266_Connect_Wifi(void);
void ESP8266_Init(void);
void ESP8266_TubWeight(uint8_t value);
void ESP8266_TotalWeight(uint8_t value);

#endif
