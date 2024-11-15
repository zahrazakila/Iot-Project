#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Define the pin connections
#define RAINDROP_SENSOR D1
#define LDR_SENSOR A0
#define BUZZER_PIN D5
#define LED_PIN D6

// Threshold value for LDR
#define LDR_THRESHOLD 43  // Adjust based on calibration

// Blynk credentials
char auth[] = "Gwqx49PxC2DFB6hKDhxATrDshcvVUNH1"; // Ganti dengan Auth Token Blynk Anda
char ssid[] = "GENDUT"; // Ganti dengan nama jaringan WiFi Anda
char pass[] = "aabbccdd"; // Ganti dengan kata sandi WiFi Anda

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println();
  Serial.println("Initializing...");

  // Initialize the pins
  pinMode(RAINDROP_SENSOR, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // Turn off buzzer and LED initially
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  // Attempt to connect to WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass,);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to Blynk
  Serial.println("Connecting to Blynk server...");
  Blynk.config(auth, "prakitblog.com", 8181);
  while (Blynk.connect() == false) {
    // Wait until connected
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nBlynk connected");
}

void loop() {
  Blynk.run();

  // Read sensor values
  int raindropValue = digitalRead(RAINDROP_SENSOR);
  int ldrValue = analogRead(LDR_SENSOR);

  // Print sensor values to the serial monitor for debugging
  Serial.print("Raindrop Sensor Value: ");
  Serial.println(raindropValue);
  Serial.print("LDR Sensor Value: ");
  Serial.println(ldrValue);

  // Send sensor values to Blynk
  Blynk.virtualWrite(V1, raindropValue); // Ganti V1 dengan pin virtual di aplikasi Blynk
  Blynk.virtualWrite(V2, ldrValue); // Ganti V2 dengan pin virtual di aplikasi Blynk

  // Check if it's raining
  if (raindropValue == LOW) {  // Assuming LOW means it's raining
    // It's raining
    digitalWrite(BUZZER_PIN, HIGH);
    Serial.println("Status: Hujan terdeteksi!");
    Blynk.notify("Hujan terdeteksi!");
  } else {
    // No rain
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("Status: Tidak hujan.");
  }

  // Check if it's cloudy
  if (ldrValue > LDR_THRESHOLD) {  // Adjust threshold based on calibration
    // It's cloudy
    digitalWrite(LED_PIN, HIGH);
    Serial.println("Status: Mendung terdeteksi!");
  } else {
    // Not cloudy
    digitalWrite(LED_PIN, LOW);
    Serial.println("Status: Tidak mendung.");
  }

  // Add a small delay to avoid flooding the serial monitor
  delay(1000);
}
