/*
 * Target Framework Protocol definitions
 *
 * (C) 2005 FUJITA Tomonori <tomof@acm.org>
 * (C) 2005 Mike Christie <michaelc@cs.wisc.edu>
 * This code is licenced under the GPL.
 */
#ifndef __TGT_PROTOCOL_H
#define __TGT_PROTOCOL_H

#include <linux/slab.h>
#include <linux/dma-mapping.h>

struct module;
struct tgt_cmd;
struct tgt_session;

/*
 * The target driver will interact with tgt core through the protocol
 * handler. The protocol handler can then use the default tgt_core functions
 * or build wrappers around them.
 */
struct tgt_protocol {
	const char *name;
	struct module *module;

	kmem_cache_t *cmd_cache;
	int uspace_pdu_size;

	/*
	 * create a command and allocate a buffer of size data_len for
	 * for transfer. The buffer will be allocated with GFP_KERNEL
	 * so if you cannot sleep the caller must pass in a done() function.
	 * The done function will be called from process context.
	 *
	 * TODO: This dual behavior is a little strange. We will convert
	 * iet to open-iscsi's model so eventually the done() function
	 * will be a requirement so we can have a common path.
	 */
	struct tgt_cmd *(* create_cmd)(struct tgt_session *session,
					void *tgt_priv, uint8_t *cmd,
					uint32_t data_len,
					enum dma_data_direction data_dir,
					uint8_t *dev_id_buff, int id_buff_size,
					void (*done)(struct tgt_cmd *));
	/*
	 * destroy a command. This will free the command and buffer
	 */
	void (* destroy_cmd)(struct tgt_cmd *cmd); 
	/*
	 * queue a command to be executed in a workqueue. A done() callback
	 * must be passed in.
	 */
	int (* queue_cmd)(struct tgt_cmd *cmd,
			   void (*done)(struct tgt_cmd *));
	/*
	 * build userspace packet
	 */
	void (* build_uspace_pdu)(struct tgt_cmd *cmd, void *data);
};

extern void tgt_protocol_init(void);
extern int tgt_protocol_register(struct tgt_protocol *proto);
extern void tgt_protocol_unregister(struct tgt_protocol *proto);
extern struct tgt_protocol *tgt_protocol_get(const char *name);
extern void tgt_protocol_put(struct tgt_protocol *proto);

#endif