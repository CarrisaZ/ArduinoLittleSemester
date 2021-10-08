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
dht11 DHT11;
#define DHT11PIN 2

//呼吸灯部分

//int incomedate = 0;

int PIR_sensor=A3;
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
      digitalWrite(greenPin,200);
      val_PIR = analogRead(PIR_sensor);
      //zhuangtai=digitalRead(pengzhuang);//碰撞开关控制蜂鸣器
      if(val_PIR>=150){digitalWrite(yellowPin,200);Serial.print("有人");}else digitalWrite(yellowPin,0);

      int current_value=0;
      int chk = DHT11.read(DHT11PIN);
      //float humi=dht.readHumidity();float tempC=readTemperature();
      int tem=(float)DHT11.temperature;        //将温度值赋值给tem
      int hum=(float)DHT11.humidity; 
      lcd.setCursor(0, 0);lcd.print("Temp:");lcd.print((float)DHT11.temperature,2);lcd.write(0xDF);lcd.print("C    ");
      lcd.setCursor(0, 1); lcd.print("Humidity:"); lcd.print((float)DHT11.humidity,2); lcd.print("%    ");                   

   if(irrecv.decode(&results)) {   // 解码成功，把数据放入results.value变量中
     if(results.value==KEY_CODE_ONE){
            myservo.write(pos=90);digitalWrite(redPin,200);digitalWrite(greenPin,0);
            delay(600);
            myservo.write(pos=0);digitalWrite(greenPin,200);digitalWrite(redPin,0);
      }
      irrecv.resume();    // 继续等待接收下一组信号 
    }if(results.value==0x16) 
     delay(600);
         

  
  
 
}//这是loop部分（这个括号一定不能删）  
