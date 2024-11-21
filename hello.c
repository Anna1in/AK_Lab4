
/*
 * Copyright (c) 2017, GlobalLogic Ukraine LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the GlobalLogic.
 * 4. Neither the name of the GlobalLogic nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GLOBALLOGIC UKRAINE LLC ``AS IS`` AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL GLOBALLOGIC UKRAINE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/ktime.h>

MODULE_AUTHOR("Serhii Popovych <serhii.popovych@globallogic.com>");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");

static unsigned int repeat_count = 1;
module_param(repeat_count, uint, S_IRUGO);
MODULE_PARM_DESC(repeat_count, "Number of times 'Hello, world!' is printed");

struct hello_event {
    struct list_head list;
    ktime_t event_time;
};

static LIST_HEAD(hello_event_list);

static int __init hello_init(void)
{
    int i;
    struct hello_event *event;

    if (repeat_count == 0 || (repeat_count >= 5 && repeat_count <= 10)) {
        printk(KERN_WARNING "Warning: repeat_count is 0 or in range 5-10\n");
    } else if (repeat_count > 10) {
        printk(KERN_ERR "Error: repeat_count is greater than 10\n");
        return -EINVAL;
    }

    for (i = 0; i < repeat_count; i++) {
        event = kmalloc(sizeof(*event), GFP_KERNEL);
        if (!event) {
            printk(KERN_ERR "Failed to allocate memory for event\n");
            return -ENOMEM;
        }

        event->event_time = ktime_get();

        INIT_LIST_HEAD(&event->list);
        list_add_tail(&event->list, &hello_event_list);

        printk(KERN_INFO "Hello, world! (%d)\n", i + 1);
    }

    return 0;
}

static void __exit hello_exit(void)
{
    struct hello_event *event, *tmp;

    list_for_each_entry_safe(event, tmp, &hello_event_list, list) {
        printk(KERN_INFO "Event time: %lld ns\n", ktime_to_ns(event->event_time));

        list_del(&event->list); 

        kfree(event);
    }

    printk(KERN_INFO "Goodbye, world!\n");
}


module_init(hello_init);
module_exit(hello_exit);

