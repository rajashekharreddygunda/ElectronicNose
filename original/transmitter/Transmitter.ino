//Transmitter code - FIXED for Arduino AVR (Uno/Nano/Mega)
#include <Wire.h>
#include <SPI.h>
#include <LoRa.h> 

// LoRa pin definitions for Arduino + XL1278/SX1278
// For Arduino Uno/Nano: SCK=13, MISO=12, MOSI=11 (hardware SPI pins)
#define SS 10    // NSS pin
#define RST 9    // Reset pin
#define DIO0 2   // DIO0 pin (interrupt capable)

const int MQ3_PIN = A0;
const int MQ135_PIN = A1;
const int MQ7_PIN = A2;
const int MQ8_PIN = A3;

const float RL_MQ3 = 10000;
const float RL_MQ135 = 10000;
const float RL_MQ7 = 10000;
const float RL_MQ8 = 10000;

const float R0_MQ3 = 23369.910;
const float R0_MQ135 = 1326.9465;
const float R0_MQ7 = 1056.725212;
const float R0_MQ8 = 6.769;

const float A_MQ3 = 0.526, B_MQ3 = -0.698;
const float A_MQ135 = 5.759, B_MQ135 = -0.380;
const float A_MQ7 = 20.207, B_MQ7 = -0.698;
const float A_MQ8 = 37034, B_MQ8 = -1.522;

float calculateRs(float sensorValue, float RL);
float calculatePPM(float sensorRs, float R0, float A, float B);

void setup() {
  Serial.begin(9600);
  while (!Serial);  
  
  Serial.println("LoRa Sender Initializing...");
  
  // Set LoRa pins (no SPI.begin() needed for AVR - uses hardware pins)
  LoRa.setPins(SS, RST, DIO0);
  
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  
  // Configure LoRa parameters
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setSyncWord(0x12);
  LoRa.setTxPower(20);
  LoRa.enableCrc();
  
  Serial.println("LoRa Sender Ready!");
}

void loop() {
  int mq3_value = analogRead(MQ3_PIN);
  int mq135_value = analogRead(MQ135_PIN);
  int mq7_value = analogRead(MQ7_PIN);
  int mq8_value = analogRead(MQ8_PIN);

  float Rs_mq3 = calculateRs(mq3_value, RL_MQ3);
  float Rs_mq135 = calculateRs(mq135_value, RL_MQ135);
  float Rs_mq7 = calculateRs(mq7_value, RL_MQ7);
  float Rs_mq8 = calculateRs(mq8_value, RL_MQ8);

  float ppm_mq3 = calculatePPM(Rs_mq3, R0_MQ3, A_MQ3, B_MQ3);
  float ppm_mq135 = calculatePPM(Rs_mq135, R0_MQ135, A_MQ135, B_MQ135);
  float ppm_mq7 = calculatePPM(Rs_mq7, R0_MQ7, A_MQ7, B_MQ7);
  float ppm_mq8 = calculatePPM(Rs_mq8, R0_MQ8, A_MQ8, B_MQ8);

  String gas1 = "ETHANOL";
  String gas2 = "CO";
  String gas3 = "AMMONIA";
  String gas4 = "HYDROGEN";

  String ppm1 = String(ppm_mq3, 2);
  String ppm2 = String(ppm_mq7, 2);
  String ppm3 = String(ppm_mq135, 2);
  String ppm4 = String(ppm_mq8, 2);

  String payload =
    gas1 + "," + ppm1 + "," +
    gas2 + "," + ppm2 + "," +
    gas3 + "," + ppm3 + "," +
    gas4 + "," + ppm4;

  // Send packet
  LoRa.beginPacket();
  LoRa.print(payload);
  LoRa.endPacket();

  // Debug output
  Serial.println("=== Transmission ===");
  Serial.print("Sent: ");
  Serial.println(payload);
  Serial.print("Length: ");
  Serial.println(payload.length());
  Serial.println("Gas concentrations (PPM):");
  Serial.print("Ethanol(ETH): "); Serial.println(ppm_mq3);
  Serial.print("CO: "); Serial.println(ppm_mq7);
  Serial.print("Ammonia(NH3): "); Serial.println(ppm_mq135);
  Serial.print("Hydrogen(H2): "); Serial.println(ppm_mq8);
  Serial.println("----------------------------");
  
  delay(3000);  // Increased delay to match receiver display cycle
}

float calculateRs(float sensorValue, float RL) {
  float Vout = sensorValue * (5.0 / 1023.0);
  if (Vout == 0) Vout = 0.001;  // Prevent division by zero
  return (5.0 * RL / Vout) - RL;
}

float calculatePPM(float sensorRs, float R0, float A, float B) {
  float rs_ro_ratio = sensorRs / R0;
  if (rs_ro_ratio <= 0) rs_ro_ratio = 0.001;  // Prevent log of negative
  return pow(10, log(rs_ro_ratio / A) / B);
}