#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include<SoftwareSerial.h>
#include <ArduinoJson.h> 
#include "DHT.h"
// 设置wifi接入信息(请根据您的WiFi信息进行修改)
const char* ssid ="vivo iQOO Pro"; // Enter your WiFi name 
const char* password = "e23ae343b63c"; // Enter WiFi password
const char* mqttServer = "broker.mqtt-dashboard.com";//"test.ranye-iot.net";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
long lastMsg = 0;////////////
char msg[50];
int value = 0;///////////

SoftwareSerial softSerial1(3,1);//rx(D1 3) tx(d1 1)
// 两个字符串分别用于存储A、B两端传来的数据:
  String device_B_String="";
  String device_A_String=""; //从uno端接收到的数据
  const char* hotnow;
///引脚信息
#define DHTPIN 14//D5温湿度
#define DHTTYPE DHT11 // DHT 11 
#define rgbr 16//D2
#define rgbg 5//D3
#define rgbb 4//D4
#define pengzhuang 12//D6门1
#define pengzhuang1 13//D7门2
#define pinBuzzer 0//D8蜂鸣器
#define Echo 15//D10 超声波echo
#define Trig 2//D9 超声波trig
#define fengshan A0//风扇

  double ddistance,ttime;//超声波的东西
  int csb=0;//在车库
  int hot=0;//无人
  int zhuangtai=0;int zhuangtai2=0;//pengzhuang的关门
  int pwm=0;//led关灯
  int homemode=0;//在家 
  int fan=0;//风扇关
  unsigned long previousMillis = 0;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
DHT dht(DHTPIN, DHTTYPE); 

/////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
///////////          setup          ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // 设置板上LED引脚为输出模式
  digitalWrite(LED_BUILTIN, HIGH);  // 启动后关闭板上LED
  pinMode(Trig,OUTPUT);pinMode(Echo,INPUT);
  pinMode(fengshan,OUTPUT);
  Serial.begin(9600);               // 启动串口通讯
  softSerial1.begin(9600);//初始化serial1，该串口用于与设备B连接通信；
  softSerial1.listen();
  
  
  WiFi.mode(WIFI_STA);//设置ESP8266工作模式为无线终端模式
  connectWifi();// 连接WiFi 
  mqttClient.setServer(mqttServer, 1883);// 设置MQTT服务器和端口号
  mqttClient.setCallback(receiveCallback);// 设置MQTT订阅回调函数
 
 
  connectMQTTserver(); // 连接MQTT服务器
  Serial.println("在setup");
}
 
void loop() {
  if (mqttClient.connected()) { // 如果开发板成功连接服务器
    mqttClient.loop();          // 处理信息以及心跳
    door();door2();wenshi();csb1(); people1(); 
   
  } else {                      // 如果开发板未能成功连接服务器
    connectMQTTserver();        // 则尝试连接服务器
  }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////          function          /////////////////////////////
//////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void people1(){//这个是先通过uno传过来
  //int hot=0;//无人
  int hotNow=0;//
  if(softSerial1.available()>0)//读取从设备A（uno）传入的数据，并在串口监视器中显示
  {
    if(softSerial1.peek()!='\n'){
      device_A_String+=(char)softSerial1.read();      
    }
    else{
      softSerial1.read();
      hotnow = device_A_String.c_str();                //由于sscanf只能识别const char*类型字符串，将String类型字符串转成const char*类型
      sscanf(hotnow,"%d",&hotNow);   //把传来的hotnow赋给hotNow.串口1接收字符串格式为150
      Serial.print(hotNow);Serial.print(" ");Serial.print(hot);
      device_A_String="";//DynamicJsonDocument data(256);  

      if(hotNow==0){//关时
        if(hot!=0){
            Serial.print("无人了");hot=0;  //DynamicJsonDocument data(256); data["shut"]=zhuangtai;   
            char json_string[256]="";json_string[0]='0';  Serial.println(json_string);mqttClient.publish("Test-Carrisa-People",json_string, false); 
          }//当之前状态为有人时才打印出无人了“  
       }
      else if(hotNow==1){
         if(hot==0){
            Serial.print("来人了");hot=1;  //DynamicJsonDocument data(256); data["shut"]=zhuangtai;   
            char json_string[256]="";json_string[0]='1';  Serial.println(json_string);mqttClient.publish("Test-Carrisa-People",json_string, false); 
            }         //当之前状态为wuren时才打印出"人来了"
        }

      
     }  //else的
    }//if的
  
  
  }
void csb1(){
  digitalWrite(Trig,LOW);
  delayMicroseconds(2);
  digitalWrite(Trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig,LOW);
  ttime=pulseIn(Echo,HIGH);
  ddistance=ttime/58;//Serial.print("d is");Serial.print(ddistance);
  int csbNow;
  if(ddistance>0.08){csbNow=1;}else{csbNow=0;}
  if(csbNow==0){//有车时
    if(csb!=0){
      Serial.print("车入库了");Serial.print(ddistance);
      DynamicJsonDocument data(256); 
     // data["cari"]=csb;serializeJson(data, json_string);
      char json_string[256]=""; json_string[0]='0';Serial.println(json_string); mqttClient.publish("Test-Carrisa-Csb",json_string, false); 
     
      } csb=0;
      }//
   if(csbNow==1){//无车时
    if(csb==0){
      Serial.print("车开走了");Serial.print(ddistance);
      DynamicJsonDocument data(256); 
      //data["caro"]=csb;
      char json_string[256]="";json_string[0]='1'; Serial.println(json_string); mqttClient.publish("Test-Carrisa-Csb",json_string, false); 
      }csb=1;
      }//
}
  
  
  

//硬件的门->mqtt
void door(){
  int zhuangtaiNow=digitalRead(pengzhuang);
  if(zhuangtaiNow==LOW){//关时
    if(zhuangtai!=0){
      Serial.print("门关上了");zhuangtai=0;
      DynamicJsonDocument data(256); 
      //data["shut"]=zhuangtai;
      
      char json_string[256]="";json_string[0]='0';  Serial.println(json_string);mqttClient.publish("Test-Carrisa-Door",json_string, false);
     // serializeJson(data, json_string); 
      
      }//当之前状态为开时才打印出”门关“
    
    }
  else if(zhuangtaiNow==HIGH){
    if(zhuangtai==0){
      Serial.print("门打开了");zhuangtai=1;
      DynamicJsonDocument data(256); 
      //data["open"]=zhuangtai;
      // data=zhuangtai;
      char json_string[256]="";json_string[0]='1';  Serial.println(json_string);mqttClient.publish("Test-Carrisa-Door", json_string, false);//{"shut":0}
      //serializeJson(data, json_string); 
      if(homemode==0){digitalWrite(pinBuzzer,LOW);}else{digitalWrite(pinBuzzer,HIGH);}
      
      }//当之前状态为关时才打印出"门开"
    }
}
void door2(){
  int zhuangtai2Now=digitalRead(pengzhuang);
  if(zhuangtai2Now==LOW){//关时
    if(zhuangtai2!=0){
      Serial.print("门2关上了");zhuangtai2=0;
      DynamicJsonDocument data(256); 
      //data["shut"]=zhuangtai2;
      
      char json_string[256]="";json_string[0]='0';  Serial.println(json_string);mqttClient.publish("Test-Carrisa-Door2",json_string, false);
     // serializeJson(data, json_string); 
      
      }//当之前状态为开时才打印出”门关“
    
    }
  else if(zhuangtai2Now==HIGH){
    if(zhuangtai2==0){
      Serial.print("门2打开了");zhuangtai2=1;
      DynamicJsonDocument data(256); 
      //data["open"]=zhuangtai;
       //data=zhuangtai;
      char json_string[256]="";json_string[0]='1';  Serial.println(json_string);mqttClient.publish("Test-Carrisa-Door2", json_string, false);//{"shut":0}
      //serializeJson(data, json_string); 
      if(homemode==0){digitalWrite(pinBuzzer,LOW);}else{digitalWrite(pinBuzzer,HIGH);}
      
      }//当之前状态为关时才打印出"门开"
    }
}



void wenshi(){
    unsigned long currentMillis = millis(); // temperature and humidity data are publish every five second 
  if (currentMillis - previousMillis >= 10000) { 
    previousMillis = currentMillis; 
    float temp = dht.readTemperature(); float hum = dht.readHumidity(); // json serialize 
    DynamicJsonDocument data(256); 
    data["temp"] = temp; data["hum"] = hum; // publish temperature and humidity 
    
    char json_string[256]; 
   serializeJson(data, json_string);  // {"temp":23.5,"hum":55} 
   
    Serial.println(json_string); 
    mqttClient.publish("Test-Carrisa-Wenshi", json_string, false); 
    } 
  }
// 连接MQTT服务器并订阅信息
void connectMQTTserver(){
  // 根据ESP8266的MAC地址生成客户端ID（避免与其它ESP8266的客户端ID重名）
  String clientId = "esp8266-" + WiFi.macAddress();
 
  // 连接MQTT服务器
  if (mqttClient.connect(clientId.c_str())) { 
    Serial.println("MQTT Server Connected.");
    Serial.print("Server Address:");
    Serial.println(mqttServer);
    Serial.print("ClientId: ");
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
  if ((char)payload[0] == '5') {
             Serial.println("设备连接中.....");
             delay(2000);
            int mull = 100; 
            snprintf (msg, 50, "%ld", mull);
          mqttClient.publish("Carrisa-Test-Light1", msg);
         Serial.println(msg); analogWrite(rgbr,100);
           }
  
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
    else if(strcmp(topic,"Carrisa-Test-Fan")==0){
          Serial.print("订阅到模式");Serial.print("Message Received [");Serial.print(topic);Serial.println("] ");
          for (int i = 0; i < length; i++) {
              Serial.print((char)payload[i]);
            }Serial.print("");Serial.print("Message Length(Bytes) ");Serial.println(length);
       int fanNow = byteArrayToInt(payload,length);Serial.print(fanNow);
       if(fan!=fanNow){
        fan=fanNow;Serial.print(fan);
        if(fan==0){Serial.print("");Serial.print("风扇关");}else if(fan==1){Serial.print("");Serial.print("风扇开");}
        Serial.print("fan=>");Serial.println(fan);
        }
        if(fan==0){digitalWrite(fengshan,LOW);}else{digitalWrite(fengshan,HIGH);}
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
  if(mqttClient.subscribe(subTopic)){Serial.print("Subscrib Topic:");Serial.println(subTopic);} 
  else {Serial.print("Subscribe Fail...");} 
  if(mqttClient.subscribe(subTopic2)){Serial.print("Subscrib Topic:");Serial.println(subTopic2);} 
  else {Serial.print("Subscribe Fail...");}
  if(mqttClient.subscribe("Carrisa-Test-Fan")){Serial.print("Subscrib Topic:");Serial.println(subTopic2);} 
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
