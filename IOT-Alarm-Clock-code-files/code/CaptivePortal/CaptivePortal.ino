#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <RtcDS3231.h>
#include <Wire.h>

// Menginisialisasi RTC menggunakan library RtcDS3231
RtcDS3231<TwoWire> Rtc(Wire);

// Konfigurasi DNS
const byte DNS_PORT = 53;
IPAddress apIP(10, 10, 10, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

// Definisi pin untuk buzzer dan LED
#define buzzer D8
#define LED_PIN D7
#define BUTTON_PIN D6

String hr, minut; // Variabel untuk menyimpan jam dan menit alarm
int hr1, minut1;
String new_time;

// Status alarm
bool alarmActive = false;

// CSS untuk halaman web
String style_details = 
   "<style type=\"text/css\">"
   " body{background-color: #a69695;}"
   "button{display: inline-block;}"
   "#buttons{text-align: center;}"
   ".controllButtons{margin-top: 15px; margin-left: 5px; background-color: white; padding: 10px; border:1px solid black; border-radius: 10px; cursor: pointer; font-size: 14px;}"
   ".controllButtons:hover{background-color: orange; padding: 10px; border:1px solid black; border-radius: 10px; cursor: pointer; font-size: 14px;}"
   "@media only screen and (max-width: 700px) {"
   "button{display: block;}"
   "#buttons{margin-top: 10%; margin-left: 35%;}"
   ".controllButtons{margin-top: 15px; margin-left: 5px; background-color: white; padding: 15px; border:1px solid black; border-radius: 10px; cursor: pointer; font-size: 16px;}"
   ".controllButtons:hover{background-color: orange; padding: 15px; border:1px solid black; border-radius: 10px; cursor: pointer; font-size: 16px;}"
   "}"
   "</style>";

// Halaman HTML untuk tampilan awal
String home_screen = 
   "<!DOCTYPE html><html>"
   "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" + style_details + 
   "</head><body><h1>IoT Alarm Clock</h1>"
   "<div id=\"login\">"
   "<form action=\"/confirmation_screen.php\" method=\"get\">"
   "<h2>Please enter your next Alarm in format (05:45)</h2>"
   "<div>Alarm Time: <input type=\"text\" name=\"code\" autofocus></div>"
   "<div id=\"submit\"><input type=\"submit\" value=\"Submit\"></div>"
   "</form>"
   "</div>"
   "</body></html>";

// Halaman HTML untuk konfirmasi penyetelan alarm
String confirmation_screen = 
   "<!DOCTYPE html><html>"
   "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" + style_details + 
   "</head><body><h1>Alarm Time Set</h1>"
   "</body></html>";

// Setup awal
void setup() {
   Serial.begin(115200);  // Initialize Serial for debugging
   Serial.println("Booting...");

   // Konfigurasi pin buzzer dan RTC
   pinMode(buzzer, OUTPUT);
   pinMode(LED_PIN, OUTPUT);
   pinMode(BUTTON_PIN, INPUT_PULLUP); // Tambahkan resistor pull-up internal

   Wire.begin(D2, D1);
   Rtc.Begin();

   // Cek keabsahan waktu RTC
   if (!Rtc.IsDateTimeValid()) {
       Serial.println("RTC lost confidence in the DateTime!");
       setCompileTime(); // Jika waktu RTC tidak valid, gunakan waktu kompilasi
   }

   // Jalankan RTC jika belum berjalan
   if (!Rtc.GetIsRunning()) {
       Serial.println("RTC was not actively running, starting now");
       Rtc.SetIsRunning(true);
   }

   // Tampilkan waktu saat ini
   RtcDateTime now = Rtc.GetDateTime();
   Serial.print("Current Time after setup: ");
   Serial.print(now.Hour());
   Serial.print(":");
   Serial.print(now.Minute());
   Serial.print(":");
   Serial.println(now.Second());

   // Konfigurasi WiFi ke mode AP
   WiFi.mode(WIFI_AP);
   WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
   WiFi.softAP("ESP8266_DNS_Server"); // Nama jaringan AP

   Serial.println("Access Point \"ESP8266_DNS_Server\" started");
   Serial.print("IP Address: ");
   Serial.println(WiFi.softAPIP());
   
   // Mulai layanan DNS
   dnsServer.start(DNS_PORT, "*", apIP);
   
   // Atur routing untuk halaman yang tidak ditemukan
   webServer.onNotFound([]() {
      webServer.sendHeader("Location", String("http://10.10.10.1/home_screen.php"), true);
      webServer.send(302, "text/plain", "");
   });
   
   // Handle permintaan untuk halaman beranda
   webServer.on("/home_screen.php", []() {
      webServer.send(200, "text/html", home_screen);
   });

   // Handle permintaan untuk halaman konfirmasi
   webServer.on("/confirmation_screen.php", []() {
      if(webServer.args() > 0) { 
         new_time = webServer.arg("code"); // Simpan waktu alarm yang dimasukkan
         webServer.send(200, "text/html", confirmation_screen); // Tampilkan halaman konfirmasi
         hr = new_time.substring(0, 2); // Ambil jam
         hr1 = hr.toInt(); // Konversi ke integer
         minut = new_time.substring(3, 5); // Ambil menit
         minut1 = minut.toInt(); // Konversi ke integer
         Serial.print("Alarm set for: ");
         Serial.print(hr1);
         Serial.print(":");
         Serial.println(minut1);
      } else {
         webServer.send(200, "text/html", home_screen);
      }
   });

   // Mulai server HTTP
   webServer.begin();
   Serial.println("HTTP server started");
}

// Loop utama
void loop() {
   dnsServer.processNextRequest();
   webServer.handleClient();
   if (alarmActive) {
       checkAlarm(); // Periksa alarm hanya jika alarm aktif
   }
   checkButton(); // Periksa tombol
   delay(1000); // Tunggu 1 detik sebelum memeriksa lagi
}
// Fungsi untuk memeriksa alarm
void checkAlarm() {
   RtcDateTime now = Rtc.GetDateTime();
   Serial.print("Current Time: ");
   Serial.print(now.Hour());
   Serial.print(":");
   Serial.print(now.Minute());
   Serial.print(":");
   Serial.println(now.Second());

   // Jika waktu saat ini sama dengan waktu alarm yang diatur
   if (now.Hour() == hr1 && now.Minute() == minut1) {
        Serial.println("Alarm ON - Alarming");
        digitalWrite(LED_PIN, HIGH); // Hidupkan LED
        Buzzer(); // Bunyikan buzzer
        Buzzer();
        Buzzer();
        Buzzer();
    } else {
        digitalWrite(LED_PIN, LOW); // Matikan LED jika alarm tidak aktif
    }
}

// Fungsi untuk menghidupkan buzzer
void Buzzer() {
   digitalWrite(buzzer, HIGH);
   delay(500);
   digitalWrite(buzzer, LOW);
   delay(500);
}

// Fungsi untuk menyetel waktu RTC berdasarkan waktu kompilasi
void setCompileTime() {
    const char* compileDate = __DATE__;
    const char* compileTime = __TIME__;
    int year, month, day, hour, minute, second;

    char monthStr[4];
    sscanf(compileDate, "%s %d %d", monthStr, &day, &year);
    sscanf(compileTime, "%d:%d:%d", &hour, &minute, &second);

    // Konversi nama bulan menjadi angka bulan
    if (strcmp(monthStr, "Jan") == 0) month = 1;
    else if (strcmp(monthStr, "Feb") == 0) month = 2;
    else if (strcmp(monthStr, "Mar") == 0) month = 3;
    else if (strcmp(monthStr, "Apr") == 0) month = 4;
    else if (strcmp(monthStr, "May") == 0) month = 5;
    else if (strcmp(monthStr, "Jun") == 0) month = 6;
    else if (strcmp(monthStr, "Jul") == 0) month = 7;
    else if (strcmp(monthStr, "Aug") == 0) month = 8;
    else if (strcmp(monthStr, "Sep") == 0) month = 9;
    else if (strcmp(monthStr, "Oct") == 0) month = 10;
    else if (strcmp(monthStr, "Nov") == 0) month = 11;
    else if (strcmp(monthStr, "Dec") == 0) month = 12;

    RtcDateTime compiledDateTime(year, month, day, hour, minute, second);
    Rtc.SetDateTime(compiledDateTime);

    Serial.print("RTC time set to: ");
    Serial.print(year);
    Serial.print("/");
    Serial.print(month);
    Serial.print("/");
    Serial.print(day);
    Serial.print(" ");
    Serial.print(hour);
    Serial.print(":");
    Serial.print(minute);
    Serial.print(":");
    Serial.println(second);
}

void checkButton() {
   // Baca status tombol
   int buttonState = digitalRead(BUTTON_PIN);

   // Jika tombol ditekan (logika negatif karena pull-up resistor)
   if (buttonState == LOW) {
      // Matikan alarm
      alarmActive = false;
      // Matikan LED
      digitalWrite(LED_PIN, LOW);
   }
}