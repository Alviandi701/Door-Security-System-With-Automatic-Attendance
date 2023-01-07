#include <LiquidCrystal_I2C.h>
#include<Wire.h>
#include <Adafruit_Fingerprint.h>
#include <WiFi.h>

#include "DFRobotDFPlayerMini.h"
int relayPin = 18;
int buzzerPin = 14;
const char* NAME;
const char* ID;


String Event_Name = "Fingerprint";

String Key = "dfyNwmcqLAmpJW3qiflmdS";

// Replace with your unique IFTTT URL resource
String resource = "/trigger/" + Event_Name + "/with/key/" + Key;

// Maker Webhooks IFTTT
const char* server = "maker.ifttt.com";

// Replace with your SSID and Password
const char* ssid     = "YAYASAN FAKIR KUOTA";
const char* password = "pocophone";
HardwareSerial Myserial(0); // UART1 (0), UART2 (1), UART3 (2)
DFRobotDFPlayerMini mp3; // Membuat variabel "myDFPlayer"
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);
LiquidCrystal_I2C lcd(0x27, 16, 2); 


void setup()
{
  lcd.begin();
  lcd.backlight();
  lcd.clear(); 
  Serial.begin(115200);
  Myserial.begin(9600, SERIAL_8N1, 4, 2);  //kecepatan, jenis komunikasi, pin RX, TX
    mp3.begin(Myserial);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
      pinMode(relayPin, OUTPUT);
    pinMode(buzzerPin, OUTPUT);
    digitalWrite(relayPin, HIGH);
  Serial.println("\n\nAdafruit finger detect test");
  lcd.setCursor(0,0);
    lcd.print("   Open Logic   ");
    lcd.setCursor(0,1);
    lcd.print("DoorLock finger");
    Serial.println("Fingerprint Door Lock");
        

  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) {
      delay(1);
    }
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  Serial.println("Waiting for valid finger...");

  Serial.print("Connecting to: ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);

  int timeout = 10 * 4; // 10 seconds
  while (WiFi.status() != WL_CONNECTED  && (timeout-- > 0)) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect, going back to sleep");
  }

  Serial.print("WiFi connected in: ");
  Serial.print(millis());
  Serial.print(", IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()                     // run over and over again
{
  getFingerprintIDez();
  if (finger.fingerID == 1) {

    Serial.print("!!--");
    Serial.println(finger.fingerID);
    NAME = "ASTRI";
    ID = "1";
    if (finger.confidence >= 60) {
      Serial.print("Attendace Marked for "); Serial.println(NAME);
      makeIFTTTRequest();
      // digital write - open the door
    }

  }

  if (finger.fingerID == 2 ) {
    Serial.print("!!--");
    Serial.println(finger.fingerID);
    NAME = "ALVIANDI";
    ID = "2";
    if (finger.confidence >= 60) {
      Serial.print("Attendace Marked for "); Serial.println(NAME);
      makeIFTTTRequest();
      // digital write - open the door
    }      //don't ned to run this at full speed.

  }
  finger.fingerID = 0;
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
   mp3.play(1); 
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("   Open Logic   ");
    lcd.setCursor(0,1);
    lcd.print(" Selamat Datang ");
    digitalWrite(relayPin, LOW);
    //digitalWrite(relayPin, HIGH);
    Serial.println("Door Unlocked Welcome");
    delay(3000);
    digitalWrite(relayPin, HIGH);
    lcd.clear();
    lcd.print("   Open Logic   ");
    lcd.setCursor(0,1);
    lcd.print("DoorLock finger");
    Serial.println("The door is locked automatically");
  return finger.fingerID;
}

// Make an HTTP request to the IFTTT web service
void makeIFTTTRequest() {
  Serial.print("Connecting to ");
  Serial.print(server);

  WiFiClient client;
  int retries = 5;
  while (!!!client.connect(server, 80) && (retries-- > 0)) {
    Serial.print(".");
  }
  Serial.println();
  if (!!!client.connected()) {
    Serial.println("Failed to connect...");
  }

  Serial.print("Request resource: ");
  Serial.println(resource);

  // Temperature in Celsius
  String jsonObject = String("{\"value1\":\"") + NAME + "\",\"value2\":\"" + ID
                      + "\"}";

  client.println(String("POST ") + resource + " HTTP/1.1");
  client.println(String("Host: ") + server);
  client.println("Connection: close\r\nContent-Type: application/json");
  client.print("Content-Length: ");
  client.println(jsonObject.length());
  client.println();
  client.println(jsonObject);

  int timeout = 5 * 10; // 5 seconds
  while (!!!client.available() && (timeout-- > 0)) {
    delay(100);
  }
  if (!!!client.available()) {
    Serial.println("No response...");
  }
  while (client.available()) {
    Serial.write(client.read());
  }

  Serial.println("\nclosing connection");
  client.stop();
}
