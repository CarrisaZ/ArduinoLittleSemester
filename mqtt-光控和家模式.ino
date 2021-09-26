clude <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> 
#include "DHT.h"
// 设置wifi接入信息(请根据您的WiFi信息进行修改)
const char* ssid ="vivo iQOO Pro"; // Enter your WiFi name 
const char* password = "e23ae343b63c"; // Enter WiFi password
const char* mqttServer = "test.ranye-iot.net";

#define DHTPIN 14//D5
#define DHTTYPE DHT11 // DHT 11 
#define rgbr 16//D2
#define rgbg 5//D3
#define rgbb 4//D4
#define pengzhuang 12;//D6
  int zhuangtai=0;//pengzhuang的关门
  int pwm=0;//led关灯
  int homemode=0;//在家 
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
DHT dht(DHTPIN, DHTTYPE); 
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // 设置板上LED引脚为输出模式
  digitalWrite(LED_BUILTIN, HIGH);  // 启动后关闭板上LED
  Serial.begin(9600);               // 启动串口通讯
  
  //设置ESP8266工作模式为无线终端模式
  WiFi.mode(WIFI_STA);
  
  // 连接WiFi
  connectWifi();
  
  // 设置MQTT服务器和端口号
  mqttClient.setServer(mqttServer, 1883);
  // 设置MQTT订阅回调函数
  mqttClient.setCallback(receiveCallback);
 
  // 连接MQTT服务器
  connectMQTTserver();
}
 
void loop() {
  if (mqttClient.connected()) { // 如果开发板成功连接服务器
    mqttClient.loop();          // 处理信息以及心跳
  } else {                      // 如果开发板未能成功连接服务器
    connectMQTTserver();        // 则尝试连接服务器
  }
}
 
// 连接MQTT服务器并订阅信息
void connectMQTTserver(){
  // 根据ESP8266的MAC地址生成客户端ID（避免与其它ESP8266的客户端ID重名）
  String clientId = "esp8266-" + WiFi.macAddress();
 
  // 连接MQTT服务器
  if (mqttClient.connect(clientId.c_str())) { 
    Serial.println("MQTT Server Connected.");
    Serial.println("Server Address:");
    Serial.println(mqttServer);
    Serial.println("ClientId: ");
    Serial.println(clientId);
    subscribeTopic(); // 订阅指定主题
  } else {
    Serial.print("MQTT Server Connect Failed. Client State:");
    Serial.println(mqttClient.state());
    delay(5000);
  }   
}//(connectMQTTserver()
 
// 收到信息后的回调函数
void receiveCallback(char* topic, byte* payload, unsigned int length) {
  if(strcmp(topic,"Carrisa-Test-Light")==0){
    Serial.print("订阅到光控");
    Serial.print("Message Received [");Serial.print(topic);Serial.println("] ");
       for (int i = 0; i < length; i++) {
          Serial.print((char)payload[i]);
          }Serial.print("Message Length(Bytes) ");Serial.println(length);
    int pwmNow = byteArrayToInt(payload,length); // 获取用户请求中的PWM数值
      if(pwmNow!=pwm){
        pwm=pwmNow;
        Serial.print("pwm => ");Serial.println(pwm);}//
     int pwmNow1=pwmNow*2;analogWrite(rgbr,pwmNow1);analogWrite(rgbg,pwmNow1);analogWrite(rgbb,pwmNow1);
    }
    else if(strcmp(topic,"Carrisa-Test-Mode")==0){
      Serial.print("订阅到模式");
      Serial.print("Message Received [");Serial.print(topic);Serial.println("] ");
          for (int i = 0; i < length; i++) {
              Serial.print((char)payload[i]);
            }Serial.print("");Serial.print("Message Length(Bytes) ");Serial.println(length);
       int homemodeNow = byteArrayToInt(payload,length);
     if(homemodeNow!=homemode){
        homemode=homemodeNow;Serial.print(homemode);Serial.print(homemodeNow);
        if(homemode==0){Serial.print("");Serial.print("在家");}else if(homemode==1){Serial.print("");Serial.print("外出");}
        Serial.print("homemode=>");Serial.println(homemode);
     }
    }
    
  }
  

 
// 订阅指定主题
void subscribeTopic(){
 
  // 建立订阅主题1。主题名称以Taichi-Maker-Sub为前缀，后面添加设备的MAC地址。
  // 这么做是为确保不同设备使用同一个MQTT服务器测试消息订阅时，所订阅的主题名称不同
  String topicString = "Carrisa-Test-Light";// + WiFi.macAddress();
  char subTopic[topicString.length() + 1];  
  strcpy(subTopic, topicString.c_str());
  // 建立订阅主题2
  String topicString2 = "Carrisa-Test-Mode";// + WiFi.macAddress();
  char subTopic2[topicString2.length() + 1];  
  strcpy(subTopic2, topicString2.c_str());
  
  // 通过串口监视器输出是否成功订阅主题以及订阅的主题1,2名称
  if(mqttClient.subscribe(subTopic)){Serial.println("Subscrib Topic:");Serial.println(subTopic);} 
  else {Serial.print("Subscribe Fail...");} 
  if(mqttClient.subscribe(subTopic2)){Serial.println("Subscrib Topic:");Serial.println(subTopic2);} 
  else {Serial.print("Subscribe Fail...");}     
}
int byteArrayToInt(byte* b,unsigned int blength){
   // return (b[0]<<24)+((b[1]&0xFF)<<16)+((b[2]&0xFF)<<8)+(b[3]&0xFF);
   String num;
   for(int i=0;i<blength;i++){
    num=num+(char)b[i];
    
    }
    return num.toInt();
}

// ESP8266连接wifi
void connectWifi(){
 
  WiFi.begin(ssid, password);
 
  //等待WiFi连接,成功连接后输出成功信息
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected!");  
  Serial.println(""); 
}