/******************************************************************************
*
* Copyright (C) 2015 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file xhdcp22_rng.h
* @addtogroup hdcp22_v1_0
* @{
*
* This header file contains identifiers and register-level core functions (or
* macros) that can be used to access the Xilinx HDCP22 Random Number Generator
* (RNG) core.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date     Changes
* ----- ------ -------- --------------------------------------------------
* 1.00  jo  10/01/15 Initial release.
* </pre>
*
******************************************************************************/

#ifndef XHDCP22_RNG_H
/**< Prevent circular inclusions by using protection macros */
#define XHDCP22_RNG_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#include "xhdcp22_rng_hw.h"
#include "xil_assert.h"
#include "xstatus.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/
/**
* This typedef contains configuration information for the HDCP22 Rng core.
* Each HDCP22 Rng device should have a configuration structure associated.
*/
typedef struct {
	u16 DeviceId;     /**< DeviceId is the unique ID of the HDCP22 Rng core */
	u32 BaseAddress;  /**< BaseAddress is the physical base address of the core's registers */
} XHdcp22_Rng_Config;

/**
* The XHdcp22 Rng driver instance data. An instance must be allocated for each
* HDCP22 Rng core in use.
*/
typedef struct {
	XHdcp22_Rng_Config Config; /**< Hardware Configuration */
	u32 IsReady;                  /**< Core and the driver instance are initialized */
} XHdcp22_Rng;

/***************** Macros (Inline Functions) Definitions *********************/

/*****************************************************************************/
/**
*
* This macro enables the HDCP22 RNG peripheral.
*
* @param  InstancePtr is a pointer to the HDCP22 RNG core instance.
*
* @return None.
*
* @note   C-style signature:
*         void XHdcp22Rng_Enable(u32 BaseAddress)
*
******************************************************************************/
#define XHdcp22Rng_Enable(InstancePtr) \
        XHdcp22Rng_WriteReg((InstancePtr)->Config.BaseAddress, \
       (XHDCP22_RNG_REG_CTRL_SET_OFFSET), (XHDCP22_RNG_REG_CTRL_RUN_MASK))

/*****************************************************************************/
/**
*
* This macro disables the HDCP22 RNG peripheral.
*
* @param  InstancePtr is a pointer to the HDCP22 RNG core instance.
*
* @return None.
*
* @note   C-style signature:
*         void XHdcp22Rng_Disable(u32 BaseAddress)
*
******************************************************************************/
#define XHdcp22Rng_Disable(InstancePtr) \
        XHdcp22Rng_WriteReg((InstancePtr)->Config.BaseAddress, \
        (XHDCP22_RNG_REG_CTRL_CLR_OFFSET), (XHDCP22_RNG_REG_CTRL_RUN_MASK))

/*****************************************************************************/
/**
*
* This macro returns the enabled state of HDCP22 RNG.
* for the HDCP22 RNG peripheral.
* @param  InstancePtr is a pointer to the HDCP22 RNG core instance.
*
* @return TRUE if HDCP22 RNG is enabled, FALSE otherwise.
*
* @note   C-style signature:
*         u32 XHdcp22Rng_IsEnabled(u32 BaseAddress)
*
******************************************************************************/
#define XHdcp22Rng_IsEnabled(InstancePtr) \
        ((XHdcp22Rng_GetControlReg((InstancePtr)->Config.BaseAddress)\
        & XHDCP22_RNG_REG_CTRL_RUN_MASK) ==  XHDCP22_RNG_REG_CTRL_RUN_MASK)


/************************** Function Prototypes ******************************/
/* Initialization function in xhdcp22_rng_sinit.c */
XHdcp22_Rng_Config *XHdcp22Rng_LookupConfig(u16 DeviceId);

/* Initialization and control functions in xhdcp22_cipher.c */
int XHdcp22Rng_CfgInitialize(XHdcp22_Rng *InstancePtr, XHdcp22_Rng_Config *CfgPtr, u32 EffectiveAddr);

/* Return a random number */
void XHdcp22Rng_GetRandom(XHdcp22_Rng *InstancePtr, u8 *BufferPtr, u16 BufferLength, u16 RandomLength);

/************************** Variable Declarations ****************************/

#ifdef __cplusplus
}
#endif

#endif /* XHDCP22_RNG_H */
/** @} */