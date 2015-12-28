#include <sys/keyboard_helpers.h>


int state = S_INPUT, pressed_key = 0, shift = 0, ctrl = 0;
char* _smalls = _key_map + 0;
char* _capitals = _key_map + 128;
/**
* buff is an array of "4" characters
**/
void print_glyph(char* buff){
	// TODO : special cases: ESC, tab, ....

	if(shift||ctrl){
		buff[2] = _capitals[pressed_key];
	}else{
		buff[2] = _smalls[pressed_key];
	}
	if(ctrl){
		buff[1]= '^';
	}
}


int keyboard_feed(unsigned char key, char* buff){
	if(state == S_NEED_1){
		state = S_FINISHED;
	}
	else if( state == S_NEED_2){
		state = S_NEED_1;
	}
	else if(state == S_INPUT){
		if (key == K_CTRLDOWN){
			ctrl = -1;			
		}
		else if (key == K_SHIFTDOWN){
			shift = -1;
		}
		else{
			pressed_key = key;
			if (ctrl || shift)
				state = S_NEED_2;
			else
				state = S_NEED_1;
		}
	}
	if (state == S_FINISHED){
		print_glyph(buff);
		return 1;
	}
	return 0;
}

