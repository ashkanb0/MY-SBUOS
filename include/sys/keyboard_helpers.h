#ifndef __KBD_HLP_H
#define __KBD_HLP_H

enum{
S_INPUT = 0,
S_NEED_1 = 1,
S_NEED_2 = 2,
S_FINISHED = 3,
};

char  _smalls  [128] = {""};
char _capitals [128] = {""};


#define K_SHIFTDOWN 0x2a
#define K_CTRLDOWN  0x1d


int keyboard_feed(unsigned char key, char* buff);


#endif
