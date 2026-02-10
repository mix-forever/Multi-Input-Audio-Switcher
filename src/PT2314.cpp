#include <Arduino.h>
#include <Wire.h>
#include "PT2314.h"

PT2314::PT2314(){
	Wire.begin();
}

void PT2314::setVolume(int vol){
  vol = 56-vol;
  writeWire(vol);
}

void PT2314::setAttL(int att_l){
  att_l = 0b11000000 + att_l;
  writeWire(att_l);
}

void PT2314::setAttR(int att_r){
  att_r = 0b11100000 + att_r;
  writeWire(att_r);
}

void PT2314::setSwitch(int input, int loud, int usil){
  switch (loud){
    case 1:loud = 0b00000000;break;
    case 0:loud = 0b00000100;break;
  }
  switch (usil){
    case 3:usil = 0b00000000;break;
    case 2:usil = 0b00001000;break;
    case 1:usil = 0b00010000;break;
    case 0:usil = 0b00011000;break; 
  }
  writeWire(0b01000000 + input + loud + usil);
}

void PT2314::setBass(int bass){
  switch (bass){
    case -7: bass = 0b01100000;break;
    case -6: bass = 0b01100001;break;
    case -5: bass = 0b01100010;break;
    case -4: bass = 0b01100011;break; 
    case -3: bass = 0b01100100;break;
    case -2: bass = 0b01100101;break;
    case -1: bass = 0b01100110;break;
    case 0:  bass = 0b01100111;break;
    case 1:  bass = 0b01101110;break;
    case 2:  bass = 0b01101101;break;
    case 3:  bass = 0b01101100;break;
    case 4:  bass = 0b01101011;break;
    case 5:  bass = 0b01101010;break;
    case 6:  bass = 0b01101001;break;
    case 7:  bass = 0b01101000;break;
  }
  writeWire(bass);
}

void PT2314::setTreble(int treb){
  switch (treb){
    case -7: treb = 0b01110000;break;
    case -6: treb = 0b01110001;break;
    case -5: treb = 0b01110010;break;
    case -4: treb = 0b01110011;break; 
    case -3: treb = 0b01110100;break;
    case -2: treb = 0b01110101;break;
    case -1: treb = 0b01110110;break;
    case 0:  treb = 0b01111111;break;
    case 1:  treb = 0b01111110;break;
    case 2:  treb = 0b01111101;break;
    case 3:  treb = 0b01111100;break;
    case 4:  treb = 0b01111011;break;
    case 5:  treb = 0b01111010;break;
    case 6:  treb = 0b01111001;break;
    case 7:  treb = 0b01111000;break;
  }
  writeWire(treb);
}

void PT2314::writeWire(char a){
  Wire.beginTransmission(PT2314_address);
  Wire.write (a);
  Wire.endTransmission();
}
