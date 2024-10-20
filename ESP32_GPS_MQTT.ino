#include <TinyGPSPlus.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <PubSubClient.h> // For MQTT communication

// Wi-Fi credentials
const char* ssid = "BLOKH49BARU";
const char* password = "wifi_ahmad";

// MQTT broker settings
const char* mqtt_server = "broker.emqx.io";
const char* mqtt_topic = "braincore/live-location";

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// The TinyGPSPlus object
TinyGPSPlus gps;

// WiFi and MQTT client objects
WiFiClient espClient;
PubSubClient client(espClient);

// Timing control
unsigned long lastSendTime = 0;       // Timestamp of last sent data
const long sendInterval = 1000;       // 1 second interval for sending data

// Tracker ID (can be changed as needed)
const int trackerID = 2;

// Function to connect to WiFi
void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi, IP address: ");
  Serial.println(WiFi.localIP());
}

// Function to connect to the MQTT broker
void connectToMQTT() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT broker...");
    if (client.connect("GPS_Tracker_Client")) {
      Serial.println("Connected to MQTT broker.");
    } else {
      Serial.print("Failed to connect to MQTT, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}

// Function to publish GPS data to the MQTT broker
void publishGPSData() {
  if (gps.location.isValid()) {
    unsigned long currentMillis = millis();
    
    // Ensure we only send data every 1 second (1000ms)
    if (currentMillis - lastSendTime >= sendInterval) {
      lastSendTime = currentMillis;  // Update last send time
      
      // Create JSON payload with latitude, longitude, tracker ID, and current timestamp
      String payload = "{\"tracker_id\": " + String(trackerID) +
                       ", \"lat\": " + String(gps.location.lat(), 6) +
                       ", \"lon\": " + String(gps.location.lng(), 6) +
                       ", \"timestamp\": " + String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second()) + "}";
      
      client.publish(mqtt_topic, payload.c_str());  // Send data to MQTT
      Serial.println("MQTT payload sent: " + payload);
    }
  } else {
    Serial.println("Invalid GPS Data");
  }
}

// Function to display GPS data on the OLED
void displayGPSData() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  if (gps.location.isValid()) {
    display.setTextSize(1);
    display.setCursor(0, 5);
    display.print("Lat: ");
    display.print(gps.location.lat(), 6);
    
    display.setCursor(0, 20);
    display.print("Lng: ");
    display.print(gps.location.lng(), 6);
    
    display.setCursor(0, 35);
    display.print("Speed: ");
    display.print(gps.speed.kmph());
    
    display.setCursor(0, 50);
    display.print("Satellites: ");
    display.print(gps.satellites.value());

    display.display();
  } else {
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.print("No Data");
    display.display();
  }
}

void setup() {
  Serial.begin(9600);      // For serial monitor
  Serial2.begin(9600, SERIAL_8N1, 16, 17);     // GPS Serial
  connectToWiFi();         // Connect to Wi-Fi
  client.setServer(mqtt_server, 1883);  // Connect to MQTT server

  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
}

void loop() {
  // Ensure Wi-Fi and MQTT connection
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }
  if (!client.connected()) {
    connectToMQTT();
  }
  client.loop();  // Ensure MQTT connection is maintained

  // Process GPS data and update every second
  bool newData = false;
  while (Serial2.available() > 0) {
    if (gps.encode(Serial2.read())) {
      newData = true;
    }
  }

  // If there is new GPS data, send it
  if (newData) {
    publishGPSData();
    displayGPSData();
  }

  // Check if GPS is not providing data
  if (gps.charsProcessed() < 10) {
    Serial.println(F("No GPS detected: check wiring."));
  }
}
