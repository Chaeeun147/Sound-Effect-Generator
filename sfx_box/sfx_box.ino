#include <LiquidCrystal_I2C.h>
#include <I2S.h>
#include "crickets_wav.h"
#include "ba_dum_tss_wav.h"
#include "FAH_wav.h"
#include "gigchad_v2_wav.h" 
#include <Wire.h>

//Pico - Amplifier pins
#define pDOUT 20
#define pBLCK 18
#define pLRC  19

//pins for keys
int key1 = 4;
int key2 = 5;
int key3 = 6;
int key4 = 7;
int key5 = 9;
int key6 = 10;
int key7 = 11;
int key8 = 12;
int key9 = 14;
int combo[3] = {3, 5, 4}; 
int comboPos = 0;
String currentText = "";

I2S i2s(OUTPUT);
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

String scanned = "";
String timsCard = "%B6010561411708866^/TIMCARD^25010004000060322242?;6010561411708866=25010004000060322242?";
String vanillaCard = "%B4118103530122905^A GIFT FOR YOU^32055211000000088000000?;4118103530122905=320552110000088?";
String rockCard = ";603628759342363525075?";

const unsigned char* currentSfx = nullptr;
unsigned int currentSfxLen = 0;
unsigned int currentSfxPos = 44; // skip 44 byte WAV header
bool update = false;

void setup1(){
  //lcd screen
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Nothing playing...");
}

void loop1(){
  if (update){
    update = false;
    lcd.setCursor(0, 0);
    lcd.print(currentText);
  }
}

void setup() {
  Serial.begin(9600);
  //key switches
  pinMode(key1, INPUT_PULLUP);
  pinMode(key2, INPUT_PULLUP);
  pinMode(key3, INPUT_PULLUP);
  pinMode(key4, INPUT_PULLUP);
  pinMode(key5, INPUT_PULLUP);
  pinMode(key6, INPUT_PULLUP);
  pinMode(key7, INPUT_PULLUP);
  pinMode(key8, INPUT_PULLUP);
  pinMode(key9, INPUT_PULLUP);

  //i2s.setLRC(pLRC);
  //i2c stuff
  i2s.setBCLK(pBLCK);
  i2s.setDOUT(pDOUT);                                                          
  if (!i2s.begin(22050)) {
    Serial.println("Failed to start I2S");
    while(1);
  }
}

void loop() {
  if (Serial.available()) {
    scanned = Serial.readStringUntil('\n');
    Serial.flush();
    scanned.trim();
    Serial.print("Scanned: ");
    Serial.println(scanned);
    playSfx(scanned);
  }
  playback();
  testKeys();
}

void playSfx(String card) {
  if (card == timsCard) {
    Serial.println("crickets - now playing");
    currentText = "--crickets--";
    update = true;
    currentSfx = crickets_wav;
    currentSfxLen = crickets_wav_len;
    currentSfxPos = 44;
  }
  else if (card == vanillaCard) {
    Serial.println("ba dum tss - now playing");
    currentText = "--ba dum tss--";
    update = true;
    currentSfx = ba_dum_tss_wav;
    currentSfxLen = ba_dum_tss_wav_len;
    currentSfxPos = 44;
  }
  else if (card == rockCard) {
    Serial.println("FAH - now playing");
    currentText = "--fahhh--";
    update = true;
    currentSfx = FAH_wav;
    currentSfxLen = FAH_wav_len;
    currentSfxPos = 44;
  }
}

void playback() {
  if (currentSfx == nullptr) return;
  if (currentSfxPos >= currentSfxLen) {
    currentSfx = nullptr;
    return;
  }
  // write samples to speakers
  int16_t sample = (int16_t)(currentSfx[currentSfxPos] | (currentSfx[currentSfxPos + 1] << 8));
  i2s.write(sample); // left
  i2s.write(sample); // right
  currentSfxPos += 2;
}

void testKeys(){
  if (digitalRead(key1) == LOW){
    Serial.println("key 1 pressed.");
    delay(150);
    while(digitalRead(key1) == LOW);
    checkPassword(3);
  }
  if (digitalRead(key2) == LOW){
    Serial.println("key 2 pressed.");
    delay(150);
    while(digitalRead(key2) == LOW);
    checkPassword(4);
  }
  if (digitalRead(key3) == LOW){
    Serial.println("key 3 pressed.");
    delay(150);
    while(digitalRead(key3) == LOW);
    checkPassword(5);
  }
  if (digitalRead(key4) == LOW){
    Serial.println("key 4 pressed.");
    delay(150);
    while(digitalRead(key4) == LOW);
    checkPassword(6);
  }
  if (digitalRead(key5) == LOW){
    Serial.println("key 5 pressed.");
    delay(150);
    while(digitalRead(key5) == LOW);
    checkPassword(8);
  }
  if (digitalRead(key6) == LOW){
    Serial.println("key 6 pressed.");
    delay(150);
    while(digitalRead(key6) == LOW);
    checkPassword(4);
  }
  if (digitalRead(key7) == LOW){
    Serial.println("key 7 pressed.");
    delay(150);
    while(digitalRead(key7) == LOW);
    checkPassword(9);
  }
  if (digitalRead(key8) == LOW){
    Serial.println("key 8 pressed.");
    delay(150);
    while(digitalRead(key8) == LOW);
    checkPassword(0);
  }
  if (digitalRead(key9) == LOW){
    Serial.println("key 9 pressed.");
    delay(150);
    while(digitalRead(key9) == LOW);
    checkPassword(7);
    
  }
}


void checkPassword(int pressedPin) {
  if (pressedPin == combo[comboPos]) {
    comboPos++;
    if (comboPos == 3) {
      Serial.println("ACCESS GRANTED");
      comboPos = 0;
      playAura();
    }
  } else {
    Serial.println("WRONG");
    comboPos = 0;
  }
}


void playAura(){
  Serial.println("Aura - now playing");
  currentText = "--gigachad--";
  update = true;
  currentSfx = gigchad_v2_wav;
  currentSfxLen = gigchad_v2_wav_len;
  currentSfxPos = 44;
}