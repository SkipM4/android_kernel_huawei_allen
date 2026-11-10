/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: mas block busy idle interface
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "mas_blk_busy_idle_interface.h"

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,19,0))
void __cficanonical __cfi_mas_blk_busyidle_handler_latency_check_timer_expire(
	struct timer_list *timer)
{
	mas_blk_busyidle_handler_latency_check_timer_expire(timer);
}
__ADDRESSABLE(__cfi_mas_blk_busyidle_handler_latency_check_timer_expire)
#else
void __cficanonical __cfi_mas_blk_busyidle_handler_latency_check_timer_expire(
	unsigned long data)
{
	mas_blk_busyidle_handler_latency_check_timer_expire(data);
}
__ADDRESSABLE(__cfi_mas_blk_busyidle_handler_latency_check_timer_expire)
#endif

int __cficanonical __cfi_mas_blk_busyidle_notify_handler(
	struct notifier_block *nb, unsigned long val, void *v)
{
	return mas_blk_busyidle_notify_handler(nb, val, v);
}
__ADDRESSABLE(__cfi_mas_blk_busyidle_notify_handler)

void __cficanonical __cfi_mas_blk_idle_notify_work(struct work_struct *work)
{
	mas_blk_idle_notify_work(work);
}
/* Keep the callback address installed by assembly visible to LTO. */
__ADDRESSABLE(__cfi_mas_blk_idle_notify_work)

void __cfi_mas_blk_busyidle_end_rq(struct request *rq, blk_status_t error)
{
	mas_blk_busyidle_end_rq(rq, error);
}

void mas_blk_busyidle_set_end_io(struct request *rq)
{
	/* Assembly must obtain the CFI callback address through compiled C. */
	rq->end_io = __cfi_mas_blk_busyidle_end_rq;
}

#if defined(CONFIG_MAS_DEBUG_FS) || defined(CONFIG_MAS_BLK_DEBUG)
ssize_t __cfi_mas_queue_busyidle_enable_store(
	struct request_queue *q, const char *page, size_t count)
{
	return mas_queue_busyidle_enable_store(q, page, count);
}

ssize_t __cfi_mas_queue_busyidle_statistic_reset_store(
	struct request_queue *q, const char *page, size_t count)
{
	return mas_queue_busyidle_statistic_reset_store(q, page, count);
}

ssize_t __cfi_mas_queue_busyidle_statistic_show(
	struct request_queue *q, char *page)
{
	return mas_queue_busyidle_statistic_show(q, page, PAGE_SIZE);
}

ssize_t __cfi_mas_queue_hw_idle_enable_show(
	struct request_queue *q, char *page)
{
	return mas_queue_hw_idle_enable_show(q, page, PAGE_SIZE);
}

ssize_t __cfi_mas_queue_idle_state_show(struct request_queue *q, char *page)
{
	return mas_queue_idle_state_show(q, page, PAGE_SIZE);
}
#endif /* CONFIG_MAS_BLK_DEBUG */

