#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

ESP8266WebServer server(80);
DNSServer dnsServer;

const int relayPin = D1; // Pin relay
bool fanIsOn = false;

// WiFi credentials
const char *ssid = "Fan Control";
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
  html += ".fan-animation { width: 100px; height: 100px; border: 2px solid #fff; border-radius: 50%; margin: 0 auto 20px auto; background: conic-gradient(#fff 0%, #fff0 0%); }";
  html += ".fan-on { animation: spin 1s linear infinite; }";
  html += "@keyframes spin { from { transform: rotate(0deg); } to { transform: rotate(360deg); } }";
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
  html += "      var fan = document.getElementById('fan');";
  html += "      if (response.fanIsOn) {";
  html += "        fan.classList.add('fan-on');";
  html += "      } else {";
  html += "        fan.classList.remove('fan-on');";
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
  html += "<h1>Smart Fan Controller</h1>";
  html += "<div id=\"fan\" class=\"fan-animation\"></div>";
  html += "<button class=\"button\" id=\"onButton\" onclick=\"sendRequest('/on')\">Turn On</button>";
  html += "<button class=\"button\" id=\"offButton\" onclick=\"sendRequest('/off')\">Turn Off</button>";
  html += "</div>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleOn() {
  digitalWrite(relayPin, LOW); // Menghidupkan relay
  fanIsOn = true;
  server.send(200, "text/plain", "Fan Turned On");
}

void handleOff() {
  digitalWrite(relayPin, HIGH); // Mematikan relay
  fanIsOn = false;
  server.send(200, "text/plain", "Fan Turned Off");
}

void handleStatus() {
  String json = "{\"fanIsOn\":";
  json += fanIsOn ? "true" : "false";
  json += "}";
  server.send(200, "application/json", json);
}

void setup() {
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // Mematikan relay awalnya
  Serial.begin(115200);

  // Setup WiFi
  WiFi.softAP(ssid, password);
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Setup DNS Server
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

  // Setup web server
  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
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
}
