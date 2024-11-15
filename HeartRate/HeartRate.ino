#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "spo2_algorithm.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Inisialisasi sensor MAX30105
MAX30105 particleSensor;

// Inisialisasi OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Variabel untuk detak jantung
const byte RATE_SIZE = 4; // Ukuran buffer untuk menyimpan data detak jantung
byte rates[RATE_SIZE]; // Buffer untuk menyimpan data detak jantung
byte rateSpot = 0;
long lastBeat = 0; // Waktu detak jantung terakhir dalam milidetik

float beatsPerMinute;
int beatAvg;

// Variabel untuk SpO2
uint32_t irBuffer[100]; // Buffer untuk IR LED
uint32_t redBuffer[100]; // Buffer untuk Red LED
int32_t bufferLength = 100; // Panjang buffer
int32_t spo2; // Nilai SpO2
int8_t validSPO2; // Validasi nilai SpO2
int32_t heartRate; // Nilai detak jantung
int8_t validHeartRate; // Validasi nilai detak jantung

// Variabel untuk rata-rata per menit
const int MINUTE_SIZE = 60; // Ukuran buffer untuk menyimpan data selama satu menit
int minuteRates[MINUTE_SIZE]; // Buffer untuk menyimpan data BPM selama satu menit
int minuteSpot = 0;
long lastMinute = 0;
int minuteAvg = 0;

// Blynk Auth Token
char auth[] = "V9zHpGkEQeRbGt9AoR6f7sMeyIZNTnIM"; // Ganti dengan Auth Token dari aplikasi Blynk

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Redmi note 11"; // Ganti dengan nama jaringan Wi-Fi Anda
char pass[] = "123456789"; // Ganti dengan kata sandi jaringan Wi-Fi Anda

void setup() {
  Serial.begin(115200);

  // Inisialisasi Blynk
  Serial.println("Menghubungkan ke Blynk...");
  Blynk.begin(auth, ssid, pass, "prakitblog.com", 8181);
  Serial.println("Terhubung ke Blynk");

  // Inisialisasi sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 was not found. Please check wiring/power.");
    while (1);
  }
  
  // Setup sensor dengan parameter yang disesuaikan untuk meningkatkan stabilitas
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A); // Set Red LED intensity
  particleSensor.setPulseAmplitudeGreen(0);  // Turn off Green LED
  particleSensor.setPulseAmplitudeIR(0x0A);  // Set IR LED intensity

  // Inisialisasi OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Alamat I2C 0x3C
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.display();
  delay(2000); // Pause for 2 seconds

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Heart Rate & SpO2:");
  display.display();

  Serial.println("Setup complete");
}

void loop() {
  Blynk.run();

  // Pengambilan data
  for (byte i = 0; i < bufferLength; i++) {
    while (particleSensor.available() == false) { // Tunggu data tersedia
      particleSensor.check();
    }
    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample(); // Ambil sampel berikutnya
  }

  // Validasi data
  long sumRed = 0;
  for (byte i = 0; i < bufferLength; i++) {
    sumRed += redBuffer[i];
  }

  if (sumRed < 50000) { // Threshold untuk mendeteksi apakah jari berada di sensor
    Serial.println("No finger detected");
    Blynk.notify("No Finger Detected");
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("No finger");
    display.setCursor(0, 20);
    display.print("detected");
    display.display();
    delay(200);
    return;
  }

  // Hitung nilai SpO2 dan detak jantung
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

  // Filter data detak jantung untuk mengurangi noise
  if (heartRate < 255 && heartRate > 20) { // Valid range for heart rate
    rates[rateSpot++] = (byte)heartRate; // Simpan data
    rateSpot %= RATE_SIZE; // Wrap buffer

    // Hitung rata-rata detak jantung
    beatAvg = 0;
    for (byte x = 0; x < RATE_SIZE; x++) {
      beatAvg += rates[x];
    }
    beatAvg /= RATE_SIZE;

    // Simpan data ke buffer satu menit
    minuteRates[minuteSpot++] = heartRate;
    if (minuteSpot >= MINUTE_SIZE) { // Jika buffer penuh, hitung rata-rata per menit
      minuteAvg = 0;
      for (int x = 0; x < MINUTE_SIZE; x++) {
        minuteAvg += minuteRates[x];
      }
      minuteAvg /= MINUTE_SIZE;
      minuteSpot = 0; // Reset buffer
    }
  } else {
    validHeartRate = 0; // Invalid heart rate
  }

  // Tampilkan nilai BPM dan SpO2 pada OLED dengan teks lebih besar
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Heart Rate & SpO2:");

  display.setTextSize(2); // Set text size to 2 for better readability
  if (validHeartRate) {
    display.setCursor(0, 20);
    display.print("BPM:");
    display.print(beatAvg);
    Blynk.virtualWrite(V1, beatAvg); // Kirim BPM ke Blynk Virtual Pin V1
  } else {
    display.setCursor(0, 20);
    display.print("BPM: --");
    Blynk.virtualWrite(V1, "Invalid"); // Kirim status BPM ke Blynk Virtual Pin V1
  }

  if (validSPO2) {
    display.setCursor(0, 40);
    display.print("SpO2:");
    display.print(spo2);
    Blynk.virtualWrite(V2, spo2); // Kirim SpO2 ke Blynk Virtual Pin V2
  } else {
    display.setCursor(0, 40);
    display.print("SpO2: --");
    Blynk.virtualWrite(V2, "Invalid"); // Kirim status SpO2 ke Blynk Virtual Pin V2
  }

  display.display();
  delay(200); // Delay untuk update data

  // Debugging
  Serial.print("IR=");
  Serial.print(irBuffer[0]);
  Serial.print(", Red=");
  Serial.print(redBuffer[0]);
  Serial.print(", BPM=");
  if (validHeartRate) {
    Serial.print(beatAvg);
  } else {
    Serial.print("Invalid");
  }
  Serial.print(", SpO2=");
  if (validSPO2) {
    Serial.print(spo2);
  } else {
    Serial.print("Invalid");
  }
  Serial.print(", Avg BPM=");
  Serial.print(minuteAvg);
  Serial.println();
}