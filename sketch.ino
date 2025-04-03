#define BLYNK_TEMPLATE_ID "TMPL302GvBVxC"
#define BLYNK_TEMPLATE_NAME "Key Pad"

const char* twilioSID = "ACbc62e9ffd20e702c0a1d659e9810118a";  // Your Account SID
const char* twilioAuthToken = "b98d63c93171d1fc7170fa25a8cf5632";  // 🔹 Replace with actual Auth Token
const char* twilioPhoneNumber = "+16154349272";  // Twilio number
const char* recipientPhoneNumber = "+919065260551";  // Your phone number

#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include "ThingSpeak.h"
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define servoPin 15

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000);  //

Servo servo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

char correctPassword[] = "1234"; 
String enteredPassword = "";  
bool isDoorLocked = true;

//ThingSpeak
const int myChannelNumber =2896098 ;
const char* myApiKey = "4OG3XOF7QPNIDFKU";
const char* server = "api.thingspeak.com";

// Blynk Credentials
char auth[] = "Rc__2uhIFC6sJwlhHU2C25QUvVV30b18";  
char ssid[] = "Wokwi-GUEST";       
char pass[] = "";  

int incorrectAttempts = 0;
unsigned long openStartTime = 0;
unsigned long openDuration = 0;
float batteryVoltage = 3.7;
int peakHourUsage[24] = {0};
int unlockAttemptsToday = 0;
int lastDay = -1;

WiFiClient client;
bool lastDoorState = true;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.println(".");
  }
  Serial.println("Wifi connected !");
  Serial.println("Local IP: " + String(WiFi.localIP()));
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  
  // Connect to Blynk
  Blynk.begin(auth, ssid, pass);

  // Setup LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Door LOCK System");
  delay(2000);
  lcd.clear();

  // Setup Servo
  servo.attach(servoPin);
  servo.write(10);  // Start in LOCKED position

  timeClient.begin();
}

BLYNK_WRITE(V2) {
  int switchState = param.asInt(); // Read switch state (0 or 1)
  
  if (switchState == 1) { // Only process when switch is turned ON
    if (enteredPassword.equals(correctPassword)) {  // Use .equals() for comparison
      isDoorLocked = !isDoorLocked; // Toggle lock state
      isDoorLocked ? doorLocked() : doorOpen();
    } else {
      incorrectAttempts++;
      displayMessage("Wrong Password", "Try Again");
    }
    enteredPassword = "";  // Reset password after checking
  }
}

// 🔹 Function to get password input from Blynk
BLYNK_WRITE(V1) {
  enteredPassword = param.asStr();  // Get text from the input field
  Serial.print("Entered Password: ");
  Serial.println(enteredPassword);
}


void loop() {
  Blynk.run();  // Keep Blynk connected

  batteryVoltage -= 0.01;  
  if (batteryVoltage < 3.0) {
    batteryVoltage = 3.7;  // Reset for simulation
  }

  timeClient.update(); 

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected, reconnecting...");
    WiFi.begin(ssid, pass);
  }

  // 🔹 Reset Unlock Attempts at Midnight
  int currentDay = timeClient.getDay();
  if (currentDay != lastDay) {
    unlockAttemptsToday = 0;
    lastDay = currentDay;
  }
  lcd.setCursor(3, 0);
  isDoorLocked ? lcd.print("DOOR LOCKED") : lcd.print("DOOR OPEN");

}

void doorLocked() {
  servo.write(10);
  displayMessage("Password Correct", "LOCKED");
  Serial.println("Password Correct : CLOSED");
  openDuration = millis() - openStartTime; // Calculate open duration
  sendToThingSpeak("Locked");

  // Send SMS Alert
    sendSMSAlert("ALERT: The door has been locked!");
}

void doorOpen() {
  servo.write(180);
  displayMessage("Password Correct", "UNLOCKED");
  Serial.println("Password Correct : OPEN");
  peakHourUsage[getHour()]++; // Track usage by hour
  unlockAttemptsToday++; // Increment daily unlock attempts
  openStartTime = millis(); // Start tracking open duration
  sendToThingSpeak("Open");

  // Send SMS Alert
    sendSMSAlert("ALERT: The door has been opened!");
}

// 🔹 Send Data to ThingSpeak
void sendToThingSpeak(String doorStatus) {
  Serial.println("Sending Data to ThingSpeak...");
  
  String timestamp = getTimeStamp();

  ThingSpeak.setField(1, doorStatus);  
  ThingSpeak.setField(2, timestamp);  
  ThingSpeak.setField(3, incorrectAttempts);  
  ThingSpeak.setField(4, (long)(openDuration / 1000));  // ✅ Cast to long 
  ThingSpeak.setField(5, getBatteryVoltage());  
  ThingSpeak.setField(6, getPeakHour());  
  ThingSpeak.setField(7, detectAnomaly());  
  ThingSpeak.setField(8, unlockAttemptsToday);  

  int response = ThingSpeak.writeFields(myChannelNumber, myApiKey);
  Serial.println(response == 200 ? "Data Sent Successfully!" : "Error sending data: " + String(response));

  delay(5000);
}

float getBatteryVoltage() {
  return batteryVoltage;  // Return the simulated value
}

int detectAnomaly() {
  return (getHour() >= 20) ? 1 : 0;
}

int getHour() {
  return timeClient.getHours();
}
int getPeakHour() {
  int maxHour = 0, maxCount = 0;
  for (int i = 0; i < 24; i++) {
    if (peakHourUsage[i] > maxCount) {
      maxCount = peakHourUsage[i];
      maxHour = i;
    }
  }
  return maxHour;
}

String getTimeStamp() {
  timeClient.update();
  return timeClient.getFormattedTime();
}



void displayMessage(const char *line1, const char *line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(3, 1);
  lcd.print(line2);
  delay(3000);
  lcd.clear();
}

void sendSMSAlert(String message) {
    HTTPClient http;
    String url = "https://api.twilio.com/2010-04-01/Accounts/" + String(twilioSID) + "/Messages.json";

    String postData = "To=" + String(recipientPhoneNumber) +
                      "&From=" + String(twilioPhoneNumber) +
                      "&Body=" + message;

    http.begin(url);
    http.setAuthorization(twilioSID, twilioAuthToken);  // Set Twilio Auth
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpResponseCode = http.POST(postData);
    Serial.print("SMS Response Code: ");
    Serial.println(httpResponseCode);

    http.end();  // Close connection
}
