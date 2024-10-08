// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright(C) 2017 MediaTek Inc.
 */

/*! \file   prealloc.c
 *   \brief  memory preallocation module
 *
 *    This file contains all implementations of memory preallocation module
 */

/*******************************************************************************
 *                         C O M P I L E R   F L A G S
 *******************************************************************************
 */

/*******************************************************************************
 *                    E X T E R N A L   R E F E R E N C E S
 *******************************************************************************
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include "precomp.h"

/*******************************************************************************
 *                              C O N S T A N T S
 *******************************************************************************
 */

/*******************************************************************************
 *                             D A T A   T Y P E S
 *******************************************************************************
 */

/*
 * -----------------             ----------------           ----------
 * | PRE_MEM_BLOCK |-pItemArray->| PRE_MEM_ITEM |-pvBuffer->| Memory |
 * |---------------|             |--------------|           ----------
 * | PRE_MEM_BLOCK |->...        | PRE_MEM_ITEM |-pvBuffer->----------
 * |---------------|             |--------------|           | Memory |
 *       .                              .                   ----------
 *       .                              .
 *       .                              .
 */
struct PRE_MEM_ITEM {
	void *pvBuffer;
};

struct PRE_MEM_BLOCK {
	u8 *pucName;
	struct PRE_MEM_ITEM *pItemArray;
	u32 u4Count;
	u32 u4Size;
	u32 u4KmallocFlags;
	u32 u4Curr;
};

/*******************************************************************************
 *                            P U B L I C   D A T A
 *******************************************************************************
 */

/*******************************************************************************
 *                           P R I V A T E   D A T A
 *******************************************************************************
 */

static s32 blockCount;
static struct PRE_MEM_BLOCK arMemBlocks[MEM_ID_NUM];

/*******************************************************************************
 *                                 M A C R O S
 *******************************************************************************
 */

/*******************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 *******************************************************************************
 */

/*******************************************************************************
 *                              F U N C T I O N S
 *******************************************************************************
 */

/*----------------------------------------------------------------------------*/
/*!
 * \brief export function for memory preallocation
 *
 * \param[in] memId memory id.
 *
 * \retval void pointer to the memory address
 */
/*----------------------------------------------------------------------------*/
void *preallocGetMem(enum ENUM_MEM_ID memId)
{
	struct PRE_MEM_BLOCK *block = NULL;
	u32 curr = 0, count = 0;
	u8 *name = NULL;

	/* check memId exist */
	if (memId < 0 || memId >= MEM_ID_NUM) {
		MP_Err("request wrong memId %d", memId);
		return NULL;
	}

	block = &arMemBlocks[memId];
	curr = block->u4Curr;
	count = block->u4Count;
	name = block->pucName;
	block->u4Curr = (curr + 1) % count; /* point to next */

	/* return request memory address */
	MP_Dbg("request [%s], return [%d]\n", name, curr);
	return block->pItemArray[curr].pvBuffer;
}
EXPORT_SYMBOL(preallocGetMem);
static void preallocFree(void)
{
	s32 i = 0, j = 0;
	struct PRE_MEM_BLOCK *block = NULL;
	struct PRE_MEM_ITEM *items = NULL;
	void *memory = NULL;

	for (i = 0; i < MEM_ID_NUM; i++) {
		block = &arMemBlocks[i];
		MP_Info("free [%d], block name=\"%s\" count=%d size=%d\n", i,
			block->pucName, block->u4Count, block->u4Size);
		items = block->pItemArray;
		if (items == NULL)
			continue;
		/* free memory */
		for (j = 0; j < block->u4Count; j++) {
			memory = items[j].pvBuffer;
			MP_Dbg(" - [%d] memory 0x%p\n", j, memory);
			kfree(memory);
		}
		/* free items */
		MP_Dbg(" - items 0x%p\n", items);
		kfree(items);
		memset(block, 0, sizeof(*block));
	}
}

static int preallocAlloc(void)
{
	s32 i = 0, j = 0;
	struct PRE_MEM_BLOCK *block = NULL;
	struct PRE_MEM_ITEM *items = NULL;
	void *memory = NULL;

	for (i = 0; i < MEM_ID_NUM; i++) {
		block = &arMemBlocks[i];
		MP_Info("allocate [%d] block name=\"%s\" count=%d size=%d\n", i,
			block->pucName, block->u4Count, block->u4Size);
		/* allocate u4Count items */
		items = kcalloc(block->u4Count, sizeof(*items), GFP_KERNEL);
		if (items == NULL) {
			MP_Err("allocate [%d] items failed\n", i);
			goto fail;
		}
		MP_Dbg(" + items 0x%p\n", items);
		block->pItemArray = items;
		for (j = 0; j < block->u4Count; j++) {
			/* allocate u4Size memory */
			memory = kmalloc(block->u4Size, block->u4KmallocFlags);
			if (memory == NULL) {
				MP_Err("allocate [%d][%d] memory failed\n", i,
				       j);
				goto fail;
			}
			MP_Dbg(" + [%d] memory 0x%p\n", j, memory);
			items[j].pvBuffer = memory;
		}
	}

	return 0;

fail:
	preallocFree();
	return -ENOMEM;
}

static void preallocAddBlock(enum ENUM_MEM_ID memId, u8 *name, u32 count,
			     u32 size, u32 kmallocFlags)
{
	if (memId != blockCount) {
		MP_Err("memId %d != index %d\n", memId, blockCount);
		return;
	}
	arMemBlocks[blockCount].pucName = name;
	arMemBlocks[blockCount].pItemArray = NULL;
	arMemBlocks[blockCount].u4Count = count;
	arMemBlocks[blockCount].u4Size = size;
	arMemBlocks[blockCount].u4KmallocFlags = kmallocFlags;
	arMemBlocks[blockCount].u4Curr = 0;
	blockCount++;
}

static int __init preallocInit(void)
{
	u32 u4Size;

	blockCount = 0;

	/* ADD BLOCK START, follow the sequence of ENUM_MEM_ID */
	preallocAddBlock(MEM_ID_NIC_ADAPTER, "NIC ADAPTER MEMORY", 1,
			 MGT_BUFFER_SIZE, GFP_KERNEL);

	u4Size = HIF_TX_COALESCING_BUFFER_SIZE > HIF_RX_COALESCING_BUFFER_SIZE ?
		 HIF_TX_COALESCING_BUFFER_SIZE :
		 HIF_RX_COALESCING_BUFFER_SIZE;
	preallocAddBlock(MEM_ID_IO_BUFFER, "IO BUFFER", 1, u4Size, GFP_KERNEL);

	preallocAddBlock(MEM_ID_IO_CTRL, "IO CTRL", 1,
			 sizeof(ENHANCE_MODE_DATA_STRUCT_T), GFP_KERNEL);
	preallocAddBlock(MEM_ID_RX_DATA, "RX DATA", HIF_RX_COALESCING_BUF_COUNT,
			 HIF_RX_COALESCING_BUFFER_SIZE, GFP_KERNEL);

	/* ADD BLOCK END */

	return preallocAlloc();
}

static void __exit preallocExit(void)
{
	preallocFree();
}

module_init(preallocInit);
module_exit(preallocExit);

MODULE_LICENSE("Dual BSD/GPL");
