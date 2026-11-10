// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>

extern int __init hyperframe_init(void);

/* Supply CFI metadata without changing the opaque assembly initialization. */
device_initcall(hyperframe_init);
