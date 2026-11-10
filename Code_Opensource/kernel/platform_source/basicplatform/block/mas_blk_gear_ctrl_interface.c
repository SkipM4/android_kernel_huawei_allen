// SPDX-License-Identifier: GPL-2.0
#include <linux/workqueue.h>
#include <platform_include/basicplatform/linux/hck/blk/hck_mas_ufs_gear_ctrl.h>

extern void mas_gear_ctrl_work_asm(struct work_struct *work);
extern void hck_mas_queue_busy_change_gear_asm(struct request_queue *q,
		struct request *req);
extern void hck_mas_queue_ioback_change_gear_asm(struct request_queue *q,
		struct request *req);
extern void hck_mas_blk_gear_ctrl_reinit_asm(void *data);

/* Assembly registrations need typed CFI entries, not raw implementation addresses. */
void __cficanonical mas_gear_ctrl_work(struct work_struct *work)
{
	mas_gear_ctrl_work_asm(work);
}
__ADDRESSABLE(mas_gear_ctrl_work)

void __cficanonical hck_mas_queue_busy_change_gear(struct request_queue *q,
		struct request *req)
{
	hck_mas_queue_busy_change_gear_asm(q, req);
}
__ADDRESSABLE(hck_mas_queue_busy_change_gear)

void __cficanonical hck_mas_queue_ioback_change_gear(struct request_queue *q,
		struct request *req)
{
	hck_mas_queue_ioback_change_gear_asm(q, req);
}
__ADDRESSABLE(hck_mas_queue_ioback_change_gear)

void __cficanonical hck_mas_blk_gear_ctrl_reinit(void *data)
{
	hck_mas_blk_gear_ctrl_reinit_asm(data);
}
__ADDRESSABLE(hck_mas_blk_gear_ctrl_reinit)
