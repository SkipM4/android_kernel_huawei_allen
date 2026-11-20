// SPDX-License-Identifier: GPL-2.0
#include <platform_include/basicplatform/linux/hck/fs/hck_hyperframe.h>
#include <linux/fs.h>
#include <linux/workqueue.h>

extern void hyperframe_fill_cq_asm(struct kiocb *iocb, long res, long res2);
extern void hyperframe_read_work_asm(struct work_struct *work);
extern void hyperframe_write_work_asm(struct work_struct *work);
extern void hyperframe_fill_cq_work_asm(struct work_struct *work);

void __cficanonical hyperframe_fill_cq(struct kiocb *iocb, long res, long res2)
{
	hyperframe_fill_cq_asm(iocb, res, res2);
}
__ADDRESSABLE(hyperframe_fill_cq)

void __cficanonical hyperframe_read_work(struct work_struct *work)
{
	hyperframe_read_work_asm(work);
}
__ADDRESSABLE(hyperframe_read_work)

void __cficanonical hyperframe_write_work(struct work_struct *work)
{
	hyperframe_write_work_asm(work);
}
__ADDRESSABLE(hyperframe_write_work)

void __cficanonical hyperframe_fill_cq_work(struct work_struct *work)
{
	hyperframe_fill_cq_work_asm(work);
}
__ADDRESSABLE(hyperframe_fill_cq_work)

extern typeof(vh_probe_hck_hyperframe_err_record) hck_hyperframe_err_record_asm;
extern typeof(vh_probe_hck_hyperframe_set_opf) hck_hyperframe_set_opf_asm;
extern typeof(vh_probe_hck_hyperframe_ctx_alloc) hck_hyperframe_ctx_alloc_asm;
extern typeof(vh_probe_hck_hyperframe_ctx_free) hck_hyperframe_ctx_free_asm;
extern typeof(vh_probe_hck_hyperframe_openat_prep) hck_hyperframe_openat_prep_asm;
extern typeof(vh_probe_hck_hyperframe_close_prep) hck_hyperframe_close_prep_asm;
extern typeof(vh_probe_hck_hyperframe_rw_prep) hck_hyperframe_rw_prep_asm;
extern typeof(vh_probe_hck_hyperframe_init_req) hck_hyperframe_init_req_asm;
extern typeof(vh_probe_hck_hyperframe_free_req) hck_hyperframe_free_req_asm;
extern typeof(vh_probe_hck_hyperframe_issue_sqe) hck_hyperframe_issue_sqe_asm;
extern typeof(vh_probe_hck_hyperframe_complete_sqe) hck_hyperframe_complete_sqe_asm;

/* Raw assembly hook registrations need canonical entries with the hook types. */
void __cficanonical hck_hyperframe_err_record(const char *func, int ret,
		int resv1, int resv2, int resv3)
{
	hck_hyperframe_err_record_asm(func, ret, resv1, resv2, resv3);
}
__ADDRESSABLE(hck_hyperframe_err_record)

void __cficanonical hck_hyperframe_set_opf(struct bio *bio, struct kiocb *iocb)
{
	hck_hyperframe_set_opf_asm(bio, iocb);
}
__ADDRESSABLE(hck_hyperframe_set_opf)

void __cficanonical hck_hyperframe_ctx_alloc(struct io_ring_ctx *ctx, int *ret)
{
	hck_hyperframe_ctx_alloc_asm(ctx, ret);
}
__ADDRESSABLE(hck_hyperframe_ctx_alloc)

void __cficanonical hck_hyperframe_ctx_free(struct io_ring_ctx *ctx)
{
	hck_hyperframe_ctx_free_asm(ctx);
}
__ADDRESSABLE(hck_hyperframe_ctx_free)

void __cficanonical hck_hyperframe_openat_prep(struct io_kiocb *req,
		const struct io_uring_sqe *sqe)
{
	hck_hyperframe_openat_prep_asm(req, sqe);
}
__ADDRESSABLE(hck_hyperframe_openat_prep)

void __cficanonical hck_hyperframe_close_prep(struct io_kiocb *req,
		const struct io_uring_sqe *sqe)
{
	hck_hyperframe_close_prep_asm(req, sqe);
}
__ADDRESSABLE(hck_hyperframe_close_prep)

void __cficanonical hck_hyperframe_rw_prep(struct io_kiocb *req,
		const struct io_uring_sqe *sqe, long *ret)
{
	hck_hyperframe_rw_prep_asm(req, sqe, ret);
}
__ADDRESSABLE(hck_hyperframe_rw_prep)

void __cficanonical hck_hyperframe_init_req(struct io_kiocb *req,
		const struct io_uring_sqe *sqe, int *ret)
{
	hck_hyperframe_init_req_asm(req, sqe, ret);
}
__ADDRESSABLE(hck_hyperframe_init_req)

void __cficanonical hck_hyperframe_free_req(struct io_kiocb *req)
{
	hck_hyperframe_free_req_asm(req);
}
__ADDRESSABLE(hck_hyperframe_free_req)

void __cficanonical hck_hyperframe_issue_sqe(struct io_kiocb *req, int *ret)
{
	hck_hyperframe_issue_sqe_asm(req, ret);
}
__ADDRESSABLE(hck_hyperframe_issue_sqe)

void __cficanonical hck_hyperframe_complete_sqe(u8 opcode, int *ret)
{
	hck_hyperframe_complete_sqe_asm(opcode, ret);
}
__ADDRESSABLE(hck_hyperframe_complete_sqe)
