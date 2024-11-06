#include "linux/init.h"
#include "linux/module.h"
#include "linux/fs.h"

#define DEVICE_NAME "dinologger";
#define LOG(msg) printk(KERN_NOTICE #DEVICE_NAME #msg "\n");
#define LOGF(msg, ...) printk(KERN_NOTICE #DEVICE_NAME #msg "\n", __VA_ARGS__);

ssize_t device_read(struct file *, char *, size_t, loff_t *);

static const char buffer[] = "I'm a dinosaur";
int buffer_size = sizeof(buffer);

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = device_read,
};

struct miscdevice misc;
int register_device() {
	misc.minor = MISC_DYNAMIC_MINOR;
	misc.name = DEVICE_NAME;
	misc.fops = &fops;

	int res = misc_register(&misc);
	if (res < 0) {
		LOGF("failed to register - error code: %d", res);
		return res;
	}
	LOGF("registered device");
	return 0;
}
void unregister_device() {
	LOG("unregistering");
	misc_deregister(&misc);
}

ssize_t device_read(struct file *filep, char *user_buffer, size_t len, loff_t *offset) {
	LOG("reading device");

	if (*offset >= buffer_size)
		return 0;
	if (*offset + len > buffer_size)
		len = buffer_size - *position;
	if (copy_to_user(usr_buffer, buffer + *offset, len) != 0)
		return -EFAULT;

	*position += len;
	return count;
}

//int register_device() {
//	LOG("registering device");
//	int res = register_chrdev(0, DEVICE_NAME, &fops);
//	if (res < 0) {
//		LOGF("failed to register - error code: %d", res);
//		return res;
//	}
//	major_nb = result;
//	LOG("registered - major number: %d", major_nb);
//	return 0;
//}

static int init(void) {
	register_device();
	return 0;
}

static void exit(void) {
	unregister_device();
	return;
}

module_init(init);
module_exit(exit);
