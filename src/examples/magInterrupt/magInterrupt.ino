#include <SPI.h>

#define LIS3_CS 38
#define LIS3_SCK 13
#define LIS3_MISO 12
#define LIS3_MOSI 11
#define LIS3_DRDY 24
#define LIS3_SPI1_SPEED 1000000

#define LIS3_REG_WHO_AM_I 0x0F // deve leggere 0b00111101
#define LIS3_REG_CTRL1 0x20 //0b01111100
#define LIS3_REG_CTRL2 0x21 //default
#define LIS3_REG_CTRL3 0x22 //0b00000001
#define LIS3_REG_CTRL4 0x23 //0b00001100
#define LIS3_REG_CTRL5 0x24 //default

#define LIS3_REG_STATUS 0x27
#define LIS3_REG_OUTX_L 0x28 //primo byte mag, 6 in totale 3x2

//booleani di appoggio
volatile bool readyMag = 0;
volatile int magRead = 0;

int magScale = 6842; //LSB per gauss

volatile float magX; //in m/ss
volatile float magY; //in m/ss
volatile float magZ; //in m/ss
int16_t rawMagX, rawMagY, rawMagZ;

//variabili per il timing

unsigned long prevMillis = 0;

void setup() {

  Serial.begin(512000);
  while(!Serial)
    delay(5);

  pinMode(LIS3_CS, OUTPUT);
  pinMode(LIS3_DRDY, INPUT);
  digitalWrite(LIS3_CS, HIGH);

  delay(100);
  //inizializzo SPI1 e faccio sapere che verrà usata all'interno degli interrupt
  SPI1.setMISO(39);
  SPI1.begin();
  //SPI1.usingInterrupt(digitalPinToInterrupt(32));
  //SPI1.usingInterrupt(digitalPinToInterrupt(31));
/*
  SPI1.beginTransaction(SPI1Settings(LIS3_SPI1_SPEED, MSBFIRST, SPI1_MODE3));
  digitalWrite(LIS3_CS, LOW);
  uint8_t lallero[3] = {LIS3_REG_INT1_CTRL | 0b10000000, 0x02, 0x01};
  SPI1.transfer(lallero, 3);
  digitalWrite(LIS3_CS, HIGH);
  SPI1.endTransaction();
  Serial.print("valori interrupt: ");
  Serial.print(lallero[1], BIN);
  Serial.print("   ");
  Serial.println(lallero[2], BIN);
  
  Serial.println("setto imu");
  */
  delay(10);
  SPI1.beginTransaction(SPISettings(LIS3_SPI1_SPEED, MSBFIRST, SPI_MODE3));
  digitalWrite(LIS3_CS, LOW);
  byte buffe[2] = {LIS3_REG_WHO_AM_I | 0b10000000};
  SPI1.transfer(buffe, 2);
  digitalWrite(LIS3_CS, HIGH);
  SPI1.endTransaction();
  Serial.print("whoAmI: ");
  Serial.println(buffe[1], BIN);

  
  MagSetup();
/*
  SPI1.beginTransaction(SPI1Settings(LIS3_SPI1_SPEED, MSBFIRST, SPI1_MODE3));
  digitalWrite(LIS3_CS, LOW);
  uint8_t laller[3] = {LIS3_REG_INT1_CTRL | 0b10000000, 0x02, 0x01};
  SPI1.transfer(laller, 3);
  digitalWrite(LIS3_CS, HIGH);
  SPI1.endTransaction();
  Serial.print("valori interrupt: ");
  Serial.print(laller[1], BIN);
  Serial.print("   ");
  Serial.println(laller[2], BIN);
  */
  SPI1.beginTransaction(SPISettings(LIS3_SPI1_SPEED, MSBFIRST, SPI_MODE3));
  digitalWrite(LIS3_CS, LOW);
  byte buffero[2] = {LIS3_REG_WHO_AM_I | 0b10000000};
  SPI1.transfer(buffero, 2);
  digitalWrite(LIS3_CS, HIGH);
  SPI1.endTransaction();
  Serial.print("whoAmI: ");
  Serial.println(buffero[1], BIN);

//attachInterrupt
  //attachInterrupt(digitalPinToInterrupt(LIS3_DRDY),IntM,RISING);
  prevMillis = millis();
  Serial.println("end");
}

void loop() {

  if(digitalRead(LIS3_DRDY) == true){
    GetMag();
  }
 
  if(millis() - prevMillis >= 1000){
    noInterrupts();
    prevMillis = millis();
    //logga roba
    Serial.println("prima");
    //GetMag();
    Serial.println(String(magX, 6) + " " + String(magY, 6) + " " + String(magZ, 6));
    //Serial.print(gyroRead);
    //Serial.print("  ");
    Serial.println(magRead);
    Serial.println("dopo");
    magRead = 0;
    interrupts();
  }
}


//imposta i registri in base ai valori desiderati
void MagSetup(){
  
  SPI1.beginTransaction(SPISettings(LIS3_SPI1_SPEED, MSBFIRST, SPI_MODE3));
  digitalWrite(LIS3_CS, LOW);
  byte buff1[2] = {LIS3_REG_CTRL2, 0b00000000};
  SPI1.transfer(buff1, 2);
  digitalWrite(LIS3_CS, HIGH);
  delayMicroseconds(500);
  digitalWrite(LIS3_CS, LOW);
  byte buff2[2] = {LIS3_REG_CTRL1, 0b11111100}; //11111100
  SPI1.transfer(buff2, 2);
  digitalWrite(LIS3_CS, HIGH);
  delayMicroseconds(500);
  digitalWrite(LIS3_CS, LOW);
  byte buff3[2] = {LIS3_REG_CTRL4, 0b00001100};
  SPI1.transfer(buff3, 2);
  digitalWrite(LIS3_CS, HIGH);
  delayMicroseconds(500);
  digitalWrite(LIS3_CS, LOW);
  byte buff4[2] = {LIS3_REG_CTRL3, 0b00000000};
  SPI1.transfer(buff4, 2);
  digitalWrite(LIS3_CS, HIGH);
  SPI1.endTransaction();
  delayMicroseconds(500);
}

void GetMag(){

  byte buff[6];
  SPI1.beginTransaction(SPISettings(LIS3_SPI1_SPEED, MSBFIRST, SPI_MODE3));
  digitalWrite(LIS3_CS, LOW);
  
  SPI1.transfer(LIS3_REG_OUTX_L | 0b11000000);
  SPI1.transfer(buff,6);

  digitalWrite(LIS3_CS, HIGH);
  SPI1.endTransaction();

  rawMagX = buff[1] << 8 | buff[0];
  rawMagY = buff[3] << 8 | buff[2];
  rawMagZ = buff[5] << 8 | buff[4];

  magX = (float)rawMagX / magScale;
  magY = (float)rawMagY / magScale;
  magZ = (float)rawMagZ / magScale;
  
  magRead++;
  
}

void IntM(){
  //Serial.println("uno");
  GetMag();
  //Serial.println("due");
}
