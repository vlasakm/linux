#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>

MODULE_DESCRIPTION("List current processes");
MODULE_AUTHOR("Kernel Hacker");
MODULE_LICENSE("GPL");

static int my_proc_init(void)
{
	struct task_struct *p;

	p = current;
	pr_info("pid: %d, name: '%s'\n", p->pid, &p->comm[0]);

	return 0;
}

static void my_proc_exit(void)
{
	struct task_struct *p;

	p = current;
	pr_info("pid: %d, name: '%s'\n", p->pid, &p->comm[0]);
}

module_init(my_proc_init);
module_exit(my_proc_exit);
