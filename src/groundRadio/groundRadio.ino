
#include <SparkFun_u-blox_GNSS_Arduino_Library.h> //http://librarymanager/All#SparkFun_u-blox_GNSS

#include <SD.h>
#include <SdFat.h>

#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 15
#define RF95_FREQ 868.0

#define SD_CONFIG SdioConfig(FIFO_SDIO) // Use Teensy SDIO
#define FILENAME "gps_dump"
int FILE_COUNT = 0;
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

SdFat32 sd;
File32 ex;

//void print_graph(UBX_NAV_PVT_data_t *gps_data);
void create_file_w_header();
void save_to_SD(UBX_NAV_PVT_data_t *gps_data);
void print_table(UBX_NAV_PVT_data_t *gps_data);

void setup() {
  
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(115200);
  //SerialUSB1.begin(115200);

  delay(100);
  if (!sd.begin(SD_CONFIG)) {
      Serial.println("SD initialization failed!");
      while(true){}
    }else{
      Serial.println("SD initialized.");
      delay(100);
  }
  create_file_w_header();
  // manual reset of radio module
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  if (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }

  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  rf95.setTxPower(23, false);
  rf95.setSpreadingFactor(7);
  rf95.setSignalBandwidth(500000);
  rf95.setCodingRate4(5);
  rf95.setPreambleLength(6);
}

void loop() {
  
  if(rf95.available()){
    Serial.println("ricevuto qualcosa");
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len)){
      UBX_NAV_PVT_data_t *gps_data = (UBX_NAV_PVT_data_t *)buf;
      //print_graph(gps_data);
      print_table(gps_data);
      save_to_SD(gps_data);
    }
  }
}

// void print_graph(UBX_NAV_PVT_data_t *gps_data){
//   //Serial.println(String(""+gps_data->hour));
//   SerialUSB1.print("numSV:");
//   SerialUSB1.println(gps_data->numSV);    // Number of satellites used in Nav Solution
//   SerialUSB1.print("lon:");
//   SerialUSB1.println(gps_data->lon);      // Longitude: deg * 1e-7
//   SerialUSB1.print("lat:");
//   SerialUSB1.println(gps_data->lat);      // Latitude: deg * 1e-7
//   SerialUSB1.print("height:");
//   SerialUSB1.println(gps_data->height);   // Height above ellipsoid: mm
//   SerialUSB1.print("hMSL:");
//   SerialUSB1.println(gps_data->hMSL);     // Height above mean sea level: mm
//   SerialUSB1.print("hAcc:");
//   SerialUSB1.println(gps_data->hAcc);    // Horizontal accuracy estimate: mm
//   SerialUSB1.print("vAcc:");
//   SerialUSB1.println(gps_data->vAcc);    // Vertical accuracy estimate: mm
//   SerialUSB1.print("velN:");
//   SerialUSB1.println(gps_data->velN);     // NED north velocity: mm/s
//   SerialUSB1.print("velE:");
//   SerialUSB1.println(gps_data->velE);     // NED east velocity: mm/s
//   SerialUSB1.print("velD:");
//   SerialUSB1.println(gps_data->velD);     // NED down velocity: mm/s
//   SerialUSB1.print("gSpeed:");
//   SerialUSB1.println(gps_data->gSpeed);   // Ground Speed (2-D): mm/s
//   SerialUSB1.print("headMot:");
//   SerialUSB1.println(gps_data->headMot);  // Heading of motion (2-D): deg * 1e-5
//   SerialUSB1.print("sAcc:");
//   SerialUSB1.println(gps_data->sAcc);    // Speed accuracy estimate: mm/s
//   SerialUSB1.print("headAcc:");
//   SerialUSB1.println(gps_data->headAcc); // Heading accuracy estimate (both motion and vehicle): deg * 1e-5
//   SerialUSB1.print("pDOP:");
//   SerialUSB1.println(gps_data->pDOP);    // Position DOP * 0.01
//   SerialUSB1.print("headVeh:");
//   SerialUSB1.println(gps_data->headVeh); // Heading of vehicle (2-D): deg * 1e-5
//   SerialUSB1.print("magDec:");
//   SerialUSB1.println(gps_data->magDec);  // Magnetic declination: deg * 1e-2
//   SerialUSB1.print("magAcc:");
//   SerialUSB1.println(gps_data->magAcc); // Magnetic declination accuracy: deg * 1e-2
  
// }

void print_table(UBX_NAV_PVT_data_t *gps_data){
  //Serial.println(String(""+gps_data->hour));
  Serial.print("numSV:");
  Serial.print(gps_data->numSV);    // Number of satellites used in Nav Solution
  Serial.print("\t");
  Serial.print("lon:");
  Serial.print(gps_data->lon);      // Longitude: deg * 1e-7
  Serial.print("\t");
  Serial.print("lat:");
  Serial.print(gps_data->lat);      // Latitude: deg * 1e-7
  Serial.print("\t");
  Serial.print("height:");
  Serial.print(gps_data->height);   // Height above ellipsoid: mm
  Serial.print("\t");
  Serial.print("hMSL:");
  Serial.print(gps_data->hMSL);     // Height above mean sea level: mm
  Serial.print("\t");
  Serial.print("hAcc:");
  Serial.print(gps_data->hAcc);    // Horizontal accuracy estimate: mm
  Serial.print("\t");
  Serial.print("vAcc:");
  Serial.print(gps_data->vAcc);    // Vertical accuracy estimate: mm
  Serial.print("\t");
  Serial.print("velN:");
  Serial.print(gps_data->velN);     // NED north velocity: mm/s
  Serial.print("\t");
  Serial.print("velE:");
  Serial.print(gps_data->velE);     // NED east velocity: mm/s
  Serial.print("\t");
  Serial.print("velD:");
  Serial.print(gps_data->velD);     // NED down velocity: mm/s
  Serial.print("\t");
  Serial.print("gSpeed:");
  Serial.println(gps_data->gSpeed);   // Ground Speed (2-D): mm/s

  Serial.print("headMot:");
  Serial.print(gps_data->headMot);  // Heading of motion (2-D): deg * 1e-5
  Serial.print("\t");
  Serial.print("sAcc:");
  Serial.print(gps_data->sAcc);    // Speed accuracy estimate: mm/s
  Serial.print("\t");
  Serial.print("headAcc:");
  Serial.print(gps_data->headAcc); // Heading accuracy estimate (both motion and vehicle): deg * 1e-5
  Serial.print("\t");
  Serial.print("pDOP:");
  Serial.print(gps_data->pDOP);    // Position DOP * 0.01
  Serial.print("\t");
  Serial.print("headVeh:");
  Serial.print(gps_data->headVeh); // Heading of vehicle (2-D): deg * 1e-5
  Serial.print("\t");
  Serial.print("magDec:");
  Serial.print(gps_data->magDec);  // Magnetic declination: deg * 1e-2
  Serial.print("\t");
  Serial.print("magAcc:");
  Serial.print(gps_data->magAcc); // Magnetic declination accuracy: deg * 1e-2
  
}

void create_file_w_header(){
  int c=0;
  while (sd.exists(FILENAME+String(c)+String(".csv"))) c++;
  FILE_COUNT=c;
  if (!ex.open(String(String(FILENAME)+String(FILE_COUNT)+String(".csv")).c_str(), O_WRONLY | O_CREAT | O_APPEND)) {
    return;
  }
  ex.print("tstp;numSV;lon;lat;height;hMSL;hAcc;vAcc;velN;velE;velD;gSpeed;headMot;sAcc;headAcc;pDOP;headVeh;magDec;magAcc;");
  
  Serial.println(String(String(FILENAME)+String(FILE_COUNT)+String(".csv")));
  ex.close();
}

void save_to_SD(UBX_NAV_PVT_data_t *gps_data){
  if (!ex.open(String(String(FILENAME)+String(FILE_COUNT)+String(".csv")).c_str(), O_WRONLY | O_CREAT | O_APPEND)) {
    return;
  }
  ex.print(gps_data->iTOW);
  ex.print(";");
  ex.print(gps_data->numSV);    // Number of satellites used in Nav Solution
  ex.print(";");
  ex.print(gps_data->lon);      // Longitude: deg * 1e-7
  ex.print(";");
  ex.print(gps_data->lat);      // Latitude: deg * 1e-7
  ex.print(";");
  ex.print(gps_data->height);   // Height above ellipsoid: mm
  ex.print(";");
  ex.print(gps_data->hMSL);     // Height above mean sea level: mm
  ex.print(";");
  ex.print(gps_data->hAcc);    // Horizontal accuracy estimate: mm
  ex.print(";");
  ex.print(gps_data->vAcc);    // Vertical accuracy estimate: mm
  ex.print(";");
  ex.print(gps_data->velN);     // NED north velocity: mm/s
  ex.print(";");
  ex.print(gps_data->velE);     // NED east velocity: mm/s
  ex.print(";");
  ex.print(gps_data->velD);     // NED down velocity: mm/s
  ex.print(";");
  ex.print(gps_data->gSpeed);   // Ground Speed (2-D): mm/s
  ex.print(";");
  ex.print(gps_data->headMot);  // Heading of motion (2-D): deg * 1e-5
  ex.print(";");
  ex.print(gps_data->sAcc);    // Speed accuracy estimate: mm/s
  ex.print(";");
  ex.print(gps_data->headAcc); // Heading accuracy estimate (both motion and vehicle): deg * 1e-5
  ex.print(";");
  ex.print(gps_data->pDOP);    // Position DOP * 0.01
  ex.print(";");
  ex.print(gps_data->headVeh); // Heading of vehicle (2-D): deg * 1e-5
  ex.print(";");
  ex.print(gps_data->magDec);  // Magnetic declination: deg * 1e-2
  ex.print(";");
  ex.print(gps_data->magAcc); // Magnetic declination accuracy: deg * 1e-2
  ex.println(";");

  //Serial.println(String(String(FILENAME)+String(FILE_COUNT)+String(".csv")));
  ex.close();
}
