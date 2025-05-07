#pragma once

#include <safe_arduino.hpp>

#define print_stack_trace()                                                         \
	do {                                                                            \
		/* 1) grab the raw SP */                                                    \
		uint32_t * _sp;                                                             \
		__asm volatile ("mov %0, sp" : "=r" (_sp));                                 \
		/* 2) grab the caller’s return address */                                   \
		void * _lr = __builtin_return_address(0);                                   \
                                                                                    \
		Serial.printf("Function: %s\r\n", __func__);  								\
		Serial.printf("Stack trace (most recent call first):\r\n");                 \
		Serial.printf(" SP   = %p\r\n", _sp);                                       \
		Serial.printf(" LR   = %p\r\n", _lr);                                       \
																		            \
		/* 3) dump a few words off the stack */                                     \
		for (int _i = 0;                                                            \
				(_sp + _i) < (uint32_t*)0x20004000;                                 \
				 ++_i) {                                                            \
			Serial.printf(" SP+%2d = %p\r\n",                                       \
										_i * 4, (void*)_sp[_i]);                    \
		}                                                                           \
	} while (0)