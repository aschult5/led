#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <asm/io.h>
/*#include <linux/gpio.h>
#include <mach/platform.h>
#define GPIO_BASE 0x20200000
*/
#define GPIO_BASE 0x3f200000


struct GpioRegisters
{
	uint32_t GPFSEL[6];
	uint32_t reserved0;
	uint32_t GPSET[2];
	uint32_t reserved1;
	uint32_t GPCLR[2];
};

#define GPIO_IN 0
#define GPIO_OUT 1

struct GpioRegisters *p_gpio_regs;
/*static struct timer_list blink_timer;*/
static const int gpio_pin = 17;
static struct timer_list t;

static void select_gpio_fn(uint32_t pin, uint32_t fn)
{
	uint32_t reg = pin/10;
	uint32_t bit = (pin%10)*3;
	uint32_t mask = 0b111 << bit;

	printk(KERN_INFO "Changing GPIO%d fn from %x to %x\n",
		pin,
		(p_gpio_regs->GPFSEL[reg] & mask) >> bit,
		fn & 0b111);
		
	p_gpio_regs->GPFSEL[reg] &= ~mask;
	p_gpio_regs->GPFSEL[reg] |= (fn & 0b111) << bit;
}

static void set_gpio_value(uint32_t pin, uint32_t val)
{
	uint32_t reg = pin/32;
	uint32_t bit = pin%32;
	
	printk(KERN_INFO "Setting GPIO%d to %x\n",
		pin,
		val & 1);

	if (val)
		p_gpio_regs->GPSET[reg] = 1 << bit;
	else
		p_gpio_regs->GPCLR[reg] = 1 << bit;
}

static void led_timer_handler(unsigned long val)
{
	static char last=1;
	printk(KERN_INFO "Received timer\n");
	if (last==1)
	{
		set_gpio_value(gpio_pin, 0);
		last = 0;
	}
	else
	{
		set_gpio_value(gpio_pin, 1);
		last = 1;
	}
	t.expires = jiffies + 100;
	add_timer(&t);
}

static int __init hello_led_init(void)
{
	printk(KERN_INFO "Hello, world\n");

	init_timer(&t);
	t.function = &led_timer_handler;

	p_gpio_regs = (struct GpioRegisters*) ioremap(GPIO_BASE,4096);
	select_gpio_fn(gpio_pin, GPIO_OUT);
	set_gpio_value(gpio_pin, 1);

	t.expires = jiffies + 100;
	add_timer(&t);

	return 0;
}

static void __exit hello_led_exit(void)
{
	printk(KERN_INFO "Goodbye, world\n");
	del_timer(&t);
	set_gpio_value(gpio_pin, 0);
	iounmap(p_gpio_regs);
}

module_init(hello_led_init);
module_exit(hello_led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("aschult5");
MODULE_DESCRIPTION("Hello world");
MODULE_VERSION("0.1");
