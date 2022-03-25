#include "config.h"
#include <TinyGPS++.h>
#include <movingAvg.h>

TinyGPSPlus gps;
TinyGPSCustom fix_type(gps, "GNGSA", 2);
unsigned char serial2bufferRead[1000];
 
float prev_alt_gps = 0; 
float gps_vspeed   = 0; 
unsigned long gpsB = 0;
float prev_cog     = 0; 
bool new_gps = false;
bool new_cog = false;

movingAvg g_vs(GPS_VS_AVG); 

bool gps_ok = false; 
bool cog_ok = false; 

void cmpt_vertical_speed_gps(float da, int dt) {
  gps_vspeed = (da/(dt/1000.0));
  gps_vspeed = g_vs.reading(gps_vspeed*100); 
  gps_vspeed = (gps_vspeed/100);
}

void get_gps() {  
  
  while (GPS_PORT.available()) { 
    gps.encode(GPS_PORT.read()); 
  }
  
  if (((gps.location.age()) < ((1000/GPS_FREQ)*2)) and (atof(fix_type.value()) == 3)) { 
    gps_ok = true; 
  }
  else { gps_ok = false; }
  
  if (gps.course.isUpdated()) {
    if (abs(prev_cog-gps.course.deg())>0.1 or gps.speed.mps()>3) {
      cog_ok = true; 
      new_cog = true;
      prev_cog = gps.course.deg(); 
    }
    else { cog_ok = false; }
  }

  if (gps.altitude.isUpdated()) {
    cmpt_vertical_speed_gps(prev_alt_gps-gps.altitude.meters(), millis()-gpsB);
    gpsB = millis(); 
    prev_alt_gps = gps.altitude.meters(); 
    new_gps = true; 
  }
}
 
void sendPacket(byte *packet, byte len){
    for (byte i = 0; i < len; i++) { GPS_PORT.write(packet[i]); }
}

void gps_setup(int a, int b, int c, int d, int e){

  g_vs.begin();
  
if (a == 9600) {
    GPS_PORT.begin(9600); 
    delay(100); 
    byte packet1[] = {0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xC0, 0x08, 0x00, 0x00, 0x80, 0x25, 0x00, 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x92, 0xB5};
    sendPacket(packet1, sizeof(packet1));
  }
    
if (a == 57600) { 
    GPS_PORT.begin(9600); 
    delay(100);
    byte packet2[] = {0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xC0, 0x08, 0x00, 0x00, 0x00, 0xE1, 0x00, 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCE, 0xC9};
    sendPacket(packet2, sizeof(packet2));
    GPS_PORT.end(); 
    GPS_PORT.begin(57600); 
    delay(100);
  }

if (a == 115200) { 
    GPS_PORT.begin(9600); 
    delay(100);
    byte packet3[] = {0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xC0, 0x08, 0x00, 0x00, 0x00, 0xC2, 0x01, 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x7E};
    sendPacket(packet3, sizeof(packet3)); 
    GPS_PORT.end(); 
    GPS_PORT.begin(115200);
    delay(100);
  }

if (b == 1) { 
    byte packet4[] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xE8, 0x03, 0x01, 0x00, 0x01, 0x00, 0x01, 0x39};
    sendPacket(packet4, sizeof(packet4));
  }

if (b == 5) { 
    byte packet5[] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xC8, 0x00, 0x01, 0x00, 0x01, 0x00, 0xDE, 0x6A};
    sendPacket(packet5, sizeof(packet5));
  }
    
if (b == 10) { 
    byte packet6[] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0x64, 0x00, 0x01, 0x00, 0x01, 0x00, 0x7A, 0x12};
    sendPacket(packet6, sizeof(packet6));
  }

if (c == 0) { 
    byte packet7[] = {0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x5E, 0x01, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x3C};
    sendPacket(packet7, sizeof(packet7));
  }
    
if (c == 1) {
    byte packet8[] = {0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x5E, 0x01, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x84, 0x08};
    sendPacket(packet8, sizeof(packet8)); 
  }

if (c == 2) {
    byte packet8[] = {0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x5E, 0x01, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x85, 0x2A};
    sendPacket(packet8, sizeof(packet8)); 
  }

if (c == 4) {
    byte packet8[] = {0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x08, 0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x5E, 0x01, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x86, 0x4C};
    sendPacket(packet8, sizeof(packet8)); 
  }

if (d == 1) { 
    byte packet9[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x03, 0x35};
    byte packet10[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x03, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x05, 0x43};
    sendPacket(packet9, sizeof(packet9));
    sendPacket(packet10, sizeof(packet10));
  }

if (e == 1) {
    byte packet11[] = {0xB5, 0x62, 0x06, 0x1E, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x32, 0x00, 0x00, 0x99, 0x4C, 0x00, 0x00, 0x5B, 0x10};
    sendPacket(packet11, sizeof(packet11));
  }

  GPS_PORT.addMemoryForRead(serial2bufferRead, sizeof(serial2bufferRead));
   
}

void dateTime(uint16_t* date, uint16_t* time){
  
  *date = FAT_DATE(gps.date.year(), gps.date.month(), gps.date.day());
  *time = FAT_TIME(gps.time.hour(), gps.time.minute(), gps.time.second());
}

String date_time() {
  String date_year  = String(gps.date.year());
  String date_month = String(gps.date.month());
  String date_day   = String(gps.date.day()); 

  String time_hour    = String(gps.time.hour());
  String time_minute  = String(gps.time.minute());
  String time_second  = String(gps.time.second());   
 
  return date_year+":"+date_month+":"+date_day+","+time_hour+":"+time_minute+":"+time_second; 
}

  
String gps_text() {
  String lat_A_text   = String(gps.location.lat(), 10); 
  String lon_A_text   = String(gps.location.lng(), 10);
  String alt_gps_text = String(gps.altitude.meters(), 1);
  String cog_text     = String(gps.course.deg());
  String speed_text   = String(gps.speed.mps(), 1);
  String sat_text     = String(gps.satellites.value());
  String fix_type_text= String(atof(fix_type.value()));
  String hdop_text    = String(gps.hdop.value());
  String pos_age_text; 
  
  if (gps.location.age()>10000) { pos_age_text = String(999); } 
  else                          { pos_age_text = String(gps.location.age());  } 

  return date_time()+","+lat_A_text+","+lon_A_text+","+alt_gps_text+","+cog_text+","+speed_text+","+sat_text+","+hdop_text+","+pos_age_text+","+fix_type_text;
}
