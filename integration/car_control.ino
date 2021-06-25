#include <Wire.h>
#include <LSM303.h>
#include <math.h>
LSM303 compass;
char pwma=5;
char pwmb=9;
int p1=4;
int p2=8;
int p3=10;
int p4=12;

int p5=7;
int p6=13;


int trigPin2 = 7;
int echoPin2 = 13;
int trigPin1 = 2;
int echoPin1 = 3;

int M=1;
int dd;

float mx2,my2,mz2,mx0,my0,mx1,my1; //mx2,my2: present; mx1,my1: initial; mx0,my0: interference;

float kp=0.3;
float theta_output;

//unsigned long SMillis=millis(),PMillis=millis();

void comp(){
  compass.read();
  mx2 = compass.m.x; 
  my2 = compass.m.y;
  mz2 = compass.m.z;
}

long sound1(){
  long duration;
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(200);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(400);
  digitalWrite(trigPin1, LOW); 
  duration = pulseIn(echoPin1, HIGH);
  return (duration/2) / 29.1;
 }

long sound2(){
  long duration;
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(200);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(400);
  digitalWrite(trigPin2, LOW); 
  duration = pulseIn(echoPin2, HIGH);
  return (duration/2) / 29.1;
 }


int get_state(){
  int dd;
  int s1=digitalRead(p5);
  int s2=digitalRead(p6);
  //int s3=digitalRead(p7);
  dd=s1*2+s2;
  return dd;
}
void set_state(int S){
  int s3=S%2;
  S/=2;
  int s2=S%2;
  S/=2;
  int s1=S%2;
  if (s1==0) digitalWrite(p5,LOW);
  else digitalWrite(p5,HIGH);
  if (s2==0) digitalWrite(p6,LOW);
  else digitalWrite(p6,HIGH);
  //if (s3==0) digitalWrite (p7,LOW);
  //else digitalWrite(p7,HIGH);
}

void car(float v1,float v2){
  if (v1<0){
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
  if (abs(v1)>255) v1=255;
  if (abs(v2)>255) v2=255;
  analogWrite (pwma,abs(v2));
  analogWrite (pwmb,abs(v1));
}

void init1 (){
  float mol3=0,mol4=1000000;
  float mx3,my3,mx4,my4;
  unsigned long SMillis=millis(),PMillis=millis();
  comp();
  mx1=mx2;
  my1=my2; //intial geomagnetic
  while (PMillis-SMillis<5000){
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
    car(120,-120);
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
  pinMode (pwma,OUTPUT);
  pinMode (pwmb,OUTPUT);
  pinMode (p1,OUTPUT);
  pinMode (p2,OUTPUT);
  pinMode (p3,OUTPUT);
  pinMode (p4,OUTPUT);
  pinMode (p5,INPUT);
  pinMode (p6,INPUT);
  pinMode (p7,OUTPUT);
  pinMode (p8,OUTPUT);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  tone (pwma,1000);
  tone (pwmb,1000);
  analogWrite (pwma,0);
  analogWrite (pwmb,0);
  Serial.begin(9600);
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
  ang1=get_angle(mx1-mx0,my1-my0);
  float err;
  unsigned long SMillis,PMillis;
  SMillis=millis();
  PMillis=millis();
  while (PMillis-SMillis<5000){
     comp();
     ang2=get_angle(mx2-mx0,my2-my0);
     err=get_err ((ang1+ang)%360,ang2);
     theta_output=pid (err);
     car (10*theta_output,-10*theta_output);
     PMillis=millis();
  }
  car(0,0);
}

void run_straight(int ang1){
  int ang2;
  float err;
  comp();
  ang2=get_angle(mx2-mx0,my2-my0);
  err=get_err ((ang1)%360,ang2);
  theta_output=pid (err);
  car (100+10*theta_output,100-10*theta_output);
}


void cross_bridge(){
  int ang1;
  float err;
  int d1;
  comp();
  mx1=mx2;
  my1=my2;
  ang1=get_angle(mx1-mx0,my1-my0);
  d1=40;
  int d=sound1();
  while (d>d1){
    run_straight(ang1);
    d=sound1();
    Serial.println(ang1,get_angle(mx2-mx0,my2-my0));
  }
  car(0,0);
  delay(1000);
  mx1=mx2;
  my1=my2;
  ang1=get_angle(mx1-mx0,my1-my0);
  //init1();
  turn_angle(90);
  delay(1000);
  comp();
  mx1=mx2;
  my1=my2;
  ang1=get_angle(mx1-mx0,my1-my0);
  d1=40;
  d=sound1();
  while (sound1()>d1){
    run_straight(ang1);
    //d=sound1();ll
    Serial.print(d );
    Serial.println(get_angle(mx2-mx0,my2-my0));
   }
  car(0,0);
  //delay(1000);
  mx1=mx2;
  my1=my2;
  ang1=get_angle(mx1-mx0,my1-my0);
  turn_angle(-90);
  car(0,0);
  //delay(1000);
  comp();
  mx1=mx2;
  my1=my2;
  ang1=get_angle(mx1-mx0,my1-my0);
  unsigned long SMillis,PMillis;
  SMillis=millis();
  PMillis=SMillis;
  while (PMillis-SMillis<3000){
    run_straight(ang1);
    PMillis=millis();
    Serial.println(d);
  }
  car(0,0);
}


void run_planter(){
  int ang1;
  float err;
  int d1,d2;
  //turn_angle(90);
  comp();
  mx1=mx2;
  my1=my2;
  ang1=get_angle(mx1-mx0,my1-my0);
  d1=20;
  while (sound1()>d1){
    run_straight(ang1);
  }
  car(0,0);
  delay(500);
  turn_angle(90);
  delay(1000);
  comp();
  mx1=mx2;
  my1=my2;
  ang1=get_angle(mx1-mx0,my1-my0);
  d2=sound2()+20;
  while (sound2()<d2){
    run_straight(ang1);
  }
  car(0,0);
  delay(500);
  turn_angle(-90);
  delay(1000);
  comp();
  mx1=mx2;
  my1=my2;
  ang1=get_angle(mx1-mx0,my1-my0);
  d2=sound2()+20;
  while (sound2()<d2){
    run_straight(ang1);
  }
  car(0,0);
  delay(500);
  turn_angle(90);
  delay(1000);
  comp();
  mx1=mx2;
  my1=my2;
  ang1=get_angle(mx1-mx0,my1-my0);
  d1=20;
  while (sound1()>d1){
    run_straight(ang1);
  }
  car(0,0);
  delay(500);
  turn_angle(90);
  delay(1000);
  comp();
  mx1=mx2;
  my1=my2;
  ang1=get_angle(mx1-mx0,my1-my0);
  d1=sound2()+10;
  while (sound1()>d1){
    run_straight(ang1);
  }
  car(0,0);
  delay(500);
  turn_angle(90);
  delay(1000);
  fishfeed();
  comp();
  mx1=mx2;
  my1=my2;
  ang1=get_angle(mx1-mx0,my1-my0);
  d1=20;
  while(sound1()>d1){
    run_straight(ang1);
  }
  car(0,0);
  delay(500);
  turn_angle(90);
  delay(1000);
  comp();
  mx1=mx2;
  my1=my2;
  ang1=get_angle(mx1-mx0,my1-my0);
  d2=sound2()+20;
  while(sound2()<d2){
    run_straight(ang1);
  }
  car(0,0);
  delay(500);
  turn_angle(-90);
  delay(1000);
  time_transmit();
  comp();
  mx1=mx2;
  my1=my2;
  ang1=get_angle(mx1-mx0,my1-my0);
  d2=sound2()+20;
  while (sound2()<d2){
    run_straight(ang1);
  }
  car(0,0);
}

void run_red(){
  //unsigned long PMillis,SMillis;
  //SMillis=millis();
  //PMillis=SMillis;
  
  int ang1,ang2;
  float err;
  turn_angle(-45);
  comp();
  mx1=mx2;
  my1=my2;
  ang1=get_angle(mx1-mx0,my1-my0);
  while (PMillis-SMillis<5000){
    run_straight(ang1);
  }
  turn_angle(-45);
  car(0,0);
}

void run_green(){
  //unsigned long PMillis,SMillis;
  SMillis=millis();
  PMillis=SMillis;
  int ang1,ang2;
  float err;
  turn_angle(-90);
  comp();
  mx1=mx2;
  my1=my2;
  ang1=get_angle(mx1-mx0,my1-my0);
  while (PMillis-SMillis<5000){
    run_straight(ang1);
  }
  car(0,0);
}

void run_blue(){
  //unsigned long PMillis,SMillis;
  SMillis=millis();
  PMillis=SMillis;
  int ang1,ang2;
  float err;
  turn_angle(-135);
  comp();
  mx1=mx2;
  my1=my2;
  ang1=get_angle(mx1-mx0,my1-my0);
  while (PMillis-SMillis<5000){
    run_straight(ang1);
  }
  turn_angle(45);
  car(0,0);
}*/

void loop() {
  dd=get_state();
   get_state();
   if(M==0){ //state 0: stop and wait for respirry
      car(0,0);
    }
   if(M==1){ //state 1: cross the bridge and the gate
     delay(1000);
     init1();
     turn_angle(0);
     cross_bridge();
     M=0;
     set_state(1);//pi active
   }
   
   if (M==2){ //state 2: go straight until color blobs
     mx1=mx2;
     my1=my2;
     int ang1=get_angle(mx1-mx0,my1-my0);
     dd=get_state();
     while (dd==0){
         run_straight(ang1);
         //M=get_state();
         dd=get_state();
     }
     M=dd;
     set_state(1);//Pi active
   }
   if (M==3){ //state 3:red color 1
    mx1=mx2;
    my1=my2;
    int ang1=get_angle(mx1-mx0,my1-my0);
    turn_angle (-45);
    set_state(1); //Pi active
    M=6;
    }
  if (M==4){//state 4: green color 1
    mx1=mx2;
    my1=my2;
    int ang1=get_angle(mx1-mx0,my1-my0);
    turn_angle(-90);
    set_state(2);//Pi rest
    M=7;
  }
  if (M==5){//state 5: blue color 1;
    mx1=mx2;
    my1=my2;
    int ang1=get_angle(mx1-mx0,my1-my0);
    turn_angle(-135);
    set_state(1);//pi active
    M=6;
  }
  if (M==6){ //state 6: red or blue color 2;
     mx1=mx2;
     my1=my2;
     dd=get_state();
     int ang1=get_angle(mx1-mx0,my1-my0);
     while (dd==0){
       run_straight(ang1);
       dd=get_state();
    }
    if (dd==3){
       mx1=mx2;
       my1=my2;
       int ang1=get_angle(mx1-mx0,my1-my0);
       turn_angle (-45);
       set_state(2);
       M=7;
    }
    if (dd==4){
       mx1=mx2;
       my1=my2;
       int ang1=get_angle(mx1-mx0,my1-my0);
       turn_angle (45);
       set_state(2);
       M=7;
    }
  }
  set_state();
  if(M==7){//state 6: run planter and feed the fish and go back
    init1();
    mx1=mx2;
    my1=my2;
    int ang1=get_angle(mx1-mx0,my1-my0);
    run_planter();
    M=0;
    set_state(2);
  }
}
