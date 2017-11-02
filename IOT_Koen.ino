#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

const char* ssid    = "Koen";                     //the name of the network
const char* pwd     = "140975vkcqyksn";           //the password of the network you're connecting to
const char* server  = "22005.hosts.ma-cloud.nl";  //the server of the .json file
String path         = "/NodeMCU/Test.json";       //the location of the .json file
static const uint8_t wifiConnecting = D7;
static const uint8_t wifiOk = D6;
boolean debug = true;
String httpResponse;
bool requestRepeat = true;

void setup() {
  Serial.begin(115200);
  pinMode(wifiConnecting, OUTPUT);
  pinMode(wifiOk, OUTPUT);
}

void loop() {
  if(WiFi.status() != WL_CONNECTED){
    wifiConnect();  
  }
  while(requestRepeat){
    httpRequest();
    Serial.println(httpResponse);
    payload();
    requestRepeat = false;
  }
}

void Blink(int led, int Timer){
  digitalWrite(led, HIGH);
  delay(Timer);
  digitalWrite(led, LOW);
  delay(Timer);
}

void httpRequest(){
  delay(2000);
  httpResponse = "";
  WiFiClient client;
  if(client.connect(server, 80)){
      client.println("GET " + path + " HTTP/1.1");
      Serial.println("GET " + path + " HTTP/1.1");
      client.println("Host: " + String(server));
      Serial.println("Host: " + String(server));
      client.println("Connection: keep-alive");
      client.println();
  }  
  else{
    Serial.println("Webserver does not respond");
    return;  
  }

  while(client.connected()){
    while(client.available()){
      //Serial.println("client available");
      Serial.print(client.read());
      httpResponse += char(client.read());
      if(httpResponse.length() > 450){
        Serial.println("Buffer overflow!");
        httpResponse = "";
        return;  
      }
    }  
  }
}

void wifiConnect(){
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.print(ssid);
  WiFi.begin(ssid, pwd);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print("!");
    Blink(wifiConnecting, 50);  
  }
  Serial.println();
  Serial.print("WiFi connected, IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(wifiOk, HIGH);
  
}

void payload(){
  String endHeader = "\r\n\r\n";
  int gotEoH = -1;
  for(int i = 0; i <= httpResponse.length()-endHeader.length(); i++){
    if(httpResponse.substring(i, endHeader.length() + i) == endHeader){
      gotEoH = i;  
    }  
  }
  httpResponse = httpResponse.substring(gotEoH);
}
