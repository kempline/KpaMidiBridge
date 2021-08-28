#ifndef __KPA_H__
#define __KPA_H__

#include <stdint.h>
#include <string>

#define MIDI_MSG_PROG_CHG       0xC0  

#define KPA_FCT_SINGLE_PARAMETER_CHANGE                     0x01
#define KPA_FCT_MULTI_PARAMETER_CHANGE                      0x02
#define KPA_FCT_STRING_PARAMETER_CHANGE                     0x03
#define KPA_FCT_BLOB                                        0x04
#define KPA_FCT_RESERVED_1                                  0x05
#define KPA_FCT_EXTENDED_PARAMETER_CHANGE                   0x06
#define KPA_FCT_EXTENDED_STRING_PARAMETER_CHANGE            0x07
#define KPA_FCT_REQUEST_SINGLE_PARAMETER_VALUE              0x41
#define KPA_FCT_REQUEST_MULTI_PARAMETER_VALUE               0x42
#define KPA_FCT_REQUEST_STRING_PARAMETER                    0x43
#define KPA_FCT_REQUEST_EXTENDED_STRING_PARAMETER           0x47
#define KPA_FCT_REQUEST_PARAMETER_VALUE_AS_RENDERED_STRING  0x7c
#define KPA_FCT_RESERVED_2                                  0x7e
#define KPA_FCT_RESERVED_3                                  0x7f
#define KPA_FCT_RESPONSE_PARAMETER_VALUE_AS_RENDERED_STRING 0x3c
#define KPA_INDEX_FCT_CODE  5

class Kpa {
private:
public:
    static bool convert5to4byte(const uint8_t *p_input, unsigned int p_msgLength, uint8_t *p_output ) {
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
};

#endif // __KPA_H__
