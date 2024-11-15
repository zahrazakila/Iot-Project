#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
 
char auth[] = "YURP6vK6K2XofzjfXUBip3gUpIM_TGHt"; //ganti token
char ssid[] = "GENDUT"; //WiFi
char pass[] = "aabbccdd"; // Password

#define BUZZ D3 
#define MQ2Pin D5
int MQ2Value;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  lcd.begin();
  lcd.backlight();
  lcd.print("Koneksi WiFi");
  lcd.setCursor(0,1);
  lcd.print(ssid);
  delay(10);
  Blynk.begin(auth, ssid, pass);
  lcd.print(" OK! ");
  delay(2000);
  lcd.clear();
  pinMode(MQ2Pin, INPUT);
  pinMode(BUZZ, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();
  MQ2Value = digitalRead(MQ2Pin);
  if(MQ2Value){
    lcd.setCursor(0, 0);
    lcd.print(" LPG GAS LEAK");
    lcd.print("DETECTOR SYSTEM");
    delay(1000);
  }
  else{
    lcd.clear();
    lcd.print(">Gas terdeteksi<");
    Blynk.notify("Kebocoran Gas terdeteksi....!");
    while(!MQ2Value){
      lcd.noBacklight();
      digitalWrite(BUZZ,LOW);
      lcd.backlight();
      delay(200);
      MQ2Value = digitalRead(MQ2Pin);
    }
    lcd.clear();
    delay(1000);
  }
}
