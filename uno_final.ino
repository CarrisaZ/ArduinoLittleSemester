#include <dht11.h>
#include <Wire.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.h>
#include <SoftwareSerial.h> 

Servo myservo;//舵机对象

int pos = 0; //舵机角度
//温湿度部分
LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display
 SoftwareSerial softSerial(0,1);
 String device_A_String="";
String device_B_String="";
const char* hotnow;
dht11 DHT11;
#define DHT11PIN 2

//呼吸灯部分

//int incomedate = 0;

int PIR_sensor=3;
int redPin=7;
int greenPin=5;
int yellowPin=6;
int recvPin = 4;//定义红外接收器的引脚为3


IRrecv irrecv(recvPin);
decode_results results;
decode_results results1;

long KEY_CODE_ONE = 0xFFA25D;
long KEY_CODE_TWO = 0xFF629D;
long KEY_CODE_THREE = 0xFFE21D;
long KEY_CODE_FOUR = 0xFF22DD;
long KEY_CODE_FIVE = 0xFF02FD;
long KEY_CODE_SIX = 0xFFC23D;
long KEY_CODE_LONG_PRESS = 0xFF38C7;
long longPressKey = -1; //长按下的那个键

int val_PIR=0;//<50无人
int men=0;//门关

void setup()
{
  

  Serial.begin(9600);
   softSerial.begin(9600);
  softSerial.listen();
  
  pinMode(2,INPUT);
//  pinMode(3,INPUT);
  pinMode(5,OUTPUT);pinMode(6,OUTPUT);pinMode(7,OUTPUT);
  pinMode(4,OUTPUT);
  myservo.attach(8);
  // pinMode(13,OUTPUT);
  pinMode(PIR_sensor,INPUT);
  irrecv.enableIRIn(); // 初始化红外接收器
  Serial.print("fuck");
  //温湿度部分
  lcd.init(); // initialize the lcd 
  lcd.backlight();
  lcd.print("Welcom...");
  digitalWrite(greenPin,200);


  Serial.begin(9600); 
 
}

void loop()
{
      //人体热
      digitalWrite(greenPin,200);int hotNow=0;
      val_PIR = analogRead(PIR_sensor);
      if(val_PIR>=150){digitalWrite(yellowPin,200);hotNow=1;Serial.print("有人");}else {digitalWrite(yellowPin,0);hotNow=0;}
      softSerial.print(hotNow);softSerial.print(" ");
        //读取从设备B传入的数据，并在串口监视器中显示
  if(softSerial.available()>0){
    if(softSerial.peek()!='\n')
      {device_B_String+=(char)softSerial.read();}
    else{
      softSerial.read();
      hotnow = device_B_String.c_str();                //由于sscanf只能识别const char*类型字符串，将String类型字符串转成const char*类型
      sscanf(hotnow,"%d",&hotNow);   //串口1接收字符串格式为435;25.25
      Serial.println("hotNow");Serial.println(hotNow); 
      device_B_String="";
    }//else
  }//if
  
      //温湿度lcd
      int current_value=0;
      int chk = DHT11.read(DHT11PIN);
      //float humi=dht.readHumidity();float tempC=readTemperature();
      int tem=(float)DHT11.temperature;        //将温度值赋值给tem
      int hum=(float)DHT11.humidity; 
      lcd.setCursor(0, 0);lcd.print("Temp:");lcd.print((float)DHT11.temperature,2);lcd.write(0xDF);lcd.print("C    ");
      lcd.setCursor(0, 1); lcd.print("Humidity:"); lcd.print((float)DHT11.humidity,2); lcd.print("%    ");                   

    //遥控器舵机
      if(irrecv.decode(&results)) {   // 解码成功，把数据放入results.value变量中
         if(results.value==KEY_CODE_ONE){
            myservo.write(90);digitalWrite(redPin,200);digitalWrite(greenPin,0);
            delay(1200);
            myservo.write(0);digitalWrite(greenPin,200);digitalWrite(redPin,0);
          }irrecv.resume();    // 继续等待接收下一组信号
       
    }if(results.value==0x16) 
     delay(600);
         

  
  
 
}//这是loop部分（这个括号一定不能删）  
