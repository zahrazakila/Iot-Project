#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Ganti alamat I2C jika perlu
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // Inisialisasi LCD
  lcd.begin();

  // Nyalakan backlight
  lcd.backlight();

  // Tampilkan pesan di LCD
  lcd.setCursor(0, 0); // Baris pertama, kolom pertama
  lcd.print("Hello, World!");

  lcd.setCursor(0, 1); // Baris kedua, kolom pertama
  lcd.print("I2C LCD Test");
}

void loop() {
  // Tidak ada yang perlu dilakukan di loop
}
