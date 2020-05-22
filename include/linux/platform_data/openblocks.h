#ifndef __OPENBLOCKS_H__
#define __OPENBLOCKS_H__

#include <linux/of_address.h>
#include <linux/of_gpio.h>

#if defined(CONFIG_MACH_OBSAX3)
static inline int obs_blink_led(void)
{
	struct device_node *np;
	struct resource res;
	unsigned int __iomem *p;

	if(!of_machine_is_compatible("marvell,armadaxp-mv78260")){
		return -ENODEV;
	}

	if(!(np = of_find_compatible_node(NULL, NULL, "marvell,orion-gpio-blon"))){
		return -ENODEV;
	}
	if(of_address_to_resource(np, 0, &res)){
		of_node_put(np);
		return -ENODEV;
	}
	if((p = ioremap(res.start, resource_size(&res))) == NULL){
		of_node_put(np);
		return -ENOMEM;
	}
	*p = 0x10000000;
	iounmap(p);
	of_node_put(np);

	if(!(np = of_find_compatible_node(NULL, NULL, "marvell,orion-gpio-bloff"))){
		return -ENODEV;
	}
	if(of_address_to_resource(np, 0, &res)){
		of_node_put(np);
		return -ENODEV;
	}
	if((p = ioremap(res.start, resource_size(&res))) == NULL){
		of_node_put(np);
		return -ENOMEM;
	}
	*p = 0x10000000;
	iounmap(p);
	of_node_put(np);

	if(!(np = of_find_compatible_node(NULL, NULL, "marvell,orion-gpio-blen"))){
		return -ENODEV;
	}
	if(of_address_to_resource(np, 0, &res)){
		of_node_put(np);
		return -ENODEV;
	}
	if((p = ioremap(res.start, resource_size(&res))) == NULL){
		of_node_put(np);
		return -ENOMEM;
	}
	*p = 0x002A0000;
	iounmap(p);
	of_node_put(np);

	return 0;
}
#elif defined(CONFIG_MACH_OBSA7)
static inline int obs_blink_led(int val)
{
	struct device_node *np, *child;
	unsigned int gpio_led;

	if(!(np = of_find_compatible_node(NULL, NULL, "gpio-leds"))){
		return -ENODEV;
	}
	for_each_child_of_node(np, child){
		gpio_led = of_get_gpio(child, 0);
		gpio_set_value(gpio_led, val);
	}

	of_node_put(np);

	return 0;
}
#endif

#endif
