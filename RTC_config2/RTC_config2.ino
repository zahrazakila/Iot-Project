#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc; // Membuat objek RTC_DS3231

void setup() {
  Serial.begin(9600); // Inisialisasi komunikasi serial
  Wire.begin(); // Memulai komunikasi I2C
  
  if (!rtc.begin()) { // Memulai RTC
    Serial.println("Couldn't find RTC");
    while (1);
  }
  
  if (rtc.lostPower()) { // Jika RTC kehilangan daya, set waktu default
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  // Uncomment baris di bawah ini untuk menyetel waktu secara manual
  rtc.adjust(DateTime(2024, 5, 26, 10, 14, 0)); // Tahun, Bulan, Tanggal, Jam, Menit, Detik
}

void loop() {
  DateTime now = rtc.now(); // Mendapatkan waktu saat ini
  
  // Menampilkan waktu di serial monitor
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  
  delay(1000); // Menunggu satu detik
}
