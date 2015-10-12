/******************************************************************************
*
* Copyright (C) 2015 Xilinx, Inc. All rights reserved.
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
* XILINX BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
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
* @file xv_hdmirx.c
*
* This is the main file for Xilinx HDMI RX core. Please see xv_hdmirx.h for
* more details of the driver.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date     Changes
* ----- ------ -------- --------------------------------------------------
* 1.00         10/07/15 Initial release.

* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xv_hdmirx.h"
#include "string.h"

/************************** Constant Definitions *****************************/

/***************** Macros (Inline Functions) Definitions *********************/


/**************************** Type Definitions *******************************/

/**
* This table contains the attributes for various standard resolutions.
* Each entry is of the format:
* 1) Resolution ID
* 2) Video Identification Code.
*/
static const XV_HdmiRx_VicTable VicTable[38] = {
	{XVIDC_VM_640x480_60_P, 1},		// Vic 1
	{XVIDC_VM_720x480_60_P, 2},		// Vic 2
	{XVIDC_VM_720x480_60_P, 3},		// Vic 3
	{XVIDC_VM_1280x720_60_P, 4},	// Vic 4
	{XVIDC_VM_1920x1080_60_I, 5},	// Vic 5
	{XVIDC_VM_1440x480_60_I, 6},	// Vic 6
	{XVIDC_VM_1440x480_60_I, 7},	// Vic 7

	{XVIDC_VM_1920x1080_60_P, 16},	// Vic 16
	{XVIDC_VM_720x576_50_P, 17}, 	// Vic 17
	{XVIDC_VM_720x576_50_P, 18}, 	// Vic 18
	{XVIDC_VM_1280x720_50_P, 19},	// Vic 19
	{XVIDC_VM_1920x1080_50_I, 20},	// Vic 20
	{XVIDC_VM_1440x576_50_I, 21},	// Vic 21
	{XVIDC_VM_1440x576_50_I, 22},	// Vic 22

	// 1680 x 720
	{XVIDC_VM_1680x720_50_P, 82},	// Vic 82
	{XVIDC_VM_1680x720_60_P, 83},	// Vic 83
	{XVIDC_VM_1680x720_100_P, 84},	// Vic 84
	{XVIDC_VM_1680x720_120_P, 85},	// Vic 85

	// 1920 x 1080
	{XVIDC_VM_1920x1080_24_P, 32},	// Vic 32
	{XVIDC_VM_1920x1080_25_P, 33},	// Vic 33
	{XVIDC_VM_1920x1080_30_P, 34},	// Vic 34
	{XVIDC_VM_1920x1080_50_P, 31},	// Vic 31
	{XVIDC_VM_1920x1080_100_P, 64},	// Vic 64
	{XVIDC_VM_1920x1080_120_P, 63},	// Vic 63

	// 2560 x 1080
	{XVIDC_VM_2560x1080_50_P, 89},	// Vic 89
	{XVIDC_VM_2560x1080_60_P, 90},	// Vic 89
	{XVIDC_VM_2560x1080_100_P, 91},	// Vic 91
	{XVIDC_VM_2560x1080_120_P, 92},	// Vic 92

	// 3840 x 2160
	{XVIDC_VM_3840x2160_24_P, 93},	// Vic 93
	{XVIDC_VM_3840x2160_25_P, 94},	// Vic 94
	{XVIDC_VM_3840x2160_30_P, 95},	// Vic 95
	{XVIDC_VM_3840x2160_50_P, 96},	// Vic 96
	{XVIDC_VM_3840x2160_60_P, 97},	// Vic 97

	// 4096 x 2160
	{XVIDC_VM_4096x2160_24_P, 98},	// Vic 98
	{XVIDC_VM_4096x2160_25_P, 99},	// Vic 99
	{XVIDC_VM_4096x2160_30_P, 100},	// Vic 100
	{XVIDC_VM_4096x2160_50_P, 101},	// Vic 101
	{XVIDC_VM_4096x2160_60_P, 102}	// Vic 102
};

/************************** Function Prototypes ******************************/

static void StubCallback(void *CallbackRef);

/************************** Variable Definitions *****************************/


/************************** Function Definitions *****************************/

/*****************************************************************************/
/**
*
* This function initializes the HDMI RX core. This function must be called
* prior to using the HDMI RX core. Initialization of the HDMI RX includes
* setting up the instance data, and ensuring the hardware is in a quiescent
* state.
*
* @param	InstancePtr is a pointer to the XHdmiRx core instance.
* @param	CfgPtr points to the configuration structure associated with
*		the HDMI RX core.
* @param	EffectiveAddr is the base address of the device. If address
*		translation is being used, then this parameter must reflect the
*		virtual base address. Otherwise, the physical address should be
*		used.
*
* @return
*		- XST_SUCCESS if XV_HdmiRx_CfgInitialize was successful.
*		- XST_FAILURE if HDMI RX PIO ID mismatched.
*
* @note		None.
*
******************************************************************************/
int XV_HdmiRx_CfgInitialize(XV_HdmiRx *InstancePtr, XV_HdmiRx_Config *CfgPtr, u32 EffectiveAddr)
{
	u32 RegValue;
	u32 Status;

	/* Verify arguments. */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(CfgPtr != NULL);
	Xil_AssertNonvoid(EffectiveAddr != (u32)0x0);

	/* Setup the instance */
	(void)memset((void *)InstancePtr, 0, sizeof(XV_HdmiRx));
	(void)memcpy((void *)&(InstancePtr->Config), (const void *)CfgPtr, sizeof(XV_HdmiRx_Config));
	InstancePtr->Config.BaseAddress = EffectiveAddr;

	/* Check PIO ID */
	RegValue = XV_HdmiRx_ReadReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_PIO_ID_OFFSET));
	RegValue = ((RegValue) >> (XV_HDMIRX_SHIFT_16)) & (XV_HDMIRX_MASK_16);
	if (RegValue != (XV_HDMIRX_PIO_ID)) {
		return (XST_FAILURE);
	}

	/*
		Callbacks
		These are placeholders pointing to the StubCallback
		The actual callback pointers will be assigned by the SetCallback function
	*/

	InstancePtr->ConnectCallback = (XV_HdmiRx_Callback)((void *)StubCallback);
	InstancePtr->IsConnectCallbackSet = (FALSE);

	InstancePtr->AuxCallback = (XV_HdmiRx_Callback)((void *)StubCallback);
	InstancePtr->IsAuxCallbackSet = (FALSE);

	InstancePtr->AudCallback = (XV_HdmiRx_Callback)((void *)StubCallback);
	InstancePtr->IsAudCallbackSet = (FALSE);

	InstancePtr->LnkStaCallback = (XV_HdmiRx_Callback)((void *)StubCallback);
	InstancePtr->IsLnkStaCallbackSet = (FALSE);

	InstancePtr->DdcCallback = (XV_HdmiRx_Callback)((void *)StubCallback);
	InstancePtr->IsDdcCallbackSet = (FALSE);

	InstancePtr->StreamDownCallback = (XV_HdmiRx_Callback)((void *)StubCallback);
	InstancePtr->IsStreamDownCallbackSet = (FALSE);

	InstancePtr->StreamInitCallback = (XV_HdmiRx_Callback)((void *)StubCallback);
	InstancePtr->IsStreamInitCallbackSet = (FALSE);

	InstancePtr->StreamUpCallback = (XV_HdmiRx_Callback)((void *)StubCallback);
	InstancePtr->IsStreamUpCallbackSet = (FALSE);

	InstancePtr->HdcpCallback = (XV_HdmiRx_Callback)((void *)StubCallback);
	InstancePtr->IsHdcpCallbackSet = (FALSE);

	/* Clear HDMI variables */
	XV_HdmiRx_Clear(InstancePtr);

	// Clear connected flag
	InstancePtr->Stream.IsConnected = (FALSE);

	// Reset all peripherals
	XV_HdmiRx_PioDisable(InstancePtr);
	XV_HdmiRx_TmrDisable(InstancePtr);
	XV_HdmiRx_VtdDisable(InstancePtr);
	XV_HdmiRx_DdcDisable(InstancePtr);
	XV_HdmiRx_AuxDisable(InstancePtr);
	XV_HdmiRx_AudioDisable(InstancePtr);
	XV_HdmiRx_LnkstaDisable(InstancePtr);

	XV_HdmiRx_PioIntrDisable(InstancePtr);
	XV_HdmiRx_TmrIntrDisable(InstancePtr);
	XV_HdmiRx_VtdIntrDisable(InstancePtr);
	XV_HdmiRx_DdcScdcClear(InstancePtr);

	/*
		PIO peripheral
	*/

	/* PIO: Set event rising edge masks */
	XV_HdmiRx_WriteReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_PIO_IN_EVT_RE_OFFSET),
			(XV_HDMIRX_PIO_IN_DET_MASK) |
			(XV_HDMIRX_PIO_IN_LNK_RDY_MASK) |
			(XV_HDMIRX_PIO_IN_VID_RDY_MASK) |
			(XV_HDMIRX_PIO_IN_SCDC_SCRAMBLER_ENABLE_MASK) |
			(XV_HDMIRX_PIO_IN_SCDC_TMDS_CLOCK_RATIO_MASK)
		);

	/* PIO: Set event falling edge masks */
	XV_HdmiRx_WriteReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_PIO_IN_EVT_FE_OFFSET),
			(XV_HDMIRX_PIO_IN_DET_MASK) |
			(XV_HDMIRX_PIO_IN_VID_RDY_MASK) |
			(XV_HDMIRX_PIO_IN_SCDC_SCRAMBLER_ENABLE_MASK) |
			(XV_HDMIRX_PIO_IN_SCDC_TMDS_CLOCK_RATIO_MASK)
		);

	/* Set run flag */
	XV_HdmiRx_PioEnable(InstancePtr);

	/* Enable interrupt */
	XV_HdmiRx_PioIntrEnable(InstancePtr);

	/*
		Timer
	*/

	/* Set run flag */
	XV_HdmiRx_TmrEnable(InstancePtr);

	/* Enable interrupt */
	XV_HdmiRx_TmrIntrEnable(InstancePtr);

	/*
		Video Timing detector peripheral
	*/

	/* Set run flag */
	XV_HdmiRx_VtdEnable(InstancePtr);

	/* Enable interrupt */
	XV_HdmiRx_VtdIntrEnable(InstancePtr);

	/*
		DDC peripheral
	*/

	/* Enable DDC */
	XV_HdmiRx_DdcEnable(InstancePtr);

	/* Enable DDC peripheral interrupt */
	//XV_HdmiRx_DdcIntrEnable(InstancePtr);

	// Enable SCDC
	XV_HdmiRx_DdcScdcEnable(InstancePtr);

	/*
		AUX peripheral
	*/

	// The aux peripheral will be enabled in the RX init done callback
	//XV_HdmiRx_AuxEnable(InstancePtr);

	/* Enable AUX peripheral interrupt */
	XV_HdmiRx_AuxIntrEnable(InstancePtr);

	/*
		Audio peripheral
	*/

	// The audio peripheral willl be enabled in the RX init done callback
	//XV_HdmiRx_AudioEnable(InstancePtr);

	/* Enable AUD peripheral interrupt */
	XV_HdmiRx_AudioIntrEnable(InstancePtr);

	/* Enable Link Status */
	XV_HdmiRx_LnkstaEnable(InstancePtr);

	/* Enable Link Status peripheral interrupt */
	//XV_HdmiRx_LinkIntrEnable(InstancePtr);

	/* Reset the hardware and set the flag to indicate the driver is ready */
	InstancePtr->IsReady = (u32)(XIL_COMPONENT_IS_READY);

	return (XST_SUCCESS);
}

/*****************************************************************************/
/**
*
* This function clears the HDMI RX variables and sets them to the defaults.
*
* @param	InstancePtr is a pointer to the XV_HdmiRx core instance.
*
* @return	None.
*
* @note		This is required after a reset or init.
*
******************************************************************************/
void XV_HdmiRx_Clear(XV_HdmiRx *InstancePtr)
{
	u32 Index;

	/* Verify argument. */
	Xil_AssertVoid(InstancePtr != NULL);

	InstancePtr->Stream.State = XV_HDMIRX_STATE_STREAM_DOWN;			// The stream is down
	InstancePtr->Stream.IsHdmi = (FALSE);
	InstancePtr->Stream.Video.ColorFormatId = (XVIDC_CSF_RGB);			// Default RGB
	InstancePtr->Stream.Video.IsInterlaced = 0;
	InstancePtr->Stream.Video.ColorDepth = (XVIDC_BPC_8);				// Default 8 bits
	InstancePtr->Stream.Video.PixPerClk = (XVIDC_PPC_2);
	InstancePtr->Stream.Video.VmId = (XVIDC_VM_NO_INPUT);
	InstancePtr->Stream.Video.Timing.HActive = 0;
	InstancePtr->Stream.Video.Timing.HFrontPorch = 0;
	InstancePtr->Stream.Video.Timing.HSyncWidth = 0;
	InstancePtr->Stream.Video.Timing.HBackPorch = 0;
	InstancePtr->Stream.Video.Timing.HTotal = 0;
	InstancePtr->Stream.Video.Timing.HSyncPolarity = 0;
	InstancePtr->Stream.Video.Timing.VActive = 0;
	InstancePtr->Stream.Video.Timing.F0PVFrontPorch = 0;
	InstancePtr->Stream.Video.Timing.F0PVSyncWidth = 0;
	InstancePtr->Stream.Video.Timing.F0PVBackPorch = 0;
	InstancePtr->Stream.Video.Timing.F0PVTotal = 0;
	InstancePtr->Stream.Video.Timing.F1VFrontPorch = 0;
	InstancePtr->Stream.Video.Timing.F1VSyncWidth = 0;
	InstancePtr->Stream.Video.Timing.F1VBackPorch = 0;
	InstancePtr->Stream.Video.Timing.F1VTotal = 0;
	InstancePtr->Stream.Video.Timing.VSyncPolarity = 0;
	InstancePtr->Stream.Vic = 0;
	InstancePtr->Stream.Audio.Active = (FALSE);								// Idle stream
	InstancePtr->Stream.Audio.Channels = 2;								// 2 channels

	/* AUX */
	InstancePtr->Aux.Header.Data = 0;
	for (Index = 0; Index < 8; Index++) {
		InstancePtr->Aux.Data.Data[Index] = 0;
	}

	/* Audio */
	InstancePtr->AudCts = 0;
	InstancePtr->AudN = 0;
	InstancePtr->AudFormat = 0;

	// Call stream down callback
	if (InstancePtr->IsStreamDownCallbackSet) {
		InstancePtr->StreamDownCallback(InstancePtr->StreamDownRef);
	}
}

/*****************************************************************************/
/**
*
* This function sets the HDMI RX stream parameters.
*
* @param	InstancePtr is a pointer to the XV_HdmiRx core instance.
* @param	Ppc specifies the pixel per clock.
*		- 1 = XVIDC_PPC_1
*		- 2 = XVIDC_PPC_2
*		- 4 = XVIDC_PPC_4
* @param	Clock specifies reference pixel clock frequency.
*
* @return
*		- XST_SUCCESS is always returned.
*
* @note		None.
*
******************************************************************************/
int XV_HdmiRx_SetStream(XV_HdmiRx *InstancePtr, XVidC_PixelsPerClock Ppc, u32 Clock)
{

	/* Verify arguments. */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid((Ppc == (XVIDC_PPC_2)) || (Ppc == (XVIDC_PPC_4)));
	Xil_AssertNonvoid(Clock > 0x0);

	/* Pixels per clock */
	InstancePtr->Stream.Video.PixPerClk = Ppc;
	InstancePtr->Stream.PixelClk = Clock;

	/* Set RX pixel rate */
	XV_HdmiRx_SetPixelRate(InstancePtr);

	return (XST_SUCCESS);
}

/*****************************************************************************/
/**
*
* This function sets the pixel rate.
*
* @param	InstancePtr is a pointer to the XV_HdmiRx core instance.
*
* @return
*		- XST_SUCCESS is always returned.
*
* @note		None.
*
******************************************************************************/
int XV_HdmiRx_SetPixelRate(XV_HdmiRx *InstancePtr)
{
	u32 RegValue;
	u8 PixelRate;

	/* Verify argument. */
	Xil_AssertNonvoid(InstancePtr != NULL);

	/* Mask pixel rate */
	XV_HdmiRx_WriteReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_PIO_OUT_MSK_OFFSET), (XV_HDMIRX_PIO_OUT_PIXEL_RATE_MASK));

	/* Check pixel per clock */
	switch (InstancePtr->Stream.Video.PixPerClk) {
		case (XVIDC_PPC_2):
			PixelRate = 1;
			break;

		case (XVIDC_PPC_4):
			PixelRate = 2;
			break;

		default:
			PixelRate = 0;
			break;
	}

	/* Set pixel rate for video path */
	RegValue = PixelRate << (XV_HDMIRX_PIO_OUT_PIXEL_RATE_SHIFT);
	XV_HdmiRx_WriteReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_PIO_OUT_OFFSET), RegValue);

	return (XST_SUCCESS);
}

/*****************************************************************************/
/**
*
* This function sets the color format
*
* @param	InstancePtr is a pointer to the XV_HdmiRx core instance.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void XV_HdmiRx_SetColorFormat(XV_HdmiRx *InstancePtr)
{
	u32 RegValue;

	/* Verify argument. */
	Xil_AssertVoid(InstancePtr != NULL);

	/* Mask PIO Out Mask register */
	XV_HdmiRx_WriteReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_PIO_OUT_MSK_OFFSET), (XV_HDMIRX_PIO_OUT_COLOR_SPACE_MASK));

	/* Check for color format */
	switch (InstancePtr->Stream.Video.ColorFormatId) {
		case (XVIDC_CSF_YCRCB_444):
			RegValue = 1;
			break;

		case (XVIDC_CSF_YCRCB_422):
			RegValue = 2;
			break;

		case (XVIDC_CSF_YCRCB_420):
			RegValue = 3;
			break;

		default:
			RegValue = 0;
			break;
	}

	/* Write color space into PIO Out register */
	XV_HdmiRx_WriteReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_PIO_OUT_OFFSET), (RegValue << (XV_HDMIRX_PIO_OUT_COLOR_SPACE_SHIFT)));
}

/*****************************************************************************/
/**
*
* This function enables/clear Hot-Plug-Detect.
*
* @param	InstancePtr is a pointer to the XV_HdmiRx core instance.
* @param	SetClr specifies TRUE/FALSE value to either enable or
*		clear HPD respectively.
*
* @return
*		- XST_SUCCESS is always returned.
*
* @note		None.
*
******************************************************************************/
int XV_HdmiRx_SetHpd(XV_HdmiRx *InstancePtr, u8 SetClr)
{
	/* Verify arguments. */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid((SetClr == (TRUE)) || (SetClr == (FALSE)));

	if (SetClr) {
		/* Set HPD */
		XV_HdmiRx_WriteReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_PIO_OUT_SET_OFFSET), (XV_HDMIRX_PIO_OUT_HPD_MASK));
	}
	else {
		/* Clear HPD */
		XV_HdmiRx_WriteReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_PIO_OUT_CLR_OFFSET), (XV_HDMIRX_PIO_OUT_HPD_MASK));
	}

	return (XST_SUCCESS);
}

/*****************************************************************************/
/**
*
* This function provides status of the HDMI RX core Link Status peripheral.
*
* @param	InstancePtr is a pointer to the XV_HdmiRx core instance.
* @param	Type specifies one of the type for which status to be provided:
*		- 0 = Link error counter for channel 0.
*		- 1 = Link error counter for channel 1.
*		- 2 = Link error counter for channel 2.
*		- 3 = Link phase.
*		- 4 = Link delay.
*		- 5 = Link line length
*
* @return	Link status of the HDMI RX core link.
*
* @note		None.
*
******************************************************************************/
u32 XV_HdmiRx_GetLinkStatus(XV_HdmiRx *InstancePtr, u8 Type)
{
	u32 RegValue;

	/* Verify arguments. */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(Type < 0x6);

	RegValue = XV_HdmiRx_ReadReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_LNKSTA_LNK_ERR0_OFFSET) + (4 * Type));

	return RegValue;
}

/*****************************************************************************/
/**
*
* This function provides status of one of the link error counters reached the
* maximum value.
*
* @param	InstancePtr is a pointer to the XV_HdmiRx core instance.
*
* @return
*		- TRUE = Maximum error counter reached.
*		- FALSE = Maximum error counter not reached.
*
* @note		None.
*
******************************************************************************/
int XV_HdmiRx_IsLinkStatusErrMax(XV_HdmiRx *InstancePtr)
{
	u32 Status;

	/* Verify argument. */
	Xil_AssertNonvoid(InstancePtr != NULL);

	/* Read Link Status peripheral Status register */
	Status = XV_HdmiRx_ReadReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_LNKSTA_STA_OFFSET)) & (XV_HDMIRX_LNKSTA_STA_ERR_MAX_MASK);

	if (Status) {
		Status = (TRUE);
	}
	else {
		Status = (FALSE);
	}

	return Status;
}

/*****************************************************************************/
/**
*
* This function clears the link error counters.
*
* @param	InstancePtr is a pointer to the XV_HdmiRx core instance.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void XV_HdmiRx_ClearLinkStatus(XV_HdmiRx *InstancePtr)
{
	/* Verify argument. */
	Xil_AssertVoid(InstancePtr != NULL);

	/* Set Error Clear bit */
	XV_HdmiRx_WriteReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_LNKSTA_CTRL_SET_OFFSET), (XV_HDMIRX_LNKSTA_CTRL_ERR_CLR_MASK));

	/* Clear Error Clear bit */
	XV_HdmiRx_WriteReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_LNKSTA_CTRL_CLR_OFFSET), (XV_HDMIRX_LNKSTA_CTRL_ERR_CLR_MASK));
}

/*****************************************************************************/
/**
*
* This function provides audio clock regenerating CTS (Cycle-Time Stamp) value
* at the HDMI sink device.
*
* @param	InstancePtr is a pointer to the XV_HdmiRx core instance.
*
* @return	Audio clock CTS value.
*
* @note		None.
*
******************************************************************************/
u32 XV_HdmiRx_GetAcrCts(XV_HdmiRx *InstancePtr)
{
	u32 CtsValue;

	/* Verify argument. */
	Xil_AssertNonvoid(InstancePtr != NULL);

	/* Read cycle time stamp value */
	CtsValue = XV_HdmiRx_ReadReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_AUD_CTS_OFFSET));

	return CtsValue;
}

/*****************************************************************************/
/**
*
* This function provides audio clock regenerating factor N value.
*
* @param	InstancePtr is a pointer to the XV_HdmiRx core instance.
*
* @return	ACR N value.
*
* @note		None.
*
******************************************************************************/
u32 XV_HdmiRx_GetAcrN(XV_HdmiRx *InstancePtr)
{
	u32 AcrNValue;

	/* Verify argument. */
	Xil_AssertNonvoid(InstancePtr != NULL);

	/* Read ACR factor N value */
	AcrNValue = XV_HdmiRx_ReadReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_AUD_N_OFFSET));

	return AcrNValue;
}

/*****************************************************************************/
/**
*
* This function gets the size of the EDID buffer of the DDC slave.
*
* @param	InstancePtr is a pointer to the XV_HdmiRx core instance.
*
* @return
*		- EDID buffer size
*
* @note		None.
*
******************************************************************************/
u16 XV_HdmiRx_DdcGetEdidWords(XV_HdmiRx *InstancePtr)
{
	u32 Data;

	// Verify argument.
	Xil_AssertNonvoid(InstancePtr != NULL);

	// Read status register
	Data = XV_HdmiRx_ReadReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_DDC_STA_OFFSET));
	Data >>= XV_HDMIRX_DDC_STA_EDID_WORDS_SHIFT;
	return (Data);
}

/*****************************************************************************/
/**
*
* This function loads the EDID data into the DDC slave.
*
* @param	InstancePtr is a pointer to the XV_HdmiRx core instance.
* @param	EdidData is a pointer to the EDID data array.
* @param	Length is the length, in bytes, of the EDID array.
*
* @return
*		- XST_SUCCESS if the EDID data was loaded successfully
*		- XST_FAILURE if the EDID data load failed
*
* @note		None.
*
******************************************************************************/
int XV_HdmiRx_DdcLoadEdid(XV_HdmiRx *InstancePtr, u8 *EdidData, u16 Length)
{
	u8 Data;
	u16 Index;

	// Verify argument.
	Xil_AssertNonvoid(InstancePtr != NULL);

	// Check if the EDID data fits in the DDC slave EDID buffer
	if (XV_HdmiRx_DdcGetEdidWords(InstancePtr) >= Length)
	{
		// Clear EDID write pointer
		XV_HdmiRx_WriteReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_DDC_EDID_WP_OFFSET), 0);

		// Copy EDID data
		for (Index = 0; Index < Length; Index++) {
			Data = *(EdidData + Index);
			XV_HdmiRx_WriteReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_DDC_EDID_DATA_OFFSET), (Data));
		}

		// Enable EDID
		XV_HdmiRx_WriteReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_DDC_CTRL_SET_OFFSET), (XV_HDMIRX_DDC_CTRL_EDID_EN_MASK));

		return (XST_SUCCESS);
	}

	// The EDID data is larger than the DDC slave EDID buffer size
	else
	{
		xil_printf("The EDID data structure is too large to be stored in the DDC peripheral (%0d).\n\r", Length);
		return (XST_FAILURE);
	}
}

/******************************************************************************/
/**
*
* This function prints stream and timing information on STDIO/Uart console.
*
* @param	InstancePtr is a pointer to the XV_HdmiRx core instance.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void XV_HdmiRx_DebugInfo(XV_HdmiRx *InstancePtr)
{
	/* Verify argument. */
	Xil_AssertVoid(InstancePtr != NULL);

	/* Print stream information */
	XVidC_ReportStreamInfo(&InstancePtr->Stream.Video);

	/* Print timing information */
	XVidC_ReportTiming(&InstancePtr->Stream.Video.Timing, InstancePtr->Stream.Video.IsInterlaced);
}

/*****************************************************************************/
/**
*
* This function provides status of the stream
*
* @param	InstancePtr is a pointer to the XV_HdmiRx core instance.
*
* @return
*		- TRUE = Stream is up.
*		- FALSE = Stream is down.
*
* @note		None.
*
******************************************************************************/
int XV_HdmiRx_IsStreamUp(XV_HdmiRx *InstancePtr)
{

	/* Verify argument. */
	Xil_AssertNonvoid(InstancePtr != NULL);

	if (InstancePtr->Stream.State == XV_HDMIRX_STATE_STREAM_UP) {
		return (TRUE);
	}
	else {
		return (FALSE);
	}
}

/*****************************************************************************/
/**
*
* This function provides the stream scrambler status
*
* @param	InstancePtr is a pointer to the XV_HdmiRx core instance.
*
* @return
*		- TRUE = Stream is scrambled.
*		- FALSE = Stream is not scrambled.
*
* @note		None.
*
******************************************************************************/
int XV_HdmiRx_IsStreamScrambled(XV_HdmiRx *InstancePtr)
{

	/* Verify argument. */
	Xil_AssertNonvoid(InstancePtr != NULL);

	return (InstancePtr->Stream.IsScrambled);
}

/*****************************************************************************/
/**
*
* This function provides the stream connected status
*
* @param	InstancePtr is a pointer to the XV_HdmiRx core instance.
*
* @return
*		- TRUE = Stream is connected.
*		- FALSE = Stream is connected.
*
* @note		None.
*
******************************************************************************/
int XV_HdmiRx_IsStreamConnected(XV_HdmiRx *InstancePtr)
{

	/* Verify argument. */
	Xil_AssertNonvoid(InstancePtr != NULL);

	return (InstancePtr->Stream.IsConnected);
}

/*****************************************************************************/
/**
*
* This function gets the SCDC TMDS clock ratio bit
*
* @param	InstancePtr is a pointer to the XV_HdmiRx core instance.
*
* @return
*		- TRUE = TMDS clock ratio bit is set.
*		- FALSE = TMDS clock ratio bit is cleared.
*
* @note		None.
*
******************************************************************************/
int XV_HdmiRx_GetTmdsClockRatio(XV_HdmiRx *InstancePtr)
{
	u32 Data;

	/* Verify argument. */
	Xil_AssertNonvoid(InstancePtr != NULL);

	Data = XV_HdmiRx_ReadReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_PIO_IN_OFFSET));

	if ((Data) & (XV_HDMIRX_PIO_IN_SCDC_TMDS_CLOCK_RATIO_MASK))
		return (TRUE);
	else
		return (FALSE);
}

/*****************************************************************************/
/**
*
* This function returns the AVI VIC (captured by the AUX peripheral)
*
* @param	InstancePtr is a pointer to the XV_HdmiRx core instance.
*
* @return 	The AVI VIC code.
*
* @note		None.
*
******************************************************************************/
u8 XV_HdmiRx_GetAviVic(XV_HdmiRx *InstancePtr)
{
	u32 Data;

	// Verify argument.
	Xil_AssertNonvoid(InstancePtr != NULL);

	// Read status register
	Data = XV_HdmiRx_ReadReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_AUX_STA_OFFSET));
	Data >>= XV_HDMIRX_AUX_STA_AVI_VIC_SHIFT;
	Data &= XV_HDMIRX_AUX_STA_AVI_VIC_MASK;
	return (u8)(Data);
}

/*****************************************************************************/
/**
*
* This function returns the AVI colorspace (captured by the AUX peripheral)
*
* @param	InstancePtr is a pointer to the XV_HdmiRx core instance.
*
* @return	The AVI colorspace value.
*
* @note		None.
*
******************************************************************************/
XVidC_ColorFormat XV_HdmiRx_GetAviColorSpace(XV_HdmiRx *InstancePtr)
{
	u32 Data;
	XVidC_ColorFormat ColorSpace;

	// Verify argument.
	Xil_AssertNonvoid(InstancePtr != NULL);

	// Read status register
	Data = XV_HdmiRx_ReadReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_AUX_STA_OFFSET));
	Data >>= XV_HDMIRX_AUX_STA_AVI_CS_SHIFT;
	Data &= XV_HDMIRX_AUX_STA_AVI_CS_MASK;

		switch (Data) {
			case 1:
				ColorSpace = (XVIDC_CSF_YCRCB_422);
				break;

			case 2:
				ColorSpace = (XVIDC_CSF_YCRCB_444);
				break;

			case 3:
				ColorSpace = (XVIDC_CSF_YCRCB_420);
				break;

			default:
				ColorSpace = (XVIDC_CSF_RGB);
				break;
		}
	return (ColorSpace);
}

/*****************************************************************************/
/**
*
* This function returns the GCP color depth (captured by the AUX peripheral)
*
* @param	InstancePtr is a pointer to the XV_HdmiRx core instance.
*
* @return	The GCP color depth.
*
* @note		None.
*
******************************************************************************/
XVidC_ColorDepth XV_HdmiRx_GetGcpColorDepth(XV_HdmiRx *InstancePtr)
{
	u32 Data;
	XVidC_ColorDepth ColorDepth;

	// Verify argument.
	Xil_AssertNonvoid(InstancePtr != NULL);

	// Read status register
	Data = XV_HdmiRx_ReadReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_AUX_STA_OFFSET));
	Data >>= XV_HDMIRX_AUX_STA_GCP_CD_SHIFT;
	Data &= XV_HDMIRX_AUX_STA_GCP_CD_MASK;

		switch (Data) {
			case 1:
				ColorDepth = (XVIDC_BPC_10);
				break;

			case 2:
				ColorDepth = (XVIDC_BPC_12);
				break;

			case 3:
				ColorDepth = (XVIDC_BPC_16);
				break;

			default:
				ColorDepth = (XVIDC_BPC_8);
				break;
		}
	return (ColorDepth);
}

/*****************************************************************************/
/**
*
* This function calculates the divider for the frame calculation
*
* @param	Dividend is the dividend value to use in the calculation.
* @param   	Divisor is the divisor value to use in the calculation.
*
* @return	The result of the calculation.
*
* @note		None.
*
******************************************************************************/
u32 XV_HdmiRx_Divide(u32 Dividend, u32 Divisor)
{
	u32 Result;
	u32 Remainder;
	Result = Dividend / Divisor;
	Remainder = Dividend % Divisor;
	if (Remainder) {
		if (Remainder > (Divisor/2))
			Result += 1;
	}
	return (Result);
}

/*****************************************************************************/
/**
*
* This function searches for the video mode based on the vic.
*
* @param	Vic
*
* @return	Vic defined in the VIC table.
*
* @note		None.
*
******************************************************************************/
XVidC_VideoMode XV_HdmiRx_LookupVmId(u8 Vic)
{
    XV_HdmiRx_VicTable const *Entry;
    u8 Index;

    for (Index = 0; Index < sizeof(VicTable)/sizeof(XV_HdmiRx_VicTable); Index++) {
      Entry = &VicTable[Index];
      if (Entry->Vic == Vic)
        return (Entry->VmId);
    }
    return XVIDC_VM_NOT_SUPPORTED;
}

/*****************************************************************************/
/**
*
* This function reads the video properties from the aux peripheral
*
* @param	InstancePtr is a pointer to the XV_HdmiRx core instance.
*
* @return
*
* @note		None.
*
******************************************************************************/
void XV_HdmiRx_GetVideoProperties(XV_HdmiRx *InstancePtr)
{
	// Get AVI colorspace
	InstancePtr->Stream.Video.ColorFormatId = XV_HdmiRx_GetAviColorSpace(InstancePtr);

	// Get AVI Vic
	InstancePtr->Stream.Vic = XV_HdmiRx_GetAviVic(InstancePtr);

	// Get GCP colordepth
	// In HDMI the colordepth in YUV422 is always 12 bits (although on the link itself it is being transmitted as 8-bits.
	// Therefore if the colorspace is YUV422, then force the colordepth to 12 bits.
	if (InstancePtr->Stream.Video.ColorFormatId == XVIDC_CSF_YCRCB_422) {
		InstancePtr->Stream.Video.ColorDepth = XVIDC_BPC_12;
	}

	// Else read the colordepth from the general control packet
	else {
		InstancePtr->Stream.Video.ColorDepth = XV_HdmiRx_GetGcpColorDepth(InstancePtr);
	}
}

/*****************************************************************************/
/**
*
* This function reads the video timing from the VTD peripheral
*
* @param	InstancePtr is a pointer to the XV_HdmiRx core instance.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void XV_HdmiRx_GetVideoTiming(XV_HdmiRx *InstancePtr)
{
	u32 Data;

	// Lookup the videomode based on the vic
	InstancePtr->Stream.Video.VmId = XV_HdmiRx_LookupVmId(InstancePtr->Stream.Vic);
	//InstancePtr->Stream.Video.VmId = XVIDC_VM_NOT_SUPPORTED;

	// Was the vic found?
	// Yes, then get the timing parameters from the video library
	if (InstancePtr->Stream.Video.VmId != (XVIDC_VM_NOT_SUPPORTED)) {

		// Timing
		InstancePtr->Stream.Video.Timing = *XVidC_GetTimingInfo(InstancePtr->Stream.Video.VmId);

		// Framerate
		InstancePtr->Stream.Video.FrameRate = XVidC_GetFrameRate(InstancePtr->Stream.Video.VmId);

		// Interlaced
		InstancePtr->Stream.Video.IsInterlaced = XVidC_IsInterlaced(InstancePtr->Stream.Video.VmId);
	}

	// No, then read the timing parameters from the video timing detector
	else {
		/* Read Total Pixels */
		InstancePtr->Stream.Video.Timing.HTotal =  XV_HdmiRx_ReadReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_VTD_TOT_PIX_OFFSET));

		// For YUV420 the total pixels must be doubled
		if (InstancePtr->Stream.Video.ColorFormatId == XVIDC_CSF_YCRCB_420)
			InstancePtr->Stream.Video.Timing.HTotal = InstancePtr->Stream.Video.Timing.HTotal * 2;

		/* Read Active Pixels */
		InstancePtr->Stream.Video.Timing.HActive =  XV_HdmiRx_ReadReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_VTD_ACT_PIX_OFFSET));

		// For YUV420 the active pixels must be doubled
		if (InstancePtr->Stream.Video.ColorFormatId == XVIDC_CSF_YCRCB_420)
			InstancePtr->Stream.Video.Timing.HActive = InstancePtr->Stream.Video.Timing.HActive * 2;

		/* Read Hsync Width */
		InstancePtr->Stream.Video.Timing.HSyncWidth =  XV_HdmiRx_ReadReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_VTD_HSW_OFFSET));

		// For YUV420 the Hsync width must be doubled
		if (InstancePtr->Stream.Video.ColorFormatId == XVIDC_CSF_YCRCB_420)
			InstancePtr->Stream.Video.Timing.HSyncWidth = InstancePtr->Stream.Video.Timing.HSyncWidth * 2;

		/* Read HFront Porch */
		InstancePtr->Stream.Video.Timing.HFrontPorch =  XV_HdmiRx_ReadReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_VTD_HFP_OFFSET));

		// For YUV420 the Hfront porch must be doubled
		if (InstancePtr->Stream.Video.ColorFormatId == XVIDC_CSF_YCRCB_420)
			InstancePtr->Stream.Video.Timing.HFrontPorch = InstancePtr->Stream.Video.Timing.HFrontPorch * 2;

		/* Read HBack Porch */
		InstancePtr->Stream.Video.Timing.HBackPorch =  XV_HdmiRx_ReadReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_VTD_HBP_OFFSET));

		// For YUV420 the Hfront porch must be doubled
		if (InstancePtr->Stream.Video.ColorFormatId == XVIDC_CSF_YCRCB_420)
			InstancePtr->Stream.Video.Timing.HBackPorch = InstancePtr->Stream.Video.Timing.HBackPorch * 2;

		/* Total lines field 1 */
		InstancePtr->Stream.Video.Timing.F0PVTotal =  XV_HdmiRx_ReadReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_VTD_TOT_LIN_OFFSET)) & (0xFFFF);

		/* Total lines field 2 */
		InstancePtr->Stream.Video.Timing.F1VTotal =  ((XV_HdmiRx_ReadReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_VTD_TOT_LIN_OFFSET))) >> 16);

		/* Active lines field 1 */
		InstancePtr->Stream.Video.Timing.VActive =  XV_HdmiRx_ReadReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_VTD_ACT_LIN_OFFSET)) & (0xFFFF);

		/* Read VSync Width field 1*/
		InstancePtr->Stream.Video.Timing.F0PVSyncWidth =  XV_HdmiRx_ReadReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_VTD_VSW_OFFSET)) & (0xFFFF);

		/* Read VSync Width field 2*/
		InstancePtr->Stream.Video.Timing.F1VSyncWidth =  ((XV_HdmiRx_ReadReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_VTD_VSW_OFFSET))) >> 16);

		/* Read VFront Porch field 1*/
		InstancePtr->Stream.Video.Timing.F0PVFrontPorch =  XV_HdmiRx_ReadReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_VTD_VFP_OFFSET)) & (0xFFFF);

		/* Read VFront Porch field 2*/
		InstancePtr->Stream.Video.Timing.F1VFrontPorch =  ((XV_HdmiRx_ReadReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_VTD_VFP_OFFSET))) >> 16);
		Data = (Data >> 16) & (0xFFFF);

		/* Read VBack Porch field 1 */
		InstancePtr->Stream.Video.Timing.F0PVBackPorch =  XV_HdmiRx_ReadReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_VTD_VBP_OFFSET)) & (0xFFFF);

		/* Read VBack Porch field 2 */
		InstancePtr->Stream.Video.Timing.F1VBackPorch =  ((XV_HdmiRx_ReadReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_VTD_VBP_OFFSET))) >> 16);

		/* Read Status register */
		Data = XV_HdmiRx_ReadReg(InstancePtr->Config.BaseAddress, (XV_HDMIRX_VTD_STA_OFFSET));

		/* Check video format */
		if ((Data) & (XV_HDMIRX_VTD_STA_FMT_MASK)) {
			/* Interlaced */
			InstancePtr->Stream.Video.IsInterlaced = 1;
		}
		else {
			/* Progressive */
			InstancePtr->Stream.Video.IsInterlaced = 0;
		}

		/* Check Vsync polarity */
		if ((Data) & (XV_HDMIRX_VTD_STA_VS_POL_MASK)) {
			/* Positive */
			InstancePtr->Stream.Video.Timing.VSyncPolarity = 1;
		}
		else {
			/* Negative */
			InstancePtr->Stream.Video.Timing.VSyncPolarity = 0;
		}

		/* Check Hsync polarity */
		if ((Data) & (XV_HDMIRX_VTD_STA_HS_POL_MASK)) {
			/* Positive */
			InstancePtr->Stream.Video.Timing.HSyncPolarity = 1;
		}
		else {
			/* Negative */
			InstancePtr->Stream.Video.Timing.HSyncPolarity = 0;
		}

		// Calculate and set the frame rate field
		InstancePtr->Stream.Video.FrameRate = XV_HdmiRx_Divide(InstancePtr->Stream.PixelClk, (InstancePtr->Stream.Video.Timing.F0PVTotal * InstancePtr->Stream.Video.Timing.HTotal));

		// If the colorspace is YUV420, then double the frame rate
		if (InstancePtr->Stream.Video.ColorFormatId == XVIDC_CSF_YCRCB_420) {
			xil_printf("Ik kom hier %d\n\r", InstancePtr->Stream.Video.FrameRate);
			InstancePtr->Stream.Video.FrameRate = InstancePtr->Stream.Video.FrameRate * 2;
		}

		// Lookup the video mode id
		InstancePtr->Stream.Video.VmId = XVidC_GetVideoModeId(InstancePtr->Stream.Video.Timing.HActive,
	                                                        InstancePtr->Stream.Video.Timing.VActive,
	                                                        InstancePtr->Stream.Video.FrameRate,
	                                                        InstancePtr->Stream.Video.IsInterlaced);
	}
}

/*****************************************************************************/
/**
*
* This function is a stub for the asynchronous callback. The stub is here in
* case the upper layer forgot to set the handlers. On initialization, all
* handlers are set to this callback. It is considered an error for this
* handler to be invoked.
*
* @param	CallbackRef is a callback reference passed in by the upper
*		layer when setting the callback functions, and passed back to
*		the upper layer when the callback is invoked.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void StubCallback(void *CallbackRef)
{
	Xil_AssertVoid(CallbackRef != NULL);
	Xil_AssertVoidAlways();
}