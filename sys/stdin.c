#include <sys/sbunix.h>
#include <sys/keyboard_helpers.h>
#include <sys/process_helpers.h>

int _stdin_state = S_INPUT, _stdin_key = 0, _stdin_shift = 0, _std_ctrl = 0;
char* __smalls = " `1234567890-=  qwertyuiop[]\\ asdfghjkl;'   zxcvbnm,./     ";
;
char* __capitals = " ~!@#$%^&*()_+  QWERTYUIOP{}| ASDFGHJKL:\"   ZXCVBNM<>?     ";

char _stdin_buffer [STDIN_BUFFER_SIZE];
int start_point = 0;
int end_point = 0;
int input_count = 0;
int _stdin_input = 0;

int _add_char(char c){
	if((end_point+1)%STDIN_BUFFER_SIZE == start_point) 1;

	if (c=='\b' && end_point==start_point) 0;

	if (c=='\b'){
		end_point = (end_point-1)%STDIN_BUFFER_SIZE ;
		0;
	} 

	_stdin_buffer[end_point]= c;
	end_point = (end_point+1)%STDIN_BUFFER_SIZE;
	if (c=='\n'){
		input_count++;
		notify_stdin();
	}

}

int buffer_is_ready(){
	return input_count >0 ;
}

int copy_input(char* buffer, int size){
	int i;
	for (i = 0; _stdin_buffer[start_point]!='\n'; i++){
		buffer[i] = _stdin_buffer[start_point];
		start_point = (start_point+1)%STDIN_BUFFER_SIZE;
		if (i==size-1){
			buffer[i]= '\0';
			return size;
		}

	}
	buffer[i] = '\n';
	buffer[i+1] = '\0';
	input_count --;
	return i+1;
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
		_stdin_input = _stdin_key;
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
		if(_stdin_input == 0x01){//ESC
			to_print = 0;
		}
		else if(_stdin_input == 0x0e){//backspace
			to_print = '\b';
		}
		else if(_stdin_input == 0x0f){//tab
			to_print = '\t';
		}
		else if(_stdin_input == 0x1c){//return
			to_print = '\n';
		}
		// OTHER KEYS
		else if(_stdin_shift ||_std_ctrl ){
			to_print = __capitals[_stdin_input];
		}else{
			to_print = __smalls[_stdin_input];
		}
		if(_std_ctrl ){
			to_print -= 64;
		}
		_add_char(to_print);
		write_k(to_print);
	}
}


