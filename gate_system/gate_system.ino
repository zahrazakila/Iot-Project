#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <Servo.h>
#include <Ticker.h>

Servo myservo;
ESP8266WebServer server(80);
DNSServer dnsServer;
Ticker closeGateTicker;

int leftIR = D1; // Sensor kiri
int rightIR = D2; // Sensor kanan
int ledPin = D4; // LED indikator
int redPinOther = D5; // Pin untuk warna merah pada lampu RGB lain
int greenPinOther = D6; // Pin untuk warna hijau pada lampu RGB lain
int bluePinOther = D7; // Pin untuk warna biru pada lampu RGB lain
int gateOpen = 150; // Sudut bukaan gerbang
int gateClose = 0.8; // Sudut penutupan gerbang
bool objectDetected = false;
bool gateIsOpen = false;

// WiFi credentials
const char *ssid = "Gate Control";
const char *password = "12345678";

// DNS settings
const byte DNS_PORT = 53;

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background-color: #1e1e1e; color: #ffffff; margin: 0; display: flex; justify-content: center; align-items: center; height: 100vh; }";
  html += ".container { width: 100%; max-width: 600px; padding: 20px; border-radius: 10px; background-color: #333333; box-shadow: 0 4px 8px rgba(0,0,0,0.1); text-align: center; }";
  html += "h1 { font-size: 2em; margin-bottom: 20px; }";
  html += ".button { display: inline-block; width: 80%; padding: 15px; font-size: 1.2em; cursor: pointer; text-align: center; text-decoration: none; outline: none; color: #fff; background-color: #4CAF50; border: none; border-radius: 10px; box-shadow: 0 5px #999; transition: background-color 0.3s, transform 0.3s; margin: 10px 0; }";
  html += ".button:hover { background-color: #45a049; }";
  html += ".button:active { background-color: #3e8e41; box-shadow: 0 5px #666; transform: translateY(4px); }";
  html += ".button:disabled { background-color: #999; cursor: not-allowed; }";
  html += "</style>";
  html += "<script>";
  html += "function sendRequest(action) {";
  html += "  var xhr = new XMLHttpRequest();";
  html += "  xhr.open('GET', action, true);";
  html += "  xhr.send();";
  html += "}";
  html += "function updateStatus() {";
  html += "  var xhr = new XMLHttpRequest();";
  html += "  xhr.onreadystatechange = function() {";
  html += "    if (xhr.readyState == 4 && xhr.status == 200) {";
  html += "      var response = JSON.parse(xhr.responseText);";
  html += "      var openButton = document.getElementById('openButton');";
  html += "      var closeButton = document.getElementById('closeButton');";
  html += "      if (response.objectDetected) {";
  html += "        openButton.disabled = false;";
  html += "        closeButton.disabled = false;";
  html += "      } else {";
  html += "        openButton.disabled = true;";
  html += "        closeButton.disabled = true;";
  html += "      }";
  html += "    }";
  html += "  };";
  html += "  xhr.open('GET', '/status', true);";
  html += "  xhr.send();";
  html += "}";
  html += "setInterval(updateStatus, 1000);"; // Update status setiap detik
  html += "</script>";
  html += "</head><body onload=\"updateStatus()\">";
  html += "<div class=\"container\">";
  html += "<h1>Automatic Gate Opener</h1>";
  html += "<button class=\"button\" id=\"openButton\" onclick=\"sendRequest('/open')\">Open Gate</button>";
  html += "<button class=\"button\" id=\"closeButton\" onclick=\"sendRequest('/close')\">Close Gate</button>";
  html += "</div>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleOpen() {
  myservo.write(gateOpen);
  digitalWrite(ledPin, LOW); // Mematikan LED saat gerbang terbuka
  gateIsOpen = true;
  closeGateTicker.once(4, handleAutoClose); // Mengatur timer untuk menutup gerbang setelah 10 detik
  server.send(200, "text/plain", "Gate Opened");
}

void handleClose() {
  myservo.write(gateClose);
  gateIsOpen = false;
  closeGateTicker.detach(); // Mematikan timer jika gerbang ditutup secara manual
  server.send(200, "text/plain", "Gate Closed");
}

void handleAutoClose() {
  if (gateIsOpen) {
    myservo.write(gateClose);
    gateIsOpen = false;
    server.send(200, "text/plain", "Gate Auto Closed");
  }
}

void handleStatus() {
  String json = "{\"objectDetected\":";
  json += objectDetected ? "true" : "false";
  json += "}";
  server.send(200, "application/json", json);
}

void turnOnRGBWhiteOther() {
  digitalWrite(redPinOther, HIGH);
  digitalWrite(greenPinOther, HIGH);
  digitalWrite(bluePinOther, HIGH);
}

void turnOffRGBOther() {
  digitalWrite(redPinOther, LOW);
  digitalWrite(greenPinOther, LOW);
  digitalWrite(bluePinOther, LOW);
}

void setup() {
  pinMode(leftIR, INPUT);
  pinMode(rightIR, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(redPinOther, OUTPUT);
  pinMode(greenPinOther, OUTPUT);
  pinMode(bluePinOther, OUTPUT);
  digitalWrite(ledPin, LOW); // Mematikan LED awalnya
  digitalWrite(redPinOther, LOW);
  digitalWrite(greenPinOther, LOW);
  digitalWrite(bluePinOther, LOW);

  myservo.attach(D3);
  myservo.write(gateClose); // Menutup gerbang awalnya
  Serial.begin(9600);

  // Setup WiFi
  WiFi.softAP(ssid, password);
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Setup DNS Server
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

  // Setup web server
  server.on("/", handleRoot);
  server.on("/open", handleOpen);
  server.on("/close", handleClose);
  server.on("/status", handleStatus);
  server.onNotFound([]() {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
  });
  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
  turnOnRGBWhiteOther();

  // Logika deteksi sensor
  if (digitalRead(leftIR) == LOW || digitalRead(rightIR) == LOW) {
    if (!objectDetected) {
      objectDetected = true;
      digitalWrite(ledPin, HIGH); // Menyalakan LED saat ada objek terdeteksi

    }
  } else {
    if (objectDetected) {
      objectDetected = false;
      digitalWrite(ledPin, LOW); // Mematikan LED saat tidak ada objek terdeteksi
    }
  }
}
