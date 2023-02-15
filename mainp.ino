#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define SensorPin 0          
unsigned long int avgValue;  
float b;
int buf[10],tempa;

int FloatSensor = D2;
int led = D7;
int buttonState = 1;


#define ONE_WIRE_BUS D4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float temp, phValue;
/*Put your SSID & Password*/
const char ssid[] = "Mohammed";  // Enter SSID here
const char password[] = "smarf0001";  //Enter Password here
WiFiClient client; 
ESP8266WebServer server(80);            
 
void setup() {
  Serial.begin(115200);
  delay(100);
  sensors.begin();
  pinMode(FloatSensor, INPUT_PULLUP);
  pinMode (led, OUTPUT);                
  Serial.println("Connecting to ");
  Serial.println(ssid);
  //connect to your local wi-fi network
  WiFi.begin(ssid, password);
  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/", LecturaTH);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
}
void loop() {
  LecturaTH();
  ShippingData();
  server.handleClient();
}
void LecturaTH(){
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    buf[i]=analogRead(SensorPin);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        tempa=buf[i];
        buf[i]=buf[j];
        buf[j]=tempa;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                     
    avgValue+=buf[i];
  float phValue=(float)avgValue*2.7/1024/6; 
  phValue = 3.5 * phValue;                     
  Serial.println(phValue); 
  buttonState = digitalRead(FloatSensor);

  if (buttonState == HIGH) {
    digitalWrite(led, LOW);
    Serial.println("WATER LEVEL - LOW");
  }
  else {
    digitalWrite(led, HIGH);
    Serial.println("WATER LEVEL - HIGH");
  }  
  server.send(200, "text/html", SendHTML(temp, phValue, buttonState)); 
}       
void handle_NotFound(){
  server.send(404, "text/plain", SendHTML(temp, phValue, buttonState));
}
void ShippingData(){
  if (WiFi.status() == WL_CONNECTED){ 
     HTTPClient http;  
     String data_to_send = "temp=" + String(temp) + "&phValue=" + String(phValue);

     http.begin(client,"http://fishfarm2323.000webhostapp.com/EspPost.php");
     http.addHeader("Content-Type", "application/x-www-form-urlencoded");

     int response_code = http.POST(data_to_send);

     if (response_code>0){
      Serial.println("HTTP code:"+ String(response_code));
        if (response_code == 200){
          String response_code = http.getString();
          Serial.println("The server replied: ");
          Serial.println(response_code);
        }
     } else {
        Serial.print("Error sent POST, code: ");
        Serial.println(response_code);
     }

       http.end();  
       
  } else {
     Serial.println("WIFI connection error");
  }
  delay(1000);
}


String SendHTML(float temp, float phValue, int buttonState){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Monitor</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>Dashbored pond</h1>\n";
  ptr +="<p>Temperature: ";
  ptr +=temp;
  ptr +="&deg;C</p>\n";
  ptr +="<p>PH: "; 
  ptr+=phValue;
  ptr +="<p>WATER LEVEL: \n ";  
  ptr+=buttonState;
  ptr +="<p>Oxygen: NaN \n ";   
  ptr+="<h6>(HIGH = 0 LOW = 1)\n";
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  ptr +="<script>\n";
  ptr +="setInterval(loadDoc,100);\n";
  ptr +="function loadDoc() {\n";
  ptr +="var xhttp = new XMLHttpRequest();\n";
  ptr +="xhttp.onreadystatechange = function() {\n";
  ptr +="if (this.readyState == 4 && this.status == 200) {\n";
  ptr +="document.body.innerHTML =this.responseText}\n";
  ptr +="};\n";
  ptr +="xhttp.open(\"GET\", \"/\", true);\n";
  ptr +="xhttp.send();\n";
  ptr +="}\n";
  ptr +="</script>\n";
  ptr +="<script>\n";
  ptr +="setInterval(loadDoc,1000);\n";
  ptr +="function loadDoc() {\n";
  ptr +="var xhttp = new XMLHttpRequest();\n";
  ptr +="xhttp.onreadystatechange = function() {\n";
  ptr +="if (this.readyState == 4 && this.status == 200) {\n";
  ptr +="document.body.innerHTML =this.responseText}\n";
  ptr +="};\n";  
  ptr +="xhttp.open(\"GET\", \"/\", true);\n";
  ptr +="xhttp.send();\n";
  ptr +="}\n";
  return ptr;
}

