#ifndef PT2314_H
#define PT2314_H

#define PT2314_address 0x44 // adres 0b10001000


#include <Arduino.h>
class PT2314
{
  public:
    PT2314();
        void setVolume(int vol); // user scale 0...56: 0=mute/max attenuation, 56=max volume
	void setAttL(int att_l); // int 0...24 === 0...-30 db step 1.25 dB      
	void setAttR(int att_r); // int 0...24 === 0...-30 dB step 1.25 dB
        void setSwitch(int input, int loud, int usil); 
        // input int 0..2 === IN 1...3 
        // loudness int 0...1 === 1-on 0-off
        // input gain int 0...3 === 0...11.25dB step 3.75 dB
        void setBass(int bass);   // dB -14...+14, hardware step 2 dB
        void setTreble(int treb); // dB -14...+14, hardware step 2 dB
	
  private:
	void writeWire(char a);
};
	
#endif //PT2314_H
