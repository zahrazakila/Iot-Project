#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Alamat default untuk modul LCD I2C 16x2 biasanya adalah 0x27 atau 0x3F
#define I2C_ADDR 0x27 
#define LCD_COLS 16
#define LCD_ROWS 2

// Inisialisasi obyek LiquidCrystal_I2C
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLS, LCD_ROWS);

const int pinSensorGempa = 10;
const int pinBuzzer = 9;
boolean bacaSensor;

void setup() {
  // Memulai komunikasi serial
  Serial.begin(115200);

  pinMode(pinSensorGempa, INPUT);
  pinMode(pinBuzzer, OUTPUT);
  digitalWrite(pinBuzzer, LOW);
  
  // Inisialisasi LCD
  lcd.begin(); // Menggunakan init() untuk inisialisasi LCD
  lcd.backlight(); // Menghidupkan backlight LCD
  
  lcd.setCursor(0, 0);
  lcd.print("Alat Pendeteksi");
  lcd.setCursor(0, 1);
  lcd.print("GEMPA...");
  delay(2000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready.....");
  delay(2000);

  // Menampilkan pesan di serial monitor
  Serial.println("Sistem siap, menunggu getaran gempa...");
}

void loop() {
  bacaSensor = digitalRead(pinSensorGempa);

  if (bacaSensor == HIGH) {
    // Meningkatkan volume buzzer (gunakan PWM)
    tone(pinBuzzer, 1000); // 1000 Hz (frekuensi yang bisa disesuaikan)
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Kondisi");
    lcd.setCursor(0, 1);
    lcd.print("BERBAHAYA... !!!");
    delay(2500);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("GEMPA");
    lcd.setCursor(0, 1);
    lcd.print("TERDETEKSI...!!!");
    delay(2500);

    // Menampilkan pesan di serial monitor
    Serial.println("GEMPA TERDETEKSI! Kondisi BERBAHAYA...!!!");

    noTone(pinBuzzer); // Matikan buzzer
  } else {
    noTone(pinBuzzer); // Matikan buzzer
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Kondisi");
    lcd.setCursor(0, 1);
    lcd.print("AMAN.....");
    delay(2000);

    // Menampilkan pesan di serial monitor
    Serial.println("Kondisi AMAN.");
  }
}
