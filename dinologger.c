#include "dinologger.h"

unsigned char *kbus[] = {US_KBMAP};
unsigned int stats[97] = {0};

char *cd_file[BUFFER_SIZE];
int cd_size = 0;
int cd_index = 0;

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = device_read,
};

char *strdup(char const *str) {
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

ssize_t device_read(struct file *filep, char *u_buffer, size_t len, loff_t *offset) {
	if (*offset >= cd_size)
		return 0;
	if (*offset + len > cd_size)
		len = cd_size - *offset;

	int i = 0;
	int j = 0;
	int total = 0;
	int bytes = 0;

	while (i < cd_index) {
		int len_str = strlen(cd_file[i]) + 1;
		if (total + len_str >= *offset) {
			j = *offset - total;
			break;
		}
		total += len_str;
		i++;
	}

	while (i < cd_index && len > 0) {
		while (cd_file[i][j]) {
			if (put_user(cd_file[i][j], u_buffer++))
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

void add_to_cd(char *str) {
	if (cd_index < BUFFER_SIZE - 1) {
		cd_file[cd_index] = str;
		cd_index++;
	} else {
		kfree(cd_file[0]);
		for (int i = 0; i < cd_index; i++) {
			cd_file[i] = cd_file[i + 1];
		}
		cd_file[cd_index] = str;
	}
}

void add_entry(int scancode, int release, struct rtc_time *time) {
	char entry[42];
	int len = scnprintf(entry, 42,
			"[%ptRt] %s (%d) %s",
			time, NAME(scancode), scancode, STATE(release));

	if (len <= 0)
		return ;
	char *log = strdup(entry);
	if (!log)
		return ;

	add_to_cd(log);
	cd_size += len + 1;

	LOGF("%s", entry);
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

	int max = 0;
	int sum = 0;
	for (int i = 0; i < 97; i++) {
		if (MAPPED(i)) {
			if (stats[i] > stats[max])
				max = i;
			sum += stats[i];
		}
	}

	int ratio = (stats[max]*100)/sum;
	LOGF("you pressed %d keys", sum);
	LOGF("most pressed key is '%s': %d times (%d%%) !", kbus[max], stats[max], ratio);

	free_irq(1, (void *)irq_handler);
	for (int i = 0; i < cd_index; i++) {
		kfree(cd_file[i]);
	}
	return;
}

MODULE_LICENSE ("GPL");
MODULE_AUTHOR("DinoMalin");

module_init(m_init);
module_exit(m_exit);
