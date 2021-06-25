/*
Author: Yuntao Xie
*/
#include<ESP8266WiFi.h>
#include<FS.h> //File system
#include"user_interface.h"//system_get_chip_id（）library
#include<ESP8266mDNS.h>
#include <DNSServer.h>
#include<WiFiClient.h>
#include <ESP8266WebServer.h>
//#include <SoftwareSerial.h>

//Storage SN
String SN;
String Mode="1";
int issetup=0;
char ssid[50]="";//SSID
char password[50]="";//Password

const int httpPort = 80;  // http port
ESP8266WebServer server(80);//Declare http server port 80
IPAddress apIP(192, 168, 1, 1);
const byte DNS_PORT = 53;//Declare DNS server port 53

const int state = D1;

int upState = 1;

DNSServer dnsServer;

void setupConfig(void);
void Server_int(void);
void handleMain(void);
void get_espid(void);

void moduleStatus(void);
void changeMode(void);
void motorSpeed(void);
void remoteControl(void);
void handleNotFound(void);

String getContentType(String);
String toStringIp(IPAddress);

void setup(){
Serial.begin(9600);

pinMode(LED_BUILTIN, OUTPUT);

pinMode(state, OUTPUT);

SPIFFS.begin();
delay(1000);//wait for the module become stable
//if(!autoConfig()){
setupConfig();

delay(1000);

ESP.wdtEnable(5000);
}
void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
  if(issetup==1){
  delay(5000);}
}

void setupConfig(){
WiFi.mode(WIFI_AP);
delay(2000);
//Serial.println("setupconfigproccess");
//WiFi.mode(WIFI_AP);
WiFi.softAPConfig(apIP,apIP,IPAddress(255,255,255,0));
WiFi.softAP("IntelliV_"+(String)ESP.getChipId(),"tdps");
dnsServer.start(DNS_PORT,"*",apIP);
//int_sever
Server_int();
}

void Server_int(){
server.on ("/", handleMain);// bind ‘/’ address to handleMain function
server.on ("/status", HTTP_GET, status);
server.on ("/getModule", HTTP_GET, moduleStatus);
server.on ("/changeMode", HTTP_GET, changeMode);
server.on ("/motorSpeed", HTTP_GET, motorSpeed);
server.onNotFound ( handleNotFound ); // handle 404 NotFound
server.begin();
//Serial.println ( "HTTP server started" );
}

void handleMain(){
File file=SPIFFS.open("/index.html","r");
size_t sent=server.streamFile(file, "text/html");
file.close();
return;
}

void status(){
String status_result="SN: "+(String)ESP.getChipId()+" Mode: "+Mode;
server.send(200,"text/plain",status_result);
}

void moduleStatus(){
String serialResult="";
Serial.print("c");
if(Serial.available()>0) {
serialResult+=Serial.read();
}
Serial.print(serialResult);
server.send(200,"text/plain",serialResult);

}

void changeMode(){
if(server.hasArg("mode")){// check http get Argument 
Mode = server.arg("mode");
Serial.print("M"+Mode);
server.send(200,"text/plain","Change Mode "+Mode);
}else{
  server.send(404,"text/plain","Missing Argument: Mode");
}

}

void motorSpeed(){
String serialResult="";
while (Serial.available()>0) {
serialResult+=Serial.read();
}
server.send(200,"text/plain",serialResult);
}

void remoteControl(){

if(server.hasArg("left")&&server.hasArg("right")){// check http get Argument 
String left = server.arg("left");
String right = server.arg("right");
Serial.print(left+","+right);
server.send(200,"text/plain","S");
}else{
server.send(404,"text/plain","Missing Argument");
}
}

void handleNotFound(){
String path=server.uri();
//Serial.print("load url:");
//Serial.println(path);
String contentType=getContentType(path);
if(SPIFFS.exists(path)){
//Serial.print("|file_exist|filetype:"+contentType);
File file=SPIFFS.open(path,"r");
size_t sent=server.streamFile(file,contentType);
file.close();
//ESP.wdtFeed();
return;
}
server.sendHeader("Location", String("http://") + toStringIp(server.client().localIP()), true);
server.send(302, "text/plain", "");   // Empty content inhibits Content-length header so we have to close the socket ourselves.
server.client().stop(); // Stop is needed because we sent no content length
}

//Resolve request files
String getContentType(String filename){
if(server.hasArg("download")) return "application/octet-stream";
else if(filename.endsWith(".htm")) return "text/html";
else if(filename.endsWith(".html")) return "text/html";
else if(filename.endsWith(".css")) return "text/css";
else if(filename.endsWith(".js")) return "application/javascript";
else if(filename.endsWith(".png")) return "image/png";
else if(filename.endsWith(".gif")) return "image/gif";
else if(filename.endsWith(".jpg")) return "image/jpeg";
else if(filename.endsWith(".ico")) return "image/x-icon";
else if(filename.endsWith(".xml")) return "text/xml";
else if(filename.endsWith(".pdf")) return "application/x-pdf";
else if(filename.endsWith(".zip")) return "application/x-zip";
return "text/plain";
}

void get_espid(){
SN=(String)system_get_chip_id();
//Serial.println(SN);
}
String toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}
