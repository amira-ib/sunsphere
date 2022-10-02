#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd( 0x27, 16, 2 );

const char* ssid     = "Amira";
const char* password = "123456789";

const char* host = "services.swpc.noaa.gov"; // only google.com not https://google.com
String datarx;
int pos, start, finish;
String words;
float speeds, temps;

void setup() {
  lcd.init();
  lcd.clear();
  lcd.backlight();
  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  delay(5000);

  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClientSecure client;
  const int httpPort = 443; // 80 is for HTTP / 443 is for HTTPS!
  
  client.setInsecure(); // this is the magical line that makes everything work
  
  if (!client.connect(host, httpPort)) { //works!
    Serial.println("connection failed");
    return;
  }

  // We now create a URI for the request
  String url = "/products/solar-wind/plasma-5-minute.json";



  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");

  Serial.println();
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }
  while (client.available()) {
    datarx += client.readStringUntil('\n');
  }
  Serial.println(datarx);
  pos = 0;
  start, finish;
  for (int i = 0; i < 6; i++)
  {
    pos = datarx.indexOf(",", pos+1);
  }
  start = pos+2;
  pos = datarx.indexOf(",", start);
  finish = pos - 1;
  words = datarx.substring(start, finish);
  speeds = words.toFloat();

  start = finish + 3;
  pos = datarx.indexOf("\"", start);
  finish = pos;
  words = datarx.substring(start, finish);
  temps = words.toFloat();

  Serial.print("temperature = ");
  Serial.println(temps);
  Serial.print("speed = ");
  Serial.println(speeds);
  datarx = "";
  Serial.println("closing connection");
  lcd.setCursor( 2, 0 );
  lcd.print( "temp = " );
  lcd.setCursor( 9, 0 );
  lcd.print(temps);
  lcd.setCursor( 2, 1 );
  lcd.print( "speed = " );
  lcd.setCursor( 10, 1 );
  lcd.print( speeds );
  delay(50000);  
  lcd.clear();
}
