/*
Author: Yuntao Xie, Qian Wang, Tianqi Wang
*/
#include <Arduino.h>
#include "DS1302.h"
#include <Servo.h> 

Servo myservo; //Create servo control object
HardwareSerial Serial1(PA10, PA9); //create bluetooth serial port
int pos = 0 ;//Init the start position of servo
int t =0; //Init the running time of servo
bool Always=false;                
String disp_time="";
//define the pins to be used
int Pin1 = 0; // PIN1 is used to control the feeding of the servo, and the feeding program is executed when a high level is detected, twice in total
int Pin2 = 0;   // Pin2 is used to control the Bluetooth transmission, when a high level is detected, Bluetooth transmission is executed

int val1 = 0;     // variable to store the read value
int val2 = 0;     // variable to store the read value

void setup()
{
 
  DS1302_Init();
  Serial.begin(9600);
  Serial1.begin(9600);  
  Serial.println("Welcome!");  
  DS1302_GetTime(&DS1302Buffer);
  Display_RTCC();
   pinMode(Pin1, INPUT);      // sets the digital pin as input
  pinMode(Pin2, INPUT);      // sets the digital pin as input
  //Serial.println("Type \"help\" To Get The Help Information.");
}

//显示时间
void Display_RTCC()
{
  
  if(DS1302Buffer.Year < 10)
  {
    Serial.print("200");
    
  }
  else
  {
    Serial.print("20");
    
  }
  Serial.print(DS1302Buffer.Year);
  
  Serial.print('-');
  Serial.print(DS1302Buffer.Month);
  
  Serial.print('-');
  Serial.print(DS1302Buffer.Day);
  disp_time="20"+String(DS1302Buffer.Year)+"-"+String(DS1302Buffer.Month)+"-"+String(DS1302Buffer.Day);
  Serial.print("   ");

  switch(DS1302Buffer.Week)
  {
    case 1:
        Serial.println("Mon");         //Display Monday
        disp_time=disp_time+"   Mon";
        break;
    case 2:
        Serial.println("Tue");         //Display Tuesday
        disp_time=disp_time+"   Tue";
        break;
    case 3:
        Serial.println("Wed");         //Display Wed
        disp_time=disp_time+"   Wed";
        break;
    case 4:
        Serial.println("Thu");         //Display Thu
        disp_time=disp_time+"   Thu";
        break;
    case 5:
        Serial.println("Fri");         //Display Fri
        disp_time=disp_time+"   Fri";
        break;
    case 6:
        Serial.println("Sat");         //Display Sat
        disp_time=disp_time+"   Sat";
        break;
    case 7:
        Serial.println("Sun");         //Display Sun
        disp_time=disp_time+"   Sun";
        break;
    default : break;            
  }

  Serial.print(DS1302Buffer.Hour);
  disp_time=disp_time+"   Time:"+String(DS1302Buffer.Hour)+":"+String(DS1302Buffer.Minute)+":"+String(DS1302Buffer.Second);
  Serial.print(':');
  Serial.print(DS1302Buffer.Minute);
  Serial.print(':');
  Serial.println(DS1302Buffer.Second);
  Serial1.println("TDPS Group 16 IntelliV");
  Serial1.println("Group Member:");
  Serial1.println("Yuntao Xie 2018190604013 2429294x");
  Serial1.println("Han Wang 2018190604024 2429305w");
  Serial1.println("Mingyan Lu  2018190604016 2429297L");
  Serial1.println("Haoyu Ma 2018190604022 2429303M");
  Serial1.println("Zilong Hu 2018190604020 2429301H");
  Serial1.println("Jiayu Wu 2018190604012 2429293w");
  Serial1.println("Qian Wang 2018190604007 2429288W");
  Serial1.println("Tianqi Wang 2018190502027 2429500W");
  Serial1.println("Lang Qin 2018190604028 2429309Q");
  Serial1.println("Qingyuan Fang 2018190604011 2429292f");
  Serial1.println(disp_time);
  Serial.println(disp_time);
}


//时间设置函数
void Set_Time(String &command)
{
  DS1302_ON_OFF(0);     //关闭振荡
  DS1302Buffer.Year=command.substring(4,6).toInt();
  DS1302Buffer.Month=command.substring(7,9).toInt();
  DS1302Buffer.Day=command.substring(10,12).toInt();
  DS1302Buffer.Week=command.substring(13,14).toInt();
  DS1302Buffer.Hour=command.substring(15,17).toInt();
  DS1302Buffer.Minute=command.substring(18).toInt();
  DS1302_SetTime( DS1302_YEAR , DS1302Buffer.Year );
  DS1302_SetTime( DS1302_MONTH , DS1302Buffer.Month );
  DS1302_SetTime( DS1302_DAY , DS1302Buffer.Day );
  DS1302_SetTime( DS1302_WEEK , DS1302Buffer.Week );
  DS1302_SetTime( DS1302_HOUR , DS1302Buffer.Hour );
  DS1302_SetTime( DS1302_MINUTE , DS1302Buffer.Minute );
  DS1302_ON_OFF(1);
  Always=false;
  Serial.println("Set Done");
  Display_RTCC();   
}



void loop()
{
  
  val1 = digitalRead(Pin1);   // read the input pin
  val2 = digitalRead(Pin2);   // read the input pin
  if (val1==1&&t==0){
    for(pos=0; pos <135; pos +=1)  // goes from 0 degrees to 180 degrees 
      {                                  // in steps of 1 degree 
        myservo.write(pos);              // tell servo to go to position in variable 'pos' 
        delay(7);                       // waits 15ms for the servo to reach the position 
                     // waits 15ms for the servo to reach the position 
      } 
      
    for(pos = 135; pos>=1; pos-=1)     // goes from 180 degrees to 0 degrees 
      {                                
        myservo.write(pos);              // tell servo to go to position in variable 'pos' 
        delay(7);       
      } 

      t++;
    
  }else if (val2==1){
  DS1302_GetTime(&DS1302Buffer);        //Get time
  if(Always)
  {
    if(Flag_Time_Refresh == 1)
      {
          Flag_Time_Refresh = 0;
          Display_RTCC();
      }
  }
  }

  

}
