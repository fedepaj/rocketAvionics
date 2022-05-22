#include "BMP388.h"

/***********************************************************************************************
 * FUNZIONI PER IL BAROMETRO
 */


int BMP388::init(){
  pinMode(BMP388_CS, OUTPUT);
  pinMode(BMP388_INT, INPUT);
  
  digitalWrite(BMP388_CS, HIGH);
  SPI.beginTransaction(SPISettings(BMP388_SPI_SPEED, MSBFIRST, SPI_MODE3));
  digitalWrite(BMP388_CS, LOW);
  //impostazioni
  byte settingsBuff[10] = {BMP388_REG_INT_CTRL, 0b01000010, BMP388_REG_PWR_CTRL, 0b00110011,
                            BMP388_REG_OSR, 0b00000011, BMP388_REG_ODR, 0x02, BMP388_REG_CONFIG, 0x02,};
  SPI.transfer(settingsBuff, 10);
  //STUCK HERE
  digitalWrite(BMP388_CS, HIGH);
  SPI.endTransaction();
  BMP388::calib();
  return 0; // the return value of the setup function will be used for error handling
}

void BMP388::calib(){
  SPI.beginTransaction(SPISettings(BMP388_SPI_SPEED, MSBFIRST, SPI_MODE3));
  digitalWrite(BMP388_CS, LOW);
  byte buff[21];
  SPI.transfer(0x31 | 0b10000000);
  SPI.transfer(0x00);
  SPI.transfer(buff, 21);
  digitalWrite(BMP388_CS, HIGH);
  SPI.endTransaction();

  uint16_t REG_par_t1 = buff[1] << 8 | buff[0];
  uint16_t REG_par_t2 = buff[3] << 8 | buff[2];
  int8_t REG_par_t3 = buff[4];
  int16_t REG_par_p1 = buff[6] << 8 | buff[5];
  int16_t REG_par_p2= buff[8] << 8 | buff[7];
  int8_t REG_par_p3 = buff[9];
  int8_t REG_par_p4 = buff[10];
  uint16_t REG_par_p5 = buff[12] << 8 | buff[11];
  uint16_t REG_par_p6 = buff[14] << 8 | buff[13];
  int8_t REG_par_p7 = buff[15];
  int8_t REG_par_p8 = buff[16];
  int16_t REG_par_p9 = buff[18] << 8 | buff[17];
  int8_t REG_par_p10 = buff[19];
  int8_t REG_par_p11 = buff[20];

  calibData.par_t1 = (double)REG_par_t1 / pow(2, -8);
  calibData.par_t2 = (double)REG_par_t2 / pow(2, 30);
  calibData.par_t3 = (double)REG_par_t3 / pow(2, 48);
  calibData.par_p1 = ((double)REG_par_p1 - pow(2, 14)) / pow(2, 20);
  calibData.par_p2 = ((double)REG_par_p2 - pow(2, 14)) / pow(2, 29);
  calibData.par_p3 = (double)REG_par_p3 / pow(2, 32);
  calibData.par_p4 = (double)REG_par_p4 / pow(2, 37);
  calibData.par_p5 = (double)REG_par_p5 / pow(2, -3);
  calibData.par_p6 = (double)REG_par_p6 / pow(2, 6);
  calibData.par_p7 = (double)REG_par_p7 / pow(2, 8);
  calibData.par_p8 = (double)REG_par_p8 / pow(2, 15);
  calibData.par_p9 = (double)REG_par_p9 / pow(2, 48);
  calibData.par_p10 = (double)REG_par_p10 / pow(2, 48);
  calibData.par_p11 = (double)REG_par_p11 / pow(2, 65);
}


altiValues_t BMP388::measure(){
  prec=curr;
  altiValues_t altiValues;
  altiValues.tstp = millis();
  altiValues.p=getP();
  altiValues.t=getT();
  altiValues.altitude=getAltitude(altiValues.p,altiValues.t);
  //filtro passa basso a 2Hz, i valori commentati sono relativi a 10Hz
  float a = 0.9802; //0.9048
  float b = 0.0099; //0.0476
  float c = 1 - a - b;
  altiValues.filtAlti = a*prec.filtAlti + b*prec.altitude + c*altiValues.altitude;
  float aS = 0.91; //0.9048
  float bS = 0.045; //0.0476
  float cS = 1 - aS - bS;
  altiValues.filtAltiS = aS*prec.filtAltiS + bS*prec.altitude + cS*altiValues.altitude;
  /*
  float R=1;
  float Q=1e-05;
  float p=1.0;
  //filtro di Kalman
  float Pp=p+Q;
  //xp=x;
  float K=Pp/(Pp+R);
  float e=altiValues.altitude-prec.filtAlti;
  p=(1-K)*Pp;
  altiValues.filtAlti=filtPrec.filtAlti+K*e;
  //altiValues.filtAlti=0.90476*prec+0.04761*altiValues.altitude+0.04761*prec;
  */
  //float vel = prec.altitude - altiValues.altitude;
  //float filtVel = prec.filtAlti - altiValues.filtAlti;
  altiValues.filtVel = (altiValues.filtAlti - prec.filtAlti)/(altiValues.tstp - prec.tstp)*1000;
  
  ALTFILTDEBUG("Altitudine:"+String(altiValues.altitude)+", "+"Filtrata:"+String(altiValues.filtAlti));
  curr=altiValues;
  return altiValues;
}

float BMP388::getAltitude(float p, float t){
 float alti = ((float)powf(SEA_LEVEL_PRESSURE / p, 0.190223f) - 1.0f) * (t + 273.15f) / 0.0065f;
 return alti;
}

float BMP388::getP(){
  byte buff[4];
  SPI.beginTransaction(SPISettings(BMP388_SPI_SPEED, MSBFIRST, SPI_MODE3));
  digitalWrite(BMP388_CS, LOW);
  SPI.transfer(BMP388_REG_OUTP | 0b10000000);
  SPI.transfer(buff,4);
  digitalWrite(BMP388_CS, HIGH);
  SPI.endTransaction();
  rawPressure = buff[3] <<16 | (buff[2] << 8 | buff[1]);
  return compensateP(rawPressure);
}

float BMP388::getT(){
  byte buff[4];
  SPI.beginTransaction(SPISettings(BMP388_SPI_SPEED, MSBFIRST, SPI_MODE3));
  digitalWrite(BMP388_CS, LOW);
  SPI.transfer(BMP388_REG_OUTT | 0b10000000);
  SPI.transfer(buff,4);
  digitalWrite(BMP388_CS, HIGH);
  SPI.endTransaction();
  rawTemp = buff[3] <<16 | (buff[2] << 8 | buff[1]);
  return compensateT(rawTemp);
}

float BMP388::compensateT(uint32_t uncomp_temp){
  float partial_data1;
  float partial_data2;
  partial_data1 = (float)(uncomp_temp - calibData.par_t1);
  partial_data2 = (float)(partial_data1 * calibData.par_t2);
  calibData.t_lin = partial_data2 + (partial_data1 * partial_data1) * calibData.par_t3;
  return calibData.t_lin;
}

float BMP388::compensateP(uint32_t uncomp_press){
  float comp_press;
  float partial_data1;
  float partial_data2;
  float partial_data3;
  float partial_data4;
  float partial_out1;
  float partial_out2;
  partial_data1 = calibData.par_p6 * calibData.t_lin;
  partial_data2 = calibData.par_p7 * (calibData.t_lin * calibData.t_lin);
  partial_data3 = calibData.par_p8 * (calibData.t_lin * calibData.t_lin * calibData.t_lin);
  partial_out1 = calibData.par_p5 + partial_data1 + partial_data2 + partial_data3;
  partial_data1 = calibData.par_p2 * calibData.t_lin;
  partial_data2 = calibData.par_p3 * (calibData.t_lin * calibData.t_lin);
  partial_data3 = calibData.par_p4 * (calibData.t_lin * calibData.t_lin * calibData.t_lin);
  partial_out2 = (float)uncomp_press * (calibData.par_p1 + partial_data1 + partial_data2 + partial_data3);
  partial_data1 = (float)uncomp_press * (float)uncomp_press;
  partial_data2 = calibData.par_p9 + calibData.par_p10 * calibData.t_lin;
  partial_data3 = partial_data1 *  partial_data2;
  partial_data4 = partial_data3 + ((float)uncomp_press * (float)uncomp_press * (float)uncomp_press) * calibData.par_p11;
  comp_press = partial_out1 + partial_out2 + partial_data4;
  return comp_press;
}
