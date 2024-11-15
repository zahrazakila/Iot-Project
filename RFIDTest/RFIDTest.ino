#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SPI.h>
#include <MFRC522.h>

char auth[] = "TBiJHh_SFTYtrTQaIxsHczSMgjEnGCc3";
char ssid[] = "GENDUT";
char pass[] = "aabbccdd";

#define SS_PIN D2
#define RST_PIN D1

#define RELAY_PIN D3
#define BUZZ_PIN D8

MFRC522 rfid(SS_PIN, RST_PIN);

byte nuidPICC[4];
byte lock;
String ID_TAG;
String ID1 = "b7e8d4b5";
bool wifiConnected = false;
bool blynkConnected = false;

BLYNK_WRITE(V1) {
  lock = param.asInt();
  Serial.print("lock: ");
  Serial.println(lock);
}

void readRFID(byte *buffer, byte bufferSize) {
  ID_TAG = "";
  for (byte i = 0; i < bufferSize; i++) {
    ID_TAG += String(buffer[i], HEX);
  }
  Serial.print("ID_TAG: ");
  Serial.println(ID_TAG);
}

void setup() {
  pinMode(BUZZ_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("RFID Reader initialized.");

  // Coba sambungkan ke WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, pass);

  // Tambahkan timeout untuk koneksi WiFi
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 30000) { // Timeout 30 detik
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed to connect to WiFi");
  }

  if (wifiConnected) {
    // Konfigurasi Blynk
    Blynk.config(auth, "prakitblog.com", 8181);

    // Tambahkan timeout untuk koneksi Blynk
    startAttemptTime = millis();
    while (!Blynk.connect(1000) && millis() - startAttemptTime < 10000) { // Timeout 10 detik
      delay(500);
      Serial.print(".");
    }
    Serial.println();
    if (Blynk.connected()) {
      blynkConnected = true;
      Serial.println("Blynk connected");
    } else {
      Serial.println("Failed to connect to Blynk");
    }
  }

  Serial.println();
  Serial.println("Kunci Brankas");
}
 
void loop() {
  if (wifiConnected && blynkConnected) {
    Blynk.run();
  }

  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }
  
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println("Kartu terdeteksi.");
  readRFID(rfid.uid.uidByte, rfid.uid.size);

  if (ID_TAG == ID1) {
    Serial.println("Kartu Terdaftar");
    Blynk.notify("Brankas diakses ID terdaftar, tekan tombol unlock untuk membuka");
    buzzer(2, 100, 100);
    while (!lock) {
      delay(100);
      Blynk.run();
    }
    buzzer(1, 500, 500);
    digitalWrite(RELAY_PIN, HIGH);
    Blynk.notify("Tekan tombol LOCK untuk mengunci brankas");
    while (lock) {
      delay(100);
      Blynk.run();
    }
    buzzer(1, 500, 500);
    digitalWrite(RELAY_PIN, LOW);
  } else {
    Serial.println("Kartu Salah");
    Blynk.notify("Brankas diakses dengan ID tidak terdaftar");
    buzzer(50, 200, 50);
    delay(3000);
  }

  // Halt PICC to stop it from being read again
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

void buzzer(byte ulang, int jedaOn, int jedaOff) {
  for (int j = 0; j < ulang; j++) {
    digitalWrite(BUZZ_PIN, HIGH);
    delay(jedaOn);
    digitalWrite(BUZZ_PIN, LOW);
    delay(jedaOff);
  }
}