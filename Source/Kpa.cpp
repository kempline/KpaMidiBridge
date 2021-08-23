#include "Kpa.h"

bool Kpa::convert5to4byte(const uint8_t *p_input, unsigned int p_msgLength, uint8_t *p_output ) {
  if( p_msgLength < 5 )
    return false;
  
  unsigned char b24_31=0, b16_23=0, b8_15=0, b0_7=0;
  
  b24_31 = p_input[0] & 0b00001111;
  b24_31 = b24_31 << 4;
  b24_31 += (p_input[1] & 0b01111000) >> 3;
  
  b16_23 = p_input[1] & 0b00000111;
  b16_23 = b16_23 << 5;
  b16_23 += (p_input[2] & 0b01111100) >> 2;
  
  b8_15 = p_input[2] & 0b00000011;
  b8_15 = b8_15 << 6;
  b8_15 += (p_input[3] & 0b01111110) >> 1;
  
  b0_7 = p_input[3] & 0b00000001;
  b0_7 = b0_7 << 7;
  b0_7 += p_input[4] & 0b01111111;
  
  p_output[0] = b24_31;
  p_output[1] = b16_23;
  p_output[2] = b8_15;
  p_output[3] = b0_7;

  return true;
}
