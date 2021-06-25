#include <Wire.h>
#include <LSM303.h>
#include <math.h>
LSM303 compass;
char pwma=9;
char pwmb=5;
char p1=4;
char p2=8;
char p3=10;
char p4=12;
 int i=0;
  int j=2;
float mx2,my2,mz2,mx0,my0,mx1,my1; //mx2,my2: present; mx1,my1: initial; mx0,my0: interference;

float kp=0.3;
float theta_output;
unsigned long SMillis=millis(),PMillis=millis();
 char M='1';
void comp(){
  compass.read();
  mx2 = compass.m.x; 
  my2 = compass.m.y;
  mz2 = compass.m.z;
}

void car(float v1,float v2){
  if (v1>0){
    digitalWrite (p3,LOW);
    digitalWrite (p4,HIGH);
  }
  else{
    digitalWrite (p4,LOW);
    digitalWrite (p3,HIGH);
  }
  if (v2>0){
    digitalWrite (p1,LOW);
    digitalWrite (p2,HIGH);
  }
  else{
    digitalWrite (p2,LOW);
    digitalWrite (p1,HIGH);
  }
  analogWrite (pwma,abs(v2));
  analogWrite (pwmb,abs(v1));
}

void init1 (){
  float mol3=0,mol4=1000000;
  float mx3,my3,mx4,my4;
  //unsigned long SMillis=millis(),PMillis=millis();
  comp();
  mx1=mx2;
  my1=my2;
  while (PMillis-SMillis<10000){
    if ((mx2*mx2+my2*my2)>mol3){
      mol3=mx2*mx2+my2*my2;
      mx3=mx2;
      my3=my2;
    }
    if ((mx2*mx2+my2*my2)<mol4){
      mol4=mx2*mx2+my2*my2;
      mx4=mx2;
      my4=my2;
    }
    car(100,-100);
    comp();
    PMillis=millis();
  //  Serial.print("mx2 ");
  //Serial.print(mx2);
 // Serial.print("my2 ");
 // Serial.print(my2);
//  Serial.print ("time ");
//  Serial.println(PMillis);
  }
  mx0=0.5*mx3+0.5*mx4;
  my0=0.5*my3+0.5*my4;
  //Serial.print("mx0 ");
  //Serial.print(mx0);
 // Serial.print("my0 ");
 // Serial.print(my0);
  //Serial.print("mx1 ");
 // Serial.print(mx1);
 // Serial.print("my1 ");
  //Serial.println(my1);
}


void setup() {
  // put your setup code here, to run once:
  pinMode (pwma,OUTPUT);
  pinMode (pwmb,OUTPUT);
  tone (pwma,1000);
  tone (pwmb,1000);
  analogWrite (pwma,0);
  analogWrite (pwmb,0);
  //Serial.begin(9600);
  Wire.begin();
  compass.init();
  compass.enableDefault();
  //init1();
}



float get_err (float ang1,float ang2){
  if (abs(ang1-ang2)>180){
    if (ang1-ang2>0) return ang1-ang2-360;
    else return ang1-ang2+360;
  }
  else return ang1-ang2;
}

float pid (float err){
  return kp*err;
}

int get_angle(float mx,float my){
  float head_angle;
  if(mx<0){
         head_angle= (atan (my/mx)/M_PI)*180+180;
             }
    else if(mx>0 && my>0)     {
         head_angle= (atan (my/mx)/M_PI)*180;
                                   }
    else if (mx>0 && my<0){
         head_angle= (atan (my/mx)/M_PI)*180+360;   
         }
    else if (mx==0 && my>0){
          head_angle=90;
    }
    else if (mx==0 && my<0){
          head_angle=270;
    }
    else if (my==0 && mx<0){
          head_angle=180;
    }
    else if (my==0 && mx>0){
          head_angle=0;
    }
    return int (head_angle);
}

void turn_angle(int ang){
  int ang1,ang2;
    init1();
    ang1=get_angle(mx1-mx0,my1-my0);
    SMillis=millis();
    PMillis=millis();
    while (PMillis-SMillis<10000){
       comp();
       ang2=get_angle(mx2-mx0,my2-my0);
       float err=get_err ((ang1+ang)%360,ang2);
       theta_output=pid (err);
       car (18*theta_output,-18*theta_output);
       PMillis=millis();
    }
}

void loop() {
  if(M=='1'){
    delay(2000);
  }
  int ang1,ang2;
  // put your main code here, to run repeatedly:
    //char M;//信号
   //while(Serial.available()>0)
   //{
   // M=Serial.read();//接受指令切换模式
   //}
   if(M=='1')
  {
     turn_angle(90);
  // Serial.print("ang2 ");
  // Serial.println(ang2);
 //  Serial.print("err ");
 //  Serial.println(err);
 //  delay (10);
     car(0,0);
     M='2';
    //Serial.print("3");
    }
  //  Serial.print("ang1+90 ");
 //  Serial.println(ang1+90);
  //else{
  //car(100,100);
  //delay (100);
  //car(0,0);
  //delay (400);
//}
  if(M=='2'){
    comp();
    mx1=mx2;
    my1=my2;
    while (M=='2'){
        comp();
        ang1=get_angle(mx1-mx0,my1-my0);
        ang2=get_angle(mx2-mx0,my2-my0);
        float err=get_err ((ang1)%360,ang2);
        theta_output=pid (err);
        car (80+10*theta_output,80-10*theta_output);
    }
  //  Serial.print('3');
  }
  if (M=='3'){
    car(0,0);
  }
  else{
    delay(10);
    }
}
