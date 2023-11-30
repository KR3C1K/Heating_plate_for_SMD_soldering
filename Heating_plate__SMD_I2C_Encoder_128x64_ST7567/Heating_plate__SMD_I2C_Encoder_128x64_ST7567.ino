#include <max6675.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <Encoder.h>

U8G2_ST7567_ENH_DG128064I_F_SW_I2C u8g2(U8G2_R2, SCL, SDA); //

Encoder encoder(5, 6);
const int BUTTON_PIN = 7;

const int SO_PIN = 13;
const int CS_PIN = 12;
const int SCK_PIN = 11;

int temp;
MAX6675 thermocouple(SCK_PIN, CS_PIN, SO_PIN);

int currentTempSetting = 0;
unsigned long heatingTime = 30000; // 30 seconds
bool isHeating = false;
unsigned long startTime;

void setup() {
  u8g2.setI2CAddress(0x3F * 2);
  u8g2.begin();
  u8g2.setContrast(255);
  u8g2.setFont(u8g2_font_spleen6x12_mf);

  u8g2.clearBuffer();
  u8g2.setCursor((128 - u8g2.getStrWidth("LUTOWNICA  SMD")) / 2, 20);
  u8g2.print("LUTOWNICA  SMD");
  u8g2.setCursor((128 - u8g2.getStrWidth("wersja 3.0")) / 2, 40);
  u8g2.print("wersja 3.0");
  u8g2.sendBuffer();
  delay(1000);
}

float odczytajTemperature()
{
  float temperatura = thermocouple.readCelsius();
  delay(100);
  return temperatura;
}

void wyswietlBlad() {
    u8g2.clearBuffer();
    u8g2.setCursor(0, 20);
    u8g2.print("Błąd czujnika");
    u8g2.setCursor(0, 40);
    u8g2.print("temperatury!");
    u8g2.sendBuffer();
  }

void loop() {
  temp = thermocouple.readCelsius();

  if (temp == -1 || temp > 1000) // Check that the temperature reading is correct
  {
    wyswietlBlad();
    return; // Break the loop
  }

  int encoderValue = encoder.read();
  currentTempSetting += encoderValue;
  currentTempSetting = constrain(currentTempSetting, 0, 300);
  
  
  u8g2.clearBuffer();
  u8g2.setCursor(0, 20);
  u8g2.print("Temp: ");
  u8g2.print(temp);
  u8g2.print(" ");
  u8g2.write(byte(0));
  u8g2.print("C ");

  u8g2.setCursor(0, 40);
  u8g2.print("Set: ");
  u8g2.print(currentTempSetting);
  u8g2.print(" ");
  u8g2.write(byte(0));
  u8g2.print("C ");

  u8g2.sendBuffer();

  if (!isHeating && temp < currentTempSetting) {
    isHeating = true;
    startTime = millis();
    // Turn on heating here, e.g., digitalWrite(PRZEKAZNIK_PIN, HIGH);
  }

  if (isHeating && temp >= currentTempSetting) {
    isHeating = false;
    // Turn off heating here, e.g., digitalWrite(PRZEKAZNIK_PIN, LOW);
  }

  if (isHeating && millis() - startTime >= heatingTime) {
    isHeating = false;
    // Turn off heating here, e.g., digitalWrite(PRZEKAZNIK_PIN, LOW);
  }

  delay(500);
}
