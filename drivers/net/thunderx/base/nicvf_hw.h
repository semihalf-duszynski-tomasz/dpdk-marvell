/*
 *   BSD LICENSE
 *
 *   Copyright (C) Cavium networks Ltd. 2016.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Cavium networks nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _THUNDERX_NICVF_HW_H
#define _THUNDERX_NICVF_HW_H

#include <stdint.h>

#include "nicvf_hw_defs.h"

#define	PCI_VENDOR_ID_CAVIUM			0x177D
#define	PCI_DEVICE_ID_THUNDERX_PASS1_NICVF	0x0011
#define	PCI_DEVICE_ID_THUNDERX_PASS2_NICVF	0xA034
#define	PCI_SUB_DEVICE_ID_THUNDERX_PASS1_NICVF	0xA11E
#define	PCI_SUB_DEVICE_ID_THUNDERX_PASS2_NICVF	0xA134

#define NICVF_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define NICVF_PASS1	(PCI_SUB_DEVICE_ID_THUNDERX_PASS1_NICVF)
#define NICVF_PASS2	(PCI_SUB_DEVICE_ID_THUNDERX_PASS2_NICVF)

#define NICVF_CAP_TUNNEL_PARSING          (1ULL << 0)

enum nicvf_tns_mode {
	NIC_TNS_BYPASS_MODE,
	NIC_TNS_MODE,
};

enum nicvf_err_e {
	NICVF_OK,
	NICVF_ERR_SET_QS = -8191,/* -8191 */
	NICVF_ERR_RESET_QS,      /* -8190 */
	NICVF_ERR_REG_POLL,      /* -8189 */
	NICVF_ERR_RBDR_RESET,    /* -8188 */
	NICVF_ERR_RBDR_DISABLE,  /* -8187 */
	NICVF_ERR_RBDR_PREFETCH, /* -8186 */
	NICVF_ERR_RBDR_RESET1,   /* -8185 */
	NICVF_ERR_RBDR_RESET2,   /* -8184 */
	NICVF_ERR_RQ_CLAIM,      /* -8183 */
	NICVF_ERR_RQ_PF_CFG,	 /* -8182 */
	NICVF_ERR_RQ_BP_CFG,	 /* -8181 */
	NICVF_ERR_RQ_DROP_CFG,	 /* -8180 */
	NICVF_ERR_CQ_DISABLE,	 /* -8179 */
	NICVF_ERR_CQ_RESET,	 /* -8178 */
	NICVF_ERR_SQ_DISABLE,	 /* -8177 */
	NICVF_ERR_SQ_RESET,	 /* -8176 */
	NICVF_ERR_SQ_PF_CFG,	 /* -8175 */
	NICVF_ERR_LOOPBACK_CFG,  /* -8174 */
	NICVF_ERR_BASE_INIT,     /* -8173 */
	NICVF_ERR_RSS_TBL_UPDATE,/* -8172 */
	NICVF_ERR_RSS_GET_SZ,    /* -8171 */
};

typedef nicvf_phys_addr_t (*rbdr_pool_get_handler)(void *opaque);

struct nicvf_rss_reta_info {
	uint8_t hash_bits;
	uint16_t rss_size;
	uint8_t ind_tbl[NIC_MAX_RSS_IDR_TBL_SIZE];
};

/* Common structs used in DPDK and base layer are defined in DPDK layer */
#include "../nicvf_struct.h"

NICVF_STATIC_ASSERT(sizeof(struct nicvf_rbdr) <= 128);
NICVF_STATIC_ASSERT(sizeof(struct nicvf_txq) <= 128);
NICVF_STATIC_ASSERT(sizeof(struct nicvf_rxq) <= 128);

static inline void
nicvf_reg_write(struct nicvf *nic, uint32_t offset, uint64_t val)
{
	nicvf_addr_write(nic->reg_base + offset, val);
}

static inline uint64_t
nicvf_reg_read(struct nicvf *nic, uint32_t offset)
{
	return nicvf_addr_read(nic->reg_base + offset);
}

static inline uintptr_t
nicvf_qset_base(struct nicvf *nic, uint32_t qidx)
{
	return nic->reg_base + (qidx << NIC_Q_NUM_SHIFT);
}

static inline void
nicvf_queue_reg_write(struct nicvf *nic, uint32_t offset, uint32_t qidx,
		      uint64_t val)
{
	nicvf_addr_write(nicvf_qset_base(nic, qidx) + offset, val);
}

static inline uint64_t
nicvf_queue_reg_read(struct nicvf *nic, uint32_t offset, uint32_t qidx)
{
	return	nicvf_addr_read(nicvf_qset_base(nic, qidx) + offset);
}

static inline void
nicvf_disable_all_interrupts(struct nicvf *nic)
{
	nicvf_reg_write(nic, NIC_VF_ENA_W1C, NICVF_INTR_ALL_MASK);
	nicvf_reg_write(nic, NIC_VF_INT, NICVF_INTR_ALL_MASK);
}

static inline uint32_t
nicvf_hw_version(struct nicvf *nic)
{
	return nic->subsystem_device_id;
}

static inline uint64_t
nicvf_hw_cap(struct nicvf *nic)
{
	return nic->hwcap;
}

int nicvf_base_init(struct nicvf *nic);

int nicvf_reg_get_count(void);
int nicvf_reg_poll_interrupts(struct nicvf *nic);
int nicvf_reg_dump(struct nicvf *nic, uint64_t *data);

int nicvf_qset_config(struct nicvf *nic);
int nicvf_qset_reclaim(struct nicvf *nic);

int nicvf_qset_rbdr_config(struct nicvf *nic, uint16_t qidx);
int nicvf_qset_rbdr_reclaim(struct nicvf *nic, uint16_t qidx);
int nicvf_qset_rbdr_precharge(struct nicvf *nic, uint16_t ridx,
			      rbdr_pool_get_handler handler, void *opaque,
			      uint32_t max_buffs);
int nicvf_qset_rbdr_active(struct nicvf *nic, uint16_t qidx);

int nicvf_qset_rq_config(struct nicvf *nic, uint16_t qidx,
			 struct nicvf_rxq *rxq);
int nicvf_qset_rq_reclaim(struct nicvf *nic, uint16_t qidx);

int nicvf_qset_cq_config(struct nicvf *nic, uint16_t qidx,
			 struct nicvf_rxq *rxq);
int nicvf_qset_cq_reclaim(struct nicvf *nic, uint16_t qidx);

int nicvf_qset_sq_config(struct nicvf *nic, uint16_t qidx,
			 struct nicvf_txq *txq);
int nicvf_qset_sq_reclaim(struct nicvf *nic, uint16_t qidx);

uint32_t nicvf_qsize_rbdr_roundup(uint32_t val);
uint32_t nicvf_qsize_cq_roundup(uint32_t val);
uint32_t nicvf_qsize_sq_roundup(uint32_t val);

void nicvf_vlan_hw_strip(struct nicvf *nic, bool enable);

int nicvf_rss_config(struct nicvf *nic, uint32_t  qcnt, uint64_t cfg);
int nicvf_rss_term(struct nicvf *nic);

int nicvf_rss_reta_update(struct nicvf *nic, uint8_t *tbl, uint32_t max_count);
int nicvf_rss_reta_query(struct nicvf *nic, uint8_t *tbl, uint32_t max_count);

void nicvf_rss_set_key(struct nicvf *nic, uint8_t *key);
void nicvf_rss_get_key(struct nicvf *nic, uint8_t *key);

void nicvf_rss_set_cfg(struct nicvf *nic, uint64_t val);
uint64_t nicvf_rss_get_cfg(struct nicvf *nic);

int nicvf_loopback_config(struct nicvf *nic, bool enable);

#endif /* _THUNDERX_NICVF_HW_H */
