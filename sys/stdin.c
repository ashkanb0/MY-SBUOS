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
	if((end_point+1)%STDIN_BUFFER_SIZE == start_point) return 1;

	if (c=='\b' && end_point==start_point) return 0;

	if (c=='\b'){
		end_point = (end_point-1)%STDIN_BUFFER_SIZE ;
		return 1;
	} 

	_stdin_buffer[end_point]= c;
	end_point = (end_point+1)%STDIN_BUFFER_SIZE;
	if (c=='\n'|| c=='D'-64){
		input_count++;
		notify_stdin();
	}
	return 1;
}

int buffer_is_ready(){
	return input_count >0 ;
}

int copy_input(char* buffer, int size){
	if(_stdin_buffer[start_point]=='D'-64){
		start_point++;
		return 0;
	}
	int i;
	for (i = 0; _stdin_buffer[start_point]!='\n'; i++){
		buffer[i] = _stdin_buffer[start_point];
		start_point = (start_point+1)%STDIN_BUFFER_SIZE;
		if (i==size-1){
			buffer[i]= '\0';
			return size;
		}

	}
	
	start_point = (start_point+1)%STDIN_BUFFER_SIZE;
	buffer[i] = '\n';
	buffer[i+1] = '\0';
	input_count --;
	return i+1;
}

void fill_single_elem_buffer(char* single_elem_buffer){
	if(_stdin_key == 0x01){//ESC
		single_elem_buffer[0] = 0;
	}
	else if(_stdin_key == 0x0e){//backspace
		single_elem_buffer[0] = '\b';
	}
	else if(_stdin_key == 0x0f){//tab
		single_elem_buffer[0] = '\t';
	}
	else if(_stdin_key == 0x1c){//return
		single_elem_buffer[0] = '\n';
	}
	// OTHER KEYS
	else if(_stdin_shift ||_std_ctrl ){
		single_elem_buffer[0] = __capitals[_stdin_key];
	}else{
		single_elem_buffer[0] = __smalls[_stdin_key];
	}
	if(_std_ctrl ){
		single_elem_buffer[0] -= 64;
	}
}

int _stdin_keyboard_feed(unsigned char key, char* single_elem_buffer){
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
		fill_single_elem_buffer(single_elem_buffer);
		_stdin_shift = 0;
		_std_ctrl = 0;
		_stdin_key = 0;
		return 1;
	}
	return 0;
}

char key_buffer [2];

void buffer_add_char(unsigned char c){
	if (_stdin_keyboard_feed(c, key_buffer)){
		char to_print = key_buffer[0];
		if(_add_char(to_print))
			write_k(to_print);
	}
}


