// HIDtoUARTCP2110.cpp
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////

#include "SLABCP2110.h"
#include "HIDtoUART.h"
#include "UsbReports.h"
#include "DeviceList.h"
#include <string.h>

/////////////////////////////////////////////////////////////////////////////
// Global Variables
/////////////////////////////////////////////////////////////////////////////

extern CDeviceList<CHIDtoUART> DeviceList;

/////////////////////////////////////////////////////////////////////////////
// Feature Report Prototypes
/////////////////////////////////////////////////////////////////////////////

static HID_UART_STATUS SetPinConfig(CHIDtoUART* device, BYTE* pinConfig, BOOL useSuspendValues, WORD suspendValue, WORD suspendMode, BYTE rs485Level, BYTE clkDiv);
static HID_UART_STATUS GetPinConfig(CHIDtoUART* device, BYTE* pinConfig, BOOL* useSuspendValues, WORD* suspendValue, WORD* suspendMode, BYTE* rs485Level, BYTE* clkDiv);

/////////////////////////////////////////////////////////////////////////////
// Feature Report Functions
/////////////////////////////////////////////////////////////////////////////

// Set feature report for HidUart_SetPinConfig
HID_UART_STATUS SetPinConfig(CHIDtoUART* device, BYTE* pinConfig, BOOL useSuspendValues, WORD suspendValue, WORD suspendMode, BYTE rs485Level, BYTE clkDiv)
{
	HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

	// Use suspend values is stuffed into bit 15 of suspendMode
	if (useSuspendValues)
	{
		// Set latch to suspendValue and mode to suspendMode
		// when the device is suspended
		suspendMode |= USE_SUSPEND_VALUES_MASK;
	}
	else
	{
		// Keep current configuration when
		// the device is suspended
		suspendMode &= ~USE_SUSPEND_VALUES_MASK;
	}

	BYTE report[FEATURE_REPORT_LENGTH];
	memset(report, 0x00, sizeof(report));

	report[0]	= GETSET_PIN_CONFIG;			// Report ID
	memcpy(&report[1], pinConfig, 13);			// GPIO Pin Modes
	report[14]	= HIBYTE(suspendValue);			// Suspend Latch Value (Big-endian)
	report[15]	= LOBYTE(suspendValue);
	report[16]	= HIBYTE(suspendMode);			// Suspend Mode (Big-endian)
	report[17]	= LOBYTE(suspendMode);
	report[18]	= rs485Level;					// RS485 Logic Level (0: Active Low, 1: Active High)
	report[19]	= clkDiv;						// Clock Output Divider (CLK = 24 MHz / (2 * clkDiv))
												// CLK = 24 MHz, when clkDiv = 0

	// Set feature report to the device
	if (HidDevice_SetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
	{
		status = HID_UART_SUCCESS;
	}

	return status;
}

// Get feature report for HidUart_GetPinConfig
HID_UART_STATUS GetPinConfig(CHIDtoUART* device, BYTE* pinConfig, BOOL* useSuspendValues, WORD* suspendValue, WORD* suspendMode, BYTE* rs485Level, BYTE* clkDiv)
{
	HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

	BYTE report[FEATURE_REPORT_LENGTH];
	memset(report, 0x00, sizeof(report));

	// Report ID
	report[0] = GETSET_PIN_CONFIG;

	// Get feature report from the device
	if (HidDevice_GetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
	{
		memcpy(pinConfig, &report[1], 13);
		*suspendValue	= MAKEWORD(report[15], report[14]);
		*suspendMode	= MAKEWORD(report[17], report[16]);
		*rs485Level		= report[18];
		*clkDiv			= report[19];

		if (*suspendMode & USE_SUSPEND_VALUES_MASK)
		{
			*useSuspendValues = TRUE;
		}
		else
		{
			*useSuspendValues = FALSE;
		}

		status = HID_UART_SUCCESS;
	}

	return status;
}

/////////////////////////////////////////////////////////////////////////////
// Exported Library Functions
/////////////////////////////////////////////////////////////////////////////

// HidUart_SetPinConfig
HID_UART_STATUS HidUart_SetPinConfig(HID_UART_DEVICE device, BYTE* pinConfig, BOOL useSuspendValues, WORD suspendValue, WORD suspendMode, BYTE rs485Level, BYTE clkDiv)
{
	HID_UART_STATUS		status		= HID_UART_UNKNOWN_ERROR;
	CHIDtoUART*			hidUart		= ((CHIDtoUART*)device);

	// Check device object
	if (DeviceList.Validate(hidUart))
	{
		// Check pointers
		if (pinConfig)
		{
            // CP2110 Only
            if (hidUart->partNumber == HID_UART_PART_CP2110)
            {
                status = SetPinConfig(hidUart, pinConfig, useSuspendValues, suspendValue, suspendMode, rs485Level, clkDiv);
            }
            else
            {
                status = HID_UART_DEVICE_NOT_SUPPORTED;
            }
		}
		else
		{
			status = HID_UART_INVALID_PARAMETER;
		}
	}
	else
	{
		status = HID_UART_INVALID_DEVICE_OBJECT;
	}

	return status;
}

// HidUart_GetPinConfig
HID_UART_STATUS HidUart_GetPinConfig(HID_UART_DEVICE device, BYTE* pinConfig, BOOL* useSuspendValues, WORD* suspendValue, WORD* suspendMode, BYTE* rs485Level, BYTE* clkDiv)
{
	HID_UART_STATUS		status		= HID_UART_UNKNOWN_ERROR;
	CHIDtoUART*			hidUart		= ((CHIDtoUART*)device);

	// Check device object
	if (DeviceList.Validate(hidUart))
	{
		// Check pointers
		if (pinConfig && useSuspendValues && suspendValue && suspendMode && rs485Level && clkDiv)
		{
            // CP2110 Only
            if (hidUart->partNumber == HID_UART_PART_CP2110)
            {
                status = GetPinConfig(hidUart, pinConfig, useSuspendValues, suspendValue, suspendMode, rs485Level, clkDiv);
            }
            else
            {
                status = HID_UART_DEVICE_NOT_SUPPORTED;
            }
		}
		else
		{
			status = HID_UART_INVALID_PARAMETER;
		}
	}
	else
	{
		status = HID_UART_INVALID_DEVICE_OBJECT;
	}

	return status;
}
