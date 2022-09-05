/* ==================================================================================
  File:          Full_V6_RA0.ino
  Author:  MCI Electronics / Space and Planetary Exploration Laboratory
                www.olimex.cl / www.spel.uchile.cl
  Description:   Example source code for Automatic Photometer Sensor Managment
  Target Device: Arduino Uno
  ==================================================================================
  //  Ver | dd mmm yyyy | Author  | Description
  // =====|=============|========|=====================================================
  // 1.00 | 08 Jul 2018 | CGC   | Second release
  // ==================================================================================*/


// Si no tienes estas librerías las descargas y las agregas a la carpeta de librerías de Arduino
#include <SFE_BMP180.h> // BMP 180 Para barometro, altura y presion
#include <Wire.h> // Comunicacion I2C
#include <TinyGPS++.h> // Libreria del GPS
#include <SoftwareSerial.h> // Libreria para crear puertos seriales
#include <SD.h> // Libreria para tarjeta SD+  


const int chipSelect = 8; // SS tarjeta SD
char ID[] = "005"; // ID del instrumento
char buff[8];
static const uint32_t GPSBaud = 9600; // Velocidad de conexion GPS
char dato;

SFE_BMP180 pressure; // Objeto BMP
TinyGPSPlus gps; // Objeto GPS
SoftwareSerial ss(3, 2); // Conexion serial para conectarse al GPS


#define SELPIN 4 //Pin de activación del ADC
#define DATAOUT 11 // MOSI
#define DATAIN 12 // MISO
#define SPICLOCK 13 // CLK
#define CTRL_Z 26 // termino


void setup()
{
  String zz, zz2_1, zz2_2, zz2_3, zz3, zz3_1, zz3_2, zz3_3, zz4;
  unsigned long tiempo;
  float lat, lon;
  boolean latlong;
  
  // Inicialización ADC
  //set pin modes
  pinMode(SELPIN, OUTPUT);
  pinMode(DATAOUT, OUTPUT);
  pinMode(DATAIN, INPUT);
  pinMode(SPICLOCK, OUTPUT);
  //disable device to start with
  digitalWrite(SELPIN, HIGH);
  digitalWrite(DATAOUT, LOW);
  digitalWrite(SPICLOCK, LOW);

  //Señal proveniente del arduino por mini
  pinMode(A2, INPUT);
  // Inicialización de comunicaciones seriales y BMP
  ss.begin(GPSBaud);
  delay(100);
  pressure.begin();
  delay(100);

  //--------------------Medicion-----------------------------
  //Sensores
  zz = ID;
  while (digitalRead(A2) == 0)
  {
  }

  delay(100);
  zz2_1 = data();
  zz3_1 = GPS_time();
  zz2_2 = data();
  zz3_2 = GPS_time();
  zz2_3 = data();
  zz3_3 = GPS_time();
  //ahora esperamos y mandamos los datos
  smartDelay(1000);
  sprintf(buff,"005/%d%d%d%d.csv",gps.date.day(),gps.date.month(),(gps.date.year()-2000),gps.time.hour());
  
  delay(100);
  SD.begin(8);
  delay(100);
  
  zz4 = BMP();
  delay(100);
  
  tiempo = millis(); //El tiempo de incicio para marcar

  while ((millis() < tiempo + 30000))
  {
    smartDelay(1000);
  }

  latlong = gps.location.isValid();
  lat = gps.location.lat();
  lon = gps.location.lng();

  zz3 = GPS(latlong, lat, lon);
   
  delay(100);

  // Guardando datos en la micro SD
  File dataFile = SD.open(buff, FILE_WRITE);
  // Si el archivo está disponible se guardan los datos
  if (dataFile) {
 
    dataFile.print(zz);
    dataFile.print(zz2_1);
    dataFile.print(zz3);
    dataFile.print(zz3_1);
    dataFile.println(zz4);

    dataFile.print(zz);
    dataFile.print(zz2_2);
    dataFile.print(zz3);
    dataFile.print(zz3_2);
    dataFile.println(zz4);


    dataFile.print(zz);
    dataFile.print(zz2_3);
    dataFile.print(zz3);
    dataFile.print(zz3_3);
    dataFile.println(zz4);
    
    delay(100);
    dataFile.close();
  }
  // En el caso de no disponibilidad avisa el error
  else
  {
  }
  delay(100);
}

void loop()
{
}



//----------------------Obtencion datos sensores---------------------------

String data()
{
  const char coma = ',';
  int readvalue;
  int data1, data2, data3, data4;
  String zz;
  unsigned long tiempo, delta_t;
  //coma = ","; // Separador reservado para ahorrar memoria
  //P0 = 1013; // Persión al nivel del mar
  tiempo = millis(); //El tiempo de incicio para marcar
  delta_t = tiempo + 32000;
  data1 = 0;
  data2 = 0;
  data3 = 0;
  data4 = 0;
  //Sensores
  while (digitalRead(A2) && (millis() <= delta_t))
  {
    readvalue = read_adc(1);

    if (data1 <= readvalue)
    {
      data1 = readvalue;
    }

    readvalue = read_adc(2);

    if (data2 <= readvalue)
    {
      data2 = readvalue;
    }
    readvalue = read_adc(3);

    if (data3 <= readvalue)
    {
      data3 = readvalue;
    }

    readvalue = read_adc(4);

    if (data4 <= readvalue)
    {
      data4 = readvalue;
    }
  }
  zz = coma;
  zz += data1;
  zz += coma;
  zz += data2;
  zz += coma;
  zz += data3;
  zz += coma;
  zz += data4;
  return zz;
}

//-------------------------Obtencion datos GPS----------------------------
String GPS(bool latlong, float lat, float lon)
{
  const char coma = ',';
  String zz;
  
  zz = coma;
  
  if (!latlong)
  {
  }
  else
  {
    if (lat <= 0)
    {
      zz.concat(lat * -1);
      zz += coma;
      zz += "S";
    }
    else
    {
      zz.concat(lat);
      zz += coma;
      zz += "N";
    }
    
    zz += coma;
    
    if (lon <= 0)
    {
      zz.concat(lon * -1);
      zz += coma;
      zz += "W";
    }
    else
    {
      zz.concat(lon);
      zz += coma;
      zz += "E";
    }
  }

  zz += coma;
  zz += gps.date.day();
  zz += coma;
  zz += gps.date.month();
  zz += coma;
  zz += gps.date.year();
  
  return zz;
}
//--------------------------------------------------------------------
String GPS_time()
{
  const char coma = ',';
  String zz;

  smartDelay(1000);

  zz = coma;
  zz += gps.time.hour();
  zz += coma;
  zz += gps.time.minute();
  zz += coma;
  zz += gps.time.second();
  zz += coma;
  zz += gps.altitude.meters();
  return zz;
}
//-----------------------Obtencion datos BMP---------------------------
String BMP()
{
  char status;
  const char coma = ',';
  String zz;
  double T, P, a;
  //,Temperatura
  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = pressure.getTemperature(T);
  }
  if (status != 0)
  {
    zz = coma;
    zz += T;

    //, Presion, Altura_BMP
    status = pressure.startPressure(3);
  }
  else
  {
    zz = coma;
  }

  if (status != 0)
  {
    delay(status);

    status = pressure.getPressure(P, T);
    if (status != 0)
    {
      const int P0 = 1013;
      zz += coma;
      zz += P;

      a = pressure.altitude(P, P0);
      zz += coma;
      zz += a;
    }
  }
  else
  {
    zz += coma;
    zz += coma;
  }
  return zz;
}
//-------------------------Lectura de datos desde ADC------------------------

int read_adc(int channel) {
  int adcvalue = 0;
  byte commandbits = B11000000; //command bits - start, mode, chn (3), dont care (3)

  //allow channel selection
  commandbits |= ((channel - 1) << 3);

  digitalWrite(SELPIN, LOW); //Select adc
  // setup bits to be written
  for (int i = 7; i >= 3; i--) {
    digitalWrite(DATAOUT, commandbits & 1 << i);
    //cycle clock
    digitalWrite(SPICLOCK, HIGH);
    digitalWrite(SPICLOCK, LOW);
  }

  digitalWrite(SPICLOCK, HIGH);   //ignores 2 null bits
  digitalWrite(SPICLOCK, LOW);
  digitalWrite(SPICLOCK, HIGH);
  digitalWrite(SPICLOCK, LOW);

  //read bits from adc
  for (int i = 11; i >= 0; i--) {
    adcvalue += digitalRead(DATAIN) << i;
    //cycle clock
    digitalWrite(SPICLOCK, HIGH);
    digitalWrite(SPICLOCK, LOW);
  }
  digitalWrite(SELPIN, HIGH); //turn off device
  return adcvalue;
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}
