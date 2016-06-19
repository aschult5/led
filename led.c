#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/gpio.h>

static struct timer_list blink_timer;
static const int gpio_pin = 17;

static void led_timer_handler(unsigned long val)
{
	static int last=1;
	if (last==1)
	{
		gpio_set_value(gpio_pin, 0);
		last = 0;
	}
	else
	{
		gpio_set_value(gpio_pin, 1);
		last = 1;
	}
	blink_timer.expires = jiffies + 100;
	add_timer(&blink_timer);
}

static int __init led_init(void)
{
	init_timer(&blink_timer);
	blink_timer.function = &led_timer_handler;

	if (gpio_request(gpio_pin, "BlinkyLED") != 0)
		return 1;

	if (gpio_direction_output(gpio_pin, 0) != 0)
		return 2;

	blink_timer.expires = jiffies + 100;
	add_timer(&blink_timer);

	return 0;
}

static void __exit led_exit(void)
{
	del_timer(&blink_timer);
	gpio_set_value(gpio_pin, 0);
	gpio_free(gpio_pin);
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("aschult5");
MODULE_DESCRIPTION("rPi ACT LED blinky");
MODULE_VERSION("2.0");
