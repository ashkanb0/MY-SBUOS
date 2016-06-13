#include <sys/sbunix.h>
#include <sys/keyboard_helpers.h>

int _stdin_state = S_INPUT, _stdin_key = 0, _stdin_shift = 0, _std_ctrl = 0;
char* __smalls = " `1234567890-=  qwertyuiop[]\\ asdfghjkl;'   zxcvbnm,./     ";
;
char* __capitals = " ~!@#$%^&*()_+  QWERTYUIOP{}| ASDFGHJKL:\"   ZXCVBNM<>?     ";

char buffer [256];
int start_point = 0;
int end_point = 1;
int input_count = 0;

void _add_char(char c){
	if((end_point+1)%256 == start_point) return;

	buffer[end_point]= c;
	end_point = (end_point+1)%256;
	if (c=='\n') input_count++;
}

int buffer_is_ready(){
	return input_count >0 ;
}



int _stdin_keyboard_feed(unsigned char key){
	if(_stdin_state == S_NEED_1){
		_stdin_state = S_FINISHED;
	}
	else if( _stdin_state == S_NEED_2){
		_stdin_state = S_NEED_1;
	}
	else if(_stdin_state == S_INPUT){
		if (key == K_CTRLDOWN){
			_std_ctrl = -1;			
		}
		else if (key == K_SHIFTDOWN){
			_stdin_shift = -1;
		}
		else{
			_stdin_key = key;
			if (_std_ctrl || _stdin_shift )
				_stdin_state = S_NEED_2;
			else
				_stdin_state = S_NEED_1;
		}
	}
	if (_stdin_state == S_FINISHED){
		_stdin_state = S_INPUT;
		_stdin_shift = 0;
		_std_ctrl = 0;
		_stdin_key = 0;
		return 1;
	}
	return 0;
}
void buffer_add_char(unsigned char c){
	if (_stdin_keyboard_feed(c)){
		char to_print = 0;
		if(_stdin_key == 0x01){//ESC
			to_print = 0;
		}
		if(_stdin_key == 0x0e){//backspace
			to_print = '\b';
		}
		if(_stdin_key == 0x0f){//tab
			to_print = '\t';
		}
		if(_stdin_key == 0x1c){//return
			to_print = '\n';
		}
		// OTHER KEYS
		if(_stdin_shift ||_std_ctrl ){
			to_print = __capitals[_stdin_key];
		}else{
			to_print = __smalls[_stdin_key];
		}
		if(_std_ctrl ){
			to_print -= 64;
		}
		_add_char(to_print);
		write_k(to_print);
	}
}


