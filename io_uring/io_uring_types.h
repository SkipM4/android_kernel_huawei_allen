/* SPDX-License-Identifier: GPL-2.0 */
#ifndef IO_URING_TYPES_H
#define IO_URING_TYPES_H

#include <linux/bitmap.h>
#include <linux/blkdev.h>
#include <linux/completion.h>
#include <linux/eventpoll.h>
#include <linux/hrtimer.h>
#include <linux/io_uring.h>
#include <linux/llist.h>
#include <linux/mutex.h>
#include <linux/net.h>
#include <linux/openat2.h>
#include <linux/pagemap.h>
#include <linux/percpu-refcount.h>
#include <linux/percpu_counter.h>
#include <linux/refcount.h>
#include <linux/socket.h>
#include <linux/task_work.h>
#include <linux/uio.h>
#include <linux/workqueue.h>
#include <linux/xarray.h>
#include <uapi/linux/io_uring.h>
#include "io-wq.h"

enum {
#ifdef CONFIG_HYPERFRAME
	IO_OP_HYPERFRAME_OPEN = IORING_OP_LAST,
	IO_OP_HYPERFRAME_WRITE,
	IO_OP_HYPERFRAME_READ,
	IO_OP_HYPERFRAME_CLOSE,
	IO_OP_LAST,
#else
	IO_OP_LAST = IORING_OP_LAST,
#endif
};

struct io_uring {
	u32 head ____cacheline_aligned_in_smp;
	u32 tail ____cacheline_aligned_in_smp;
};

/*
 * This data is shared with the application through the mmap at offsets
 * IORING_OFF_SQ_RING and IORING_OFF_CQ_RING.
 *
 * The offsets to the member fields are published through struct
 * io_sqring_offsets when calling io_uring_setup.
 */
struct io_rings {
	/*
	 * Head and tail offsets into the ring; the offsets need to be
	 * masked to get valid indices.
	 *
	 * The kernel controls head of the sq ring and the tail of the cq ring,
	 * and the application controls tail of the sq ring and the head of the
	 * cq ring.
	 */
	struct io_uring		sq, cq;
	/*
	 * Bitmasks to apply to head and tail offsets (constant, equals
	 * ring_entries - 1)
	 */
	u32			sq_ring_mask, cq_ring_mask;
	/* Ring sizes (constant, power of 2) */
	u32			sq_ring_entries, cq_ring_entries;
	/*
	 * Number of invalid entries dropped by the kernel due to
	 * invalid index stored in array
	 *
	 * Written by the kernel, shouldn't be modified by the
	 * application (i.e. get number of "new events" by comparing to
	 * cached value).
	 *
	 * After a new SQ head value was read by the application this
	 * counter includes all submissions that were dropped reaching
	 * the new SQ head (and possibly more).
	 */
	u32			sq_dropped;
	/*
	 * Runtime SQ flags
	 *
	 * Written by the kernel, shouldn't be modified by the
	 * application.
	 *
	 * The application needs a full memory barrier before checking
	 * for IORING_SQ_NEED_WAKEUP after updating the sq tail.
	 */
	u32			sq_flags;
	/*
	 * Runtime CQ flags
	 *
	 * Written by the application, shouldn't be modified by the
	 * kernel.
	 */
	u32			cq_flags;
	/*
	 * Number of completion events lost because the queue was full;
	 * this should be avoided by the application by making sure
	 * there are not more requests pending than there is space in
	 * the completion queue.
	 *
	 * Written by the kernel, shouldn't be modified by the
	 * application (i.e. get number of "new events" by comparing to
	 * cached value).
	 *
	 * As completion events come in out of order this counter is not
	 * ordered with any other data.
	 */
	u32			cq_overflow;
	/*
	 * Ring buffer of completion events.
	 *
	 * The kernel writes completion events fresh every time they are
	 * produced, so the application is allowed to modify pending
	 * entries.
	 */
	struct io_uring_cqe	cqes[] ____cacheline_aligned_in_smp;
};

enum io_uring_cmd_flags {
	IO_URING_F_NONBLOCK		= 1,
	IO_URING_F_COMPLETE_DEFER	= 2,
};

struct io_mapped_ubuf {
	u64		ubuf;
	u64		ubuf_end;
	unsigned int	nr_bvecs;
	unsigned long	acct_pages;
	struct bio_vec	bvec[];
};

struct io_ring_ctx;

struct io_overflow_cqe {
	struct io_uring_cqe cqe;
	struct list_head list;
};

struct io_fixed_file {
	/* file * with additional FFS_* flags */
	unsigned long file_ptr;
};

struct io_rsrc_put {
	struct list_head list;
	u64 tag;
	union {
		void *rsrc;
		struct file *file;
		struct io_mapped_ubuf *buf;
	};
};

struct io_file_table {
	struct io_fixed_file *files;
};

struct io_rsrc_node {
	struct percpu_ref		refs;
	struct list_head		node;
	struct list_head		rsrc_list;
	struct io_rsrc_data		*rsrc_data;
	struct llist_node		llist;
	bool				done;
};

typedef void (rsrc_put_fn)(struct io_ring_ctx *ctx, struct io_rsrc_put *prsrc);

struct io_rsrc_data {
	struct io_ring_ctx		*ctx;

	u64				**tags;
	unsigned int			nr;
	rsrc_put_fn			*do_put;
	atomic_t			refs;
	struct completion		done;
	bool				quiesce;
};

struct io_buffer {
	struct list_head list;
	__u64 addr;
	__u32 len;
	__u16 bid;
};

struct io_restriction {
	DECLARE_BITMAP(register_op, IORING_REGISTER_LAST);
	DECLARE_BITMAP(sqe_op, IO_OP_LAST);
	u8 sqe_flags_allowed;
	u8 sqe_flags_required;
	bool registered;
};

enum {
	IO_SQ_THREAD_SHOULD_STOP = 0,
	IO_SQ_THREAD_SHOULD_PARK,
};

struct io_sq_data {
	refcount_t		refs;
	atomic_t		park_pending;
	struct mutex		lock;

	/* ctx's that are using this sqd */
	struct list_head	ctx_list;

	struct task_struct	*thread;
	struct wait_queue_head	wait;

	unsigned		sq_thread_idle;
	int			sq_cpu;
	pid_t			task_pid;
	pid_t			task_tgid;

	unsigned long		state;
	struct completion	exited;
};

#define IO_COMPL_BATCH			32
#define IO_REQ_CACHE_SIZE		32
#define IO_REQ_ALLOC_BATCH		8

struct io_submit_link {
	struct io_kiocb		*head;
	struct io_kiocb		*last;
};

struct io_submit_state {
	struct blk_plug		plug;
	struct io_submit_link	link;

	/*
	 * io_kiocb alloc cache
	 */
	void			*reqs[IO_REQ_CACHE_SIZE];
	unsigned int		free_reqs;

	bool			plug_started;

	/*
	 * Batch completion logic
	 */
	struct io_kiocb		*compl_reqs[IO_COMPL_BATCH];
	unsigned int		compl_nr;
	/* inline/task_work completion list, under ->uring_lock */
	struct list_head	free_list;

	unsigned int		ios_left;
};

struct io_ring_ctx {
	/* const or read-mostly hot data */
	struct {
		struct percpu_ref	refs;

		struct io_rings		*rings;
		unsigned int		flags;
		unsigned int		compat: 1;
		unsigned int		drain_next: 1;
		unsigned int		eventfd_async: 1;
		unsigned int		restricted: 1;
		unsigned int		off_timeout_used: 1;
		unsigned int		drain_active: 1;
	} ____cacheline_aligned_in_smp;

	/* submission data */
	struct {
		struct mutex		uring_lock;

		/*
		 * Ring buffer of indices into array of io_uring_sqe, which is
		 * mmapped by the application using the IORING_OFF_SQES offset.
		 *
		 * This indirection could e.g. be used to assign fixed
		 * io_uring_sqe entries to operations and only submit them to
		 * the queue when needed.
		 *
		 * The kernel modifies neither the indices array nor the entries
		 * array.
		 */
		u32			*sq_array;
		struct io_uring_sqe	*sq_sqes;
		unsigned		cached_sq_head;
		unsigned		sq_entries;
		struct list_head	defer_list;

		/*
		 * Fixed resources fast path, should be accessed only under
		 * uring_lock, and updated through io_uring_register(2)
		 */
		struct io_rsrc_node	*rsrc_node;
		struct io_file_table	file_table;
		unsigned		nr_user_files;
		unsigned		nr_user_bufs;
		struct io_mapped_ubuf	**user_bufs;

		struct io_submit_state	submit_state;
		struct list_head	timeout_list;
		struct list_head	ltimeout_list;
		struct list_head	cq_overflow_list;
		struct xarray		io_buffers;
		struct xarray		personalities;
		u32			pers_next;
		unsigned		sq_thread_idle;
	} ____cacheline_aligned_in_smp;

	/* IRQ completion list, under ->completion_lock */
	struct list_head	locked_free_list;
	unsigned int		locked_free_nr;

	const struct cred	*sq_creds;	/* cred used for __io_sq_thread() */
	struct io_sq_data	*sq_data;	/* if using sq thread polling */

	struct wait_queue_head	sqo_sq_wait;
	struct list_head	sqd_list;

	unsigned long		check_cq_overflow;

	struct {
		unsigned		cached_cq_tail;
		unsigned		cq_entries;
		struct eventfd_ctx	*cq_ev_fd;
		struct wait_queue_head	poll_wait;
		struct wait_queue_head	cq_wait;
		unsigned		cq_extra;
		atomic_t		cq_timeouts;
		unsigned		cq_last_tm_flush;
	} ____cacheline_aligned_in_smp;

	struct {
		spinlock_t		completion_lock;

		spinlock_t		timeout_lock;

		/*
		 * ->iopoll_list is protected by the ctx->uring_lock for
		 * io_uring instances that don't use IORING_SETUP_SQPOLL.
		 * For SQPOLL, only the single threaded io_sq_thread() will
		 * manipulate the list, hence no extra locking is needed there.
		 */
		struct list_head	iopoll_list;
		struct hlist_head	*cancel_hash;
		unsigned		cancel_hash_bits;
		bool			poll_multi_queue;
	} ____cacheline_aligned_in_smp;

	struct io_restriction		restrictions;

	/* slow path rsrc auxilary data, used by update/register */
	struct {
		struct io_rsrc_node		*rsrc_backup_node;
		struct io_mapped_ubuf		*dummy_ubuf;
		struct io_rsrc_data		*file_data;
		struct io_rsrc_data		*buf_data;

		struct delayed_work		rsrc_put_work;
		struct llist_head		rsrc_put_llist;
		struct list_head		rsrc_ref_list;
		spinlock_t			rsrc_ref_lock;
	};

	/* Keep this last, we don't need it for the fast path */
	struct {
		/* hashed buffered write serialization */
		struct io_wq_hash		*hash_map;

		/* Only used for accounting purposes */
		struct user_struct		*user;
		struct mm_struct		*mm_account;

		/* ctx exit and cancelation */
		struct llist_head		fallback_llist;
		struct delayed_work		fallback_work;
		struct work_struct		exit_work;
		struct list_head		tctx_list;
		struct completion		ref_comp;
		u32				iowq_limits[2];
		bool				iowq_limits_set;
	};
#ifdef CONFIG_HYPERFRAME
	void				*buf;
#endif
};

struct io_uring_task {
	/* submission side */
	int			cached_refs;
	struct xarray		xa;
	struct wait_queue_head	wait;
	const struct io_ring_ctx *last;
	struct io_wq		*io_wq;
	struct percpu_counter	inflight;
	atomic_t		inflight_tracked;
	atomic_t		in_idle;

	spinlock_t		task_lock;
	struct io_wq_work_list	task_list;
	struct callback_head	task_work;
	bool			task_running;
};

/*
 * First field must be the file pointer in all the
 * iocb unions! See also 'struct kiocb' in <linux/fs.h>
 */
struct io_poll_iocb {
	struct file			*file;
	struct wait_queue_head		*head;
	__poll_t			events;
	int				retries;
	struct wait_queue_entry		wait;
};

struct io_poll_update {
	struct file			*file;
	u64				old_user_data;
	u64				new_user_data;
	__poll_t			events;
	bool				update_events;
	bool				update_user_data;
};

struct io_close {
	struct file			*file;
	int				fd;
	u32				file_slot;
};

struct io_timeout_data {
	struct io_kiocb			*req;
	struct hrtimer			timer;
	struct timespec64		ts;
	enum hrtimer_mode		mode;
	u32				flags;
};

struct io_accept {
	struct file			*file;
	struct sockaddr __user		*addr;
	int __user			*addr_len;
	int				flags;
	u32				file_slot;
	unsigned long			nofile;
};

struct io_sync {
	struct file			*file;
	loff_t				len;
	loff_t				off;
	int				flags;
	int				mode;
};

struct io_cancel {
	struct file			*file;
	u64				addr;
};

struct io_timeout {
	struct file			*file;
	u32				off;
	u32				target_seq;
	struct list_head		list;
	/* head of the link, used by linked timeouts only */
	struct io_kiocb			*head;
	/* for linked completions */
	struct io_kiocb			*prev;
};

struct io_timeout_rem {
	struct file			*file;
	u64				addr;

	/* timeout update */
	struct timespec64		ts;
	u32				flags;
	bool				ltimeout;
};

struct io_rw {
	/* NOTE: kiocb has the file as the first member, so don't do it here */
	struct kiocb			kiocb;
	u64				addr;
	u64				len;
};

struct io_connect {
	struct file			*file;
	struct sockaddr __user		*addr;
	int				addr_len;
};

struct io_sr_msg {
	struct file			*file;
	union {
		struct compat_msghdr __user	*umsg_compat;
		struct user_msghdr __user	*umsg;
		void __user			*buf;
	};
	int				msg_flags;
	int				bgid;
	size_t				len;
	size_t				done_io;
	struct io_buffer		*kbuf;
	void __user			*msg_control;
};

struct io_open {
	struct file			*file;
	int				dfd;
	u32				file_slot;
	struct filename			*filename;
	struct open_how			how;
	unsigned long			nofile;
};

struct io_rsrc_update {
	struct file			*file;
	u64				arg;
	u32				nr_args;
	u32				offset;
};

struct io_fadvise {
	struct file			*file;
	u64				offset;
	u32				len;
	u32				advice;
};

struct io_madvise {
	struct file			*file;
	u64				addr;
	u32				len;
	u32				advice;
};

struct io_epoll {
	struct file			*file;
	int				epfd;
	int				op;
	int				fd;
	struct epoll_event		event;
};

struct io_splice {
	struct file			*file_out;
	loff_t				off_out;
	loff_t				off_in;
	u64				len;
	int				splice_fd_in;
	unsigned int			flags;
};

struct io_provide_buf {
	struct file			*file;
	__u64				addr;
	__u32				len;
	__u32				bgid;
	__u16				nbufs;
	__u16				bid;
};

struct io_statx {
	struct file			*file;
	int				dfd;
	unsigned int			mask;
	unsigned int			flags;
	const char __user		*filename;
	struct statx __user		*buffer;
};

struct io_shutdown {
	struct file			*file;
	int				how;
};

struct io_rename {
	struct file			*file;
	int				old_dfd;
	int				new_dfd;
	struct filename			*oldpath;
	struct filename			*newpath;
	int				flags;
};

struct io_unlink {
	struct file			*file;
	int				dfd;
	int				flags;
	struct filename			*filename;
};

struct io_mkdir {
	struct file			*file;
	int				dfd;
	umode_t				mode;
	struct filename			*filename;
};

struct io_symlink {
	struct file			*file;
	int				new_dfd;
	struct filename			*oldpath;
	struct filename			*newpath;
};

struct io_hardlink {
	struct file			*file;
	int				old_dfd;
	int				new_dfd;
	struct filename			*oldpath;
	struct filename			*newpath;
	int				flags;
};

struct io_completion {
	struct file			*file;
	u32				cflags;
};

struct io_async_connect {
	struct sockaddr_storage		address;
};

struct io_async_msghdr {
	struct iovec			fast_iov[UIO_FASTIOV];
	/* points to an allocated iov, if NULL we use fast_iov instead */
	struct iovec			*free_iov;
	struct sockaddr __user		*uaddr;
	struct msghdr			msg;
	struct sockaddr_storage		addr;
};

struct io_async_rw {
	struct iovec			fast_iov[UIO_FASTIOV];
	const struct iovec		*free_iovec;
	struct iov_iter			iter;
	struct iov_iter_state		iter_state;
	size_t				bytes_done;
	struct wait_page_queue		wpq;
};

enum {
	REQ_F_FIXED_FILE_BIT	= IOSQE_FIXED_FILE_BIT,
	REQ_F_IO_DRAIN_BIT	= IOSQE_IO_DRAIN_BIT,
	REQ_F_LINK_BIT		= IOSQE_IO_LINK_BIT,
	REQ_F_HARDLINK_BIT	= IOSQE_IO_HARDLINK_BIT,
	REQ_F_FORCE_ASYNC_BIT	= IOSQE_ASYNC_BIT,
	REQ_F_BUFFER_SELECT_BIT	= IOSQE_BUFFER_SELECT_BIT,

	/* first byte is taken by user flags, shift it to not overlap */
	REQ_F_FAIL_BIT		= 8,
	REQ_F_INFLIGHT_BIT,
	REQ_F_CUR_POS_BIT,
	REQ_F_NOWAIT_BIT,
	REQ_F_LINK_TIMEOUT_BIT,
	REQ_F_NEED_CLEANUP_BIT,
	REQ_F_POLLED_BIT,
	REQ_F_BUFFER_SELECTED_BIT,
	REQ_F_COMPLETE_INLINE_BIT,
	REQ_F_REISSUE_BIT,
	REQ_F_CREDS_BIT,
	REQ_F_REFCOUNT_BIT,
	REQ_F_ARM_LTIMEOUT_BIT,
	REQ_F_PARTIAL_IO_BIT,
	/* keep async read/write and isreg together and in order */
	REQ_F_NOWAIT_READ_BIT,
	REQ_F_NOWAIT_WRITE_BIT,
	REQ_F_ISREG_BIT,
#ifdef CONFIG_SCSI_UFS_ENHANCED_INLINE_CRYPTO_V3
	REQ_F_CRYPTO_IO_BIT,
#endif

	/* not a real bit, just to check we're not overflowing the space */
	__REQ_F_LAST_BIT,
};

enum {
	/* ctx owns file */
	REQ_F_FIXED_FILE	= BIT(REQ_F_FIXED_FILE_BIT),
	/* drain existing IO first */
	REQ_F_IO_DRAIN		= BIT(REQ_F_IO_DRAIN_BIT),
	/* linked sqes */
	REQ_F_LINK		= BIT(REQ_F_LINK_BIT),
	/* doesn't sever on completion < 0 */
	REQ_F_HARDLINK		= BIT(REQ_F_HARDLINK_BIT),
	/* IOSQE_ASYNC */
	REQ_F_FORCE_ASYNC	= BIT(REQ_F_FORCE_ASYNC_BIT),
	/* IOSQE_BUFFER_SELECT */
	REQ_F_BUFFER_SELECT	= BIT(REQ_F_BUFFER_SELECT_BIT),

	/* fail rest of links */
	REQ_F_FAIL		= BIT(REQ_F_FAIL_BIT),
	/* on inflight list, should be cancelled and waited on exit reliably */
	REQ_F_INFLIGHT		= BIT(REQ_F_INFLIGHT_BIT),
	/* read/write uses file position */
	REQ_F_CUR_POS		= BIT(REQ_F_CUR_POS_BIT),
	/* must not punt to workers */
	REQ_F_NOWAIT		= BIT(REQ_F_NOWAIT_BIT),
	/* has or had linked timeout */
	REQ_F_LINK_TIMEOUT	= BIT(REQ_F_LINK_TIMEOUT_BIT),
	/* needs cleanup */
	REQ_F_NEED_CLEANUP	= BIT(REQ_F_NEED_CLEANUP_BIT),
	/* already went through poll handler */
	REQ_F_POLLED		= BIT(REQ_F_POLLED_BIT),
	/* buffer already selected */
	REQ_F_BUFFER_SELECTED	= BIT(REQ_F_BUFFER_SELECTED_BIT),
	/* completion is deferred through io_comp_state */
	REQ_F_COMPLETE_INLINE	= BIT(REQ_F_COMPLETE_INLINE_BIT),
	/* caller should reissue async */
	REQ_F_REISSUE		= BIT(REQ_F_REISSUE_BIT),
	/* supports async reads */
	REQ_F_NOWAIT_READ	= BIT(REQ_F_NOWAIT_READ_BIT),
	/* supports async writes */
	REQ_F_NOWAIT_WRITE	= BIT(REQ_F_NOWAIT_WRITE_BIT),
	/* regular file */
	REQ_F_ISREG		= BIT(REQ_F_ISREG_BIT),
	/* has creds assigned */
	REQ_F_CREDS		= BIT(REQ_F_CREDS_BIT),
	/* skip refcounting if not set */
	REQ_F_REFCOUNT		= BIT(REQ_F_REFCOUNT_BIT),
	/* there is a linked timeout that has to be armed */
	REQ_F_ARM_LTIMEOUT	= BIT(REQ_F_ARM_LTIMEOUT_BIT),
	/* request has already done partial IO */
	REQ_F_PARTIAL_IO	= BIT(REQ_F_PARTIAL_IO_BIT),
#ifdef CONFIG_SCSI_UFS_ENHANCED_INLINE_CRYPTO_V3
	REQ_F_CRYPTO_IO		= BIT(REQ_F_CRYPTO_IO_BIT),
#endif
};

struct async_poll {
	struct io_poll_iocb	poll;
	struct io_poll_iocb	*double_poll;
};

typedef void (*io_req_tw_func_t)(struct io_kiocb *req, bool *locked);

struct io_task_work {
	union {
		struct io_wq_work_node	node;
		struct llist_node	fallback_node;
	};
	io_req_tw_func_t		func;
};

enum {
	IORING_RSRC_FILE		= 0,
	IORING_RSRC_BUFFER		= 1,
};

/*
 * NOTE! Each of the iocb union members has the file pointer
 * as the first entry in their struct definition. So you can
 * access the file pointer through any of the sub-structs,
 * or directly as just 'ki_filp' in this struct.
 */
struct io_kiocb {
	union {
		struct file		*file;
		struct io_rw		rw;
		struct io_poll_iocb	poll;
		struct io_poll_update	poll_update;
		struct io_accept	accept;
		struct io_sync		sync;
		struct io_cancel	cancel;
		struct io_timeout	timeout;
		struct io_timeout_rem	timeout_rem;
		struct io_connect	connect;
		struct io_sr_msg	sr_msg;
		struct io_open		open;
		struct io_close		close;
		struct io_rsrc_update	rsrc_update;
		struct io_fadvise	fadvise;
		struct io_madvise	madvise;
		struct io_epoll		epoll;
		struct io_splice	splice;
		struct io_provide_buf	pbuf;
		struct io_statx		statx;
		struct io_shutdown	shutdown;
		struct io_rename	rename;
		struct io_unlink	unlink;
		struct io_mkdir		mkdir;
		struct io_symlink	symlink;
		struct io_hardlink	hardlink;
		/* use only after cleaning per-op data, see io_clean_op() */
		struct io_completion	compl;
	};

	/* opcode allocated if it needs to store data for async defer */
	void				*async_data;
	u8				opcode;
	/* polled IO has completed */
	u8				iopoll_completed;

	u16				buf_index;
	u32				result;

	struct io_ring_ctx		*ctx;
	unsigned int			flags;
	atomic_t			refs;
	struct task_struct		*task;
	u64				user_data;

	struct io_kiocb			*link;
	struct percpu_ref		*fixed_rsrc_refs;

	/* used with ctx->iopoll_list with reads/writes */
	struct list_head		inflight_entry;
	struct io_task_work		io_task_work;
	/* for polled requests, i.e. IORING_OP_POLL_ADD and async armed poll */
	struct hlist_node		hash_node;
	struct async_poll		*apoll;
	struct io_wq_work		work;
	const struct cred		*creds;

	/* store used ubuf, so we can prevent reloading */
	struct io_mapped_ubuf		*imu;
	/* stores selected buf, valid IFF REQ_F_BUFFER_SELECTED is set */
	struct io_buffer		*kbuf;
	atomic_t			poll_refs;
#ifdef CONFIG_HYPERFRAME
	void				*buf;
#endif
};

struct io_tctx_node {
	struct list_head	ctx_node;
	struct task_struct	*task;
	struct io_ring_ctx	*ctx;
};

struct io_defer_entry {
	struct list_head	list;
	struct io_kiocb		*req;
	u32			seq;
};

#endif
