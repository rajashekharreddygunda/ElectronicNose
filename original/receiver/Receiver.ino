//Receiver code - FIXED for Arduino AVR (Uno/Nano/Mega)
#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include <LiquidCrystal_I2C.h>

// LoRa pin definitions for Arduino + XL1278/SX1278
// For Arduino Uno/Nano: SCK=13, MISO=12, MOSI=11 (hardware SPI pins)
#define SS 10    // NSS pin
#define RST 9    // Reset pin
#define DIO0 2   // DIO0 pin (interrupt capable)

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Change to 0x3F if needed

String gas1 = "";
String ppm1 = "";
String gas2 = "";
String ppm2 = "";
String gas3 = "";
String ppm3 = "";
String gas4 = "";
String ppm4 = "";

unsigned long lastPacketTime = 0;

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Initializing...");
  
  Serial.println("LoRa Receiver Initializing...");
  
  // Set LoRa pins (no SPI.begin() needed for AVR - uses hardware pins)
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed!");
    lcd.clear();
    lcd.print("LoRa Failed!");
    while (1);
  }
  
  // Configure LoRa parameters (must match transmitter)
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setSyncWord(0x12);
  LoRa.enableCrc();
  
  Serial.println("LoRa Receiver Ready!");
  lcd.clear();
  lcd.print("LoRa Ready");
  lcd.setCursor(0, 1);
  lcd.print("Waiting...");
  delay(2000);
}

void loop() {
  int packetSize = LoRa.parsePacket();
  
  if (packetSize) {
    String packet = "";

    while (LoRa.available()) {
      packet += (char)LoRa.read();
    }

    int rssi = LoRa.packetRssi();
    
    Serial.println("=== Received Packet ===");
    Serial.print("Data: ");
    Serial.println(packet);
    Serial.print("RSSI: ");
    Serial.println(rssi);
    Serial.print("Length: ");
    Serial.println(packet.length());

    // Split the received string
    int i1 = packet.indexOf(',');
    int i2 = packet.indexOf(',', i1 + 1);
    int i3 = packet.indexOf(',', i2 + 1);
    int i4 = packet.indexOf(',', i3 + 1);
    int i5 = packet.indexOf(',', i4 + 1);
    int i6 = packet.indexOf(',', i5 + 1);
    int i7 = packet.indexOf(',', i6 + 1);

    // Check if parsing is valid
    if (i1 != -1 && i2 != -1 && i3 != -1 && i4 != -1 && 
        i5 != -1 && i6 != -1 && i7 != -1) {
      
      gas1 = packet.substring(0, i1);
      ppm1 = packet.substring(i1 + 1, i2);

      gas2 = packet.substring(i2 + 1, i3);
      ppm2 = packet.substring(i3 + 1, i4);

      gas3 = packet.substring(i4 + 1, i5);
      ppm3 = packet.substring(i5 + 1, i6);

      gas4 = packet.substring(i6 + 1, i7);
      ppm4 = packet.substring(i7 + 1);

      lastPacketTime = millis();

      // Display one at a time
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(gas1);
      lcd.setCursor(0, 1);
      lcd.print(ppm1);
      lcd.print(" ppm");
      delay(2500);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(gas2);
      lcd.setCursor(0, 1);
      lcd.print(ppm2);
      lcd.print(" ppm");
      delay(2500);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(gas3);
      lcd.setCursor(0, 1);
      lcd.print(ppm3);
      lcd.print(" ppm");
      delay(2500);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(gas4);
      lcd.setCursor(0, 1);
      lcd.print(ppm4);
      lcd.print(" ppm");
      delay(2500);
      
      Serial.println("Display complete");
      Serial.println("----------------------------");
    } else {
      Serial.println("ERROR: Invalid packet format");
      lcd.clear();
      lcd.print("Invalid Data");
      delay(1000);
    }
  }
  
  // Show "waiting" if no packet received for 15 seconds
  if (millis() - lastPacketTime > 15000 && lastPacketTime != 0) {
    lcd.clear();
    lcd.print("Waiting for");
    lcd.setCursor(0, 1);
    lcd.print("transmission...");
    lastPacketTime = 0;
  }
}