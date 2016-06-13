#ifndef __SBUNIX_H
#define __SBUNIX_H

#include <sys/defs.h>
#include <sys/keyboard_helpers.h>
#include <sys/timer_helpers.h>
#include <stdarg.h>



void put_pressed_key(unsigned char key);

void buffer_add_char(unsigned char c);
// extern char _time_buffer [];
int buffer_is_ready();

void put_time_since_boot(void);

void set_display_address(uint64_t add);


void printf(const char *fmt, ...);

void write_k(char v);
#endif
