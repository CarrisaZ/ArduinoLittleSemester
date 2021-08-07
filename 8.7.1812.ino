#include <dht11.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.h>
#include <SoftwareSerial.h> 
//蓝牙
SoftwareSerial BT(6,5); 
String v = "";

//温湿度部分
LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display
dht11 DHT11;
#define DHT11PIN 2

//呼吸灯部分
int ledPin = 4;
int incomedate = 0;
int relayPin = 7;         //继电器引脚
int redPin=9;
int greenPin=10;
int bluePin=11;
int recvPin = 3;//定义红外接收器的引脚为3
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

//有无人模式
int pinBuzzer=13;
int pengzhuang=12;
int zhuangtai=0;
int zhuangtai2=0;//0为在家模式 1为外出模式
int PIR_sensor=A3;//<50无人
int val_PIR=0;
int reganying=0;



void setup()
{

//蓝牙模块设置///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
  Serial.begin(9600);
  Serial.println("Hi");
  BT.begin(9600);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  pinMode(3,INPUT);
  pinMode(13,OUTPUT);
  // pinMode(13,OUTPUT);
  pinMode(PIR_sensor,INPUT);
  pinMode(pinBuzzer,OUTPUT);
  pinMode(relayPin, OUTPUT);        //继电器输出模式
  pinMode(pengzhuang,INPUT_PULLUP);
  irrecv.enableIRIn(); // 初始化红外接收器
  
  //温湿度部分
  lcd.init(); // initialize the lcd 
  lcd.backlight();
  lcd.print("Welcom...");
  
  //红外 灯
  pinMode(ledPin, OUTPUT);
  pinMode(redPin,OUTPUT);
  pinMode(greenPin,OUTPUT);
  pinMode(bluePin,OUTPUT);

  Serial.begin(9600); 
 
}

void loop()
{
  if(digitalRead(8)){//倾斜传感器控制蜂鸣器
        delay(100);
        digitalWrite(13,LOW);     
      }
      else{
        digitalWrite(13,HIGH); 
        tone(7,2093); 
    }
  
  //蓝牙输入///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (BT.available()) {
    while (BT.available() > 0)
      {
        v += char(BT.read());
        delay(2);
      }
    Serial.println(v);//蓝牙输入
  }


//密码部分，注释掉了，后续可增加   
// if(v == "123456")//这里是预设的密码，密码正确，
//   {
//   BT.println("welcome");
//   }








  
  int   current_value=0;
  int chk = DHT11.read(DHT11PIN);
  int tem=(float)DHT11.temperature;        //将温度值赋值给tem
  int hum=(float)DHT11.humidity; 

                        lcd.setCursor(0, 0);
                        lcd.print("Temp:");
                        lcd.print((float)DHT11.temperature,2);
                        lcd.write(0xDF);
                        lcd.print("C    ");
                        lcd.setCursor(0, 1);
                        lcd.print("Humidity:");
                        lcd.print((float)DHT11.humidity,2);
                        lcd.print("%    ");
                        Serial.print("Humidity(%):");
   
     if (tem>23)        //如果温度大于23摄氏度
    {
      digitalWrite(relayPin, LOW);        //继电器关，风扇关
      Serial.println(" CLOSE!");
    } else
    {
      digitalWrite(relayPin,HIGH);
      Serial.println("OPEN!");
    }
   
    val_PIR = analogRead(PIR_sensor);
    zhuangtai=digitalRead(pengzhuang);//碰撞开关控制蜂鸣器
    if(zhuangtai2==1){//////////////////////////////////////////////////??????????????
        if(val_PIR>=150){digitalWrite(pinBuzzer,HIGH);Serial.print(zhuangtai2);Serial.print("响声");}//////////////??????????????
        else if(zhuangtai==HIGH){digitalWrite(pinBuzzer,HIGH);}////////////??????????????
        else{digitalWrite(pinBuzzer,LOW);Serial.print(zhuangtai2);Serial.print("外出模式下关门并无人");}
      }else{digitalWrite(pinBuzzer,LOW);Serial.print(zhuangtai2);Serial.print("在家");}


//蓝牙控制



if(v == "1")
  {
    BT.println("tem");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp:");
    lcd.print((float)DHT11.temperature,2);
    lcd.write(0xDF);lcd.print("C    "); //温度
    lcd.setCursor(0, 1); 
    lcd.print("Humidity:");
    lcd.print((float)DHT11.humidity,2);lcd.print("%    "); //湿度
    v = "";//置零
  
   }

  if(v == "2")
     { 
      BT.println("mode choose");
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("Light Controll:");
      lcd.setCursor(0, 1);lcd.print("current: ");delay(3000);
      v = "";//置零 
      }     
  if(v == "21")
         {
          BT.println("mode1");
          lcd.clear();
          lcd.setCursor(0, 0);lcd.print("Light Controll:");
          lcd.setCursor(0, 1);lcd.print("current: 1");
//  lcd.print("Temp:");lcd.print((float)DHT11.temperature,2);lcd.write(0xDF);lcd.print("C");lcd.print("Humidity:");lcd.print((float)DHT11.humidity,2);lcd.print("%");
          digitalWrite(ledPin, HIGH);setColor(255,255,255); delay(3000);
           v = "";//置零
           }
   
  if(v == "22")
        {
          BT.println("mode2");
          lcd.clear();
          lcd.setCursor(0, 0);lcd.print("Light Controll:");
          lcd.setCursor(0, 1);lcd.print("current: 2");
//          lcd.print("Temp:");lcd.print((float)DHT11.temperature,2);lcd.write(0xDF);lcd.print("C");lcd.print("Humidity:");lcd.print((float)DHT11.humidity,2);lcd.print("%");
          analogWrite(ledPin,50);setColor(100,55,55);delay(3000);
         
          v = "";//置零
         }
  if(v == "23")
        {
         BT.println("mode3");
         lcd.clear();
         lcd.setCursor(0, 0);lcd.print("Light Controll:");
         lcd.setCursor(0, 1);lcd.print("current: 3");
//         lcd.print("Temp:");lcd.print((float)DHT11.temperature,2);lcd.write(0xDF);lcd.print("C");lcd.print("Humidity:");lcd.print((float)DHT11.humidity,2);lcd.print("%");
         analogWrite(ledPin,10);setColor(25,25,25);
         
          v = "";//置零
         }
  if(v == "20")
        {
         BT.println("mode4");
         lcd.clear();
         lcd.setCursor(0, 0);lcd.print("Light Controll:");
         lcd.setCursor(0, 1);lcd.print("current: 4");lcd.print("Temp:");
//         lcd.print((float)DHT11.temperature,2);lcd.write(0xDF);lcd.print("C");lcd.print("Humidity:");lcd.print((float)DHT11.humidity,2);lcd.print("%");
         ledOff(ledPin); setColor(0,0,0);delay(3000);
         
          v = "";//置零
         }







         
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         
 if(v=="3"){
             BT.println("outing status");
          if(zhuangtai2==1){BT.println("is outing ");}
            else if(zhuangtai2==0){BT.println("is in home ");}
 v = "";}//////////////////////////??????????????????
    if(v=="30"){BT.println("alter state to in home");zhuangtai2=0;v = "";}//////////////////////////??????????????????
    if(v=="31"){BT.println("alter state to go out");zhuangtai2=1;v = "";}//////////////////////////??????????????????
     if(v=="4"){BT.println("check door status");
        if(zhuangtai==HIGH){return "door open";}
        else if(zhuangtai==LOW){return "door close";}
        v = "";}//////////////////////////??????????????????
    
         
         
    
        
     
  






//这是红外部分被我注释掉了
/////irrecv.resume(); // 接收下一个值
//     
//     ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
// if (irrecv.decode(&results)) {
//   long irr=results.value;
//    int current_value=0;
//    Serial.print("in 1");
//    
//        switch (results.value) {
//                
//                case 0xFF629D:////////////////////////////////按下2键的事件
//                      lcd.clear();
//                     lcd.setCursor(0, 0);lcd.print("Door:");if(zhuangtai==HIGH){lcd.print("   open  ");}else{lcd.print("  close  ");}//////////////////////////////////////////////////////
//                       lcd.setCursor(0, 1);lcd.print("Status:");if(zhuangtai2==0){lcd.print("   home ");}else{lcd.print("  out  ");}//////////////////////////////////////////////////////
//                       delay(2000);
//                break;
//            
//            } //这是switchcase的
//
//             irrecv.resume();         
//            
//        }//这是包含2个选项的if的
// 




 
}//这是loop部分（这个括号一定不能删）  


      
int men(int zhuangtai){
  if(zhuangtai==HIGH){return "door open";}
  else if(zhuangtai==LOW){return "door close";}
} 
       

int waichu(int zhuangtai2){
  if(zhuangtai2==1){return "status is go out";}
  else if(zhuangtai2==0){return "status is in home";}
}

void setColor(int redValue,int greenValue,int blueValue){
  analogWrite(redPin,redValue);
  analogWrite(greenPin,greenValue);
  analogWrite(bluePin,blueValue); 
 }
// led亮灯
void ledOn(int ledPin) {
  digitalWrite(ledPin, HIGH);
}//led闪烁
void flash(int ledPin, long highDuration, long lowDuration) {
  digitalWrite(ledPin, HIGH);
  delay(highDuration);
  digitalWrite(ledPin, LOW);
  delay(lowDuration);
}
//呼吸灯On
void fadeOn(unsigned int time, int increament) {
  for (byte value = 0 ; value < 255; value += increament) {
    //Serial.println(value);
    analogWrite(ledPin, value);
    delay(time / (255 / 5));
  }
}
//呼吸灯Off
void fadeOff(unsigned int time, int decreament) {
  for (byte value = 255; value > 0; value -= decreament) {
    //Serial.println(value);
    analogWrite(ledPin, value);
    delay(time / (255 / 5));
  }
}
// led熄灯
void ledOff(int ledPin) {
  digitalWrite(ledPin, LOW);
}
