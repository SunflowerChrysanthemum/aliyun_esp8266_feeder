#include "ESP8266.h"

uint8_t start_flag = 0;
uint8_t ok_flag = 1;
uint8_t data_flag = 0;

void ESP8266_Connect_Wifi(void)
{
	printf("AT+RST\r\n");
	Delay_ms(200);
	printf("AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_NAME, WIFI_KEY);
	printf("AT+CWJAP=\"box123\",\"244466666\"\r\n");
}


void ESP8266_Init(void)
{
	printf("AT+RST\r\n");
	Delay_s(3);
	
	ok_flag = 1;
	start_flag = 1;
	printf("AT+CWMODE=1\r\n");
	while(ok_flag == 0);
	Delay_ms(500);
	
	ok_flag = 0;
	start_flag = 2;
	printf("AT+MQTTUSERCFG=0,1,\"NULL\",\"%s\",\"%s\",0,0,\"\"\r\n", USERNAME, PASSWD);
	while(ok_flag == 0);
	Delay_ms(500);
	
	ok_flag = 0;
	start_flag = 3;
	printf("AT+MQTTCLIENTID=0,\"%s\"\r\n", CLIENTLD);
	while(ok_flag == 0);
	Delay_ms(500);
	
	ok_flag = 0;
	start_flag = 4;
	//printf("AT+MQTTCONN=0,\"%s\",%s,1\r\n", MQTHOSTURL, PORT);
	printf("AT+MQTTCONN=0,\"iot-06z00e5p5s5m01p.mqtt.iothub.aliyuncs.com\",1883,1\r\n");
	while(ok_flag == 0);
	Delay_ms(500);
	
	ok_flag = 0;
	start_flag = 5;
	/* subscribe data
	AT+MQTTSUB=0,"/sys/k1lw9sOkV7x/esp8266/thing/service/property/set",1 */
	printf("AT+MQTTSUB=0,\"%s\",1\r\n", SUBSCRIBE);
	while(ok_flag == 0);
	Delay_ms(500);
	
	ok_flag = 0;
	start_flag = 6;
	ESP8266_TotalWeight(0);
	while(ok_flag == 0);
	Delay_ms(500);
	start_flag = 0;
	
	data_flag = 1;
}

//AT+MQTTPUB=0,"/k1lw9sOkV7x/esp8266/user/esp8266","{\"tubweight\":4}",1,0
void ESP8266_TubWeight(uint8_t value)
{
	printf("AT+MQTTPUB=0,\"%s\",\"{\\\"tubweight\\\":%d}\",1,0\r\n", RULE_ESP8266, value);
}

//AT+MQTTPUB=0,"/k1lw9sOkV7x/esp8266/user/esp8266","{\"totalweight\":12}",1,0
void ESP8266_TotalWeight(uint8_t value)
{
	printf("AT+MQTTPUB=0,\"%s\",\"{\\\"totalweight\\\":%d}\",1,0\r\n", RULE_ESP8266, value);
}

