#ifndef __KBD_HLP_H
#define __KBD_HLP_H

int _4_bit_to_h(unsigned short n){
	return (n<10)?'0'+n:'A'+n-10;	
}

int keyboard_get_char_for(unsigned char key, char* buff){
	buff[0] = _4_bit_to_h((key>>4)&0x0f);
	buff[1] = _4_bit_to_h(key&0x0f);
	buff[2] = 0;
	return 0;
}

#endif
