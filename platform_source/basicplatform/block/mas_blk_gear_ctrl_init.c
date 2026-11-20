// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>

extern int __init mas_gear_ctrl_hck_init(void);

/* Let the C initcall macro supply CFI metadata for the opaque assembly body. */
device_initcall(mas_gear_ctrl_hck_init);
