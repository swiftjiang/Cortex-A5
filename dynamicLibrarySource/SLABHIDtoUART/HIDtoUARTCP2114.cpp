// HIDtoUARTCP2114.cpp
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////

#include "SLABCP2114.h"
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

static HID_UART_STATUS GetVersions(CHIDtoUART* device, BYTE* api_version, BYTE* fw_version, BYTE* config_version);
static HID_UART_STATUS SetPinConfig(CHIDtoUART* device, BYTE* pinConfig, BOOL useSuspendValues, WORD suspendValue, WORD suspendMode,BYTE clkDiv);
static HID_UART_STATUS GetPinConfig(CHIDtoUART* device, BYTE* pinConfig, BOOL* useSuspendValues, WORD* suspendValue, WORD* suspendMode,BYTE* clkDiv);

static HID_UART_STATUS Get2114Status(CHIDtoUART* device, BYTE* pCP2114Status);
static HID_UART_STATUS Get2114Caps(CHIDtoUART* device, PCP2114_CAPS_STRUCT pCP2114CapsStruct);
//static HID_UART_STATUS Set2114RamConfig(CHIDtoUART* device, PCP2114_CONFIG_STRUCT pCP2114ConfigStruct);
static HID_UART_STATUS Set2114RamConfig(CHIDtoUART* device, PCP2114_RAM_CONFIG_STRUCT pCP2114RamConfigStruct);
//static HID_UART_STATUS Get2114RamConfig(CHIDtoUART* device, PCP2114_CONFIG_STRUCT pCP2114ConfigStruct);
static HID_UART_STATUS Get2114RamConfig(CHIDtoUART* device, PCP2114_RAM_CONFIG_STRUCT pCP2114RamConfigStruct);
static HID_UART_STATUS SetDacRegisters(CHIDtoUART* device, BYTE* pDacConfigBuffer, BYTE dacConfigBufferLength);
static HID_UART_STATUS GetDacRegisters(CHIDtoUART* device, BYTE dacStartAddress, BYTE dacRegistersToRead, BYTE* pDacConfigBuffer);
static HID_UART_STATUS Get2114OtpConfig(CHIDtoUART* device, BYTE cp2114ConfigNumber, PCP2114_CONFIG_STRUCT pCP2114ConfigStruct);
static HID_UART_STATUS CreateNew2114OtpConfig(CHIDtoUART* device, WORD configBufferLength, BYTE* pConfigBuffer);
static HID_UART_STATUS SetBoot2114(CHIDtoUART* device, BYTE cp2114ConfigNumber);
static HID_UART_STATUS ReadOTP(CHIDtoUART* device, UINT cp2114Address ,BYTE* pReadBuffer, UINT readLength);
static HID_UART_STATUS WriteOTP(CHIDtoUART* device, UINT cp2114Address ,BYTE* pWriteBuffer, UINT writeLength);
static HID_UART_STATUS I2cWriteData(CHIDtoUART* device, BYTE slaveAddress, BYTE* pWriteBuffer, BYTE writeLength);
static HID_UART_STATUS I2cReadData(CHIDtoUART* device, BYTE slaveAddress, BYTE* pWriteBuffer, BYTE writeLength, BYTE* pReadBuffer, BYTE readLength);

/////////////////////////////////////////////////////////////////////////////
// Feature Report Functions
/////////////////////////////////////////////////////////////////////////////

// Get feature report for CP2114_GetVersions()
HID_UART_STATUS GetVersions(CHIDtoUART* device, BYTE* api_version, BYTE* fw_version, BYTE* config_version)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof(report));

    // Report ID
    report[0] = GET_DEVICE_VERSIONS;

    // Get feature report from the device
    if (HidDevice_GetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        // Retrieve version information from the device
        *api_version    = report[1];
        *fw_version     = report[2];
        *config_version = report[3];
        status = HID_UART_SUCCESS;
    }

    return status;
}

// Set feature report for CP2114_SetPinConfig
HID_UART_STATUS SetPinConfig(CHIDtoUART* device, BYTE* pinConfig, BOOL useSuspendValues, WORD suspendValue, WORD suspendMode,BYTE clkDiv)
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
    memcpy(&report[1], pinConfig, 14);			// GPIO Pin Modes
    report[15]	= HIBYTE(suspendValue);			// Suspend Latch Value (Big-endian)
    report[16]	= LOBYTE(suspendValue);
    report[17]	= HIBYTE(suspendMode);			// Suspend Mode (Big-endian)
    report[18]	= LOBYTE(suspendMode);
    report[19]	= clkDiv;						// Clock Output Divider (CLK = 24 MHz / (2 * clkDiv))
                                                // CLK = 24 MHz, when clkDiv = 0

    // Set feature report to the device
    if (HidDevice_SetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        status = HID_UART_SUCCESS;
    }

    return status;
}

// Get feature report for CP2114_GetPinConfig
HID_UART_STATUS GetPinConfig(CHIDtoUART* device, BYTE* pinConfig, BOOL* useSuspendValues, WORD* suspendValue, WORD* suspendMode, BYTE* clkDiv)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof(report));

    // Report ID
    report[0] = GETSET_PIN_CONFIG;

    // Get feature report from the device
    if (HidDevice_GetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        memcpy(pinConfig, &report[1], 14);
        *suspendValue	= MAKEWORD(report[16], report[15]);
        *suspendMode	= MAKEWORD(report[18], report[17]);
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

HID_UART_STATUS Get2114Status(CHIDtoUART* device, BYTE* pCP2114Status)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof(report));

    // Report ID
    report[0] = GET_DEVICE_STATUS;

    // Get feature report from the device
    if (HidDevice_GetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        *pCP2114Status = report[1];
        status = HID_UART_SUCCESS;
    }

    return status;
}

HID_UART_STATUS Get2114Caps(CHIDtoUART* device, PCP2114_CAPS_STRUCT pCP2114CapsStruct)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof(report));

    // Report ID
    report[0] = GET_DEVICE_CAPS;

    // Get feature report from the device
    if (HidDevice_GetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        memcpy(pCP2114CapsStruct, &report[1], sizeof(CP2114_CAPS_STRUCT));
        status = HID_UART_SUCCESS;
    }

    return status;
}

HID_UART_STATUS Set2114RamConfig(CHIDtoUART* device, PCP2114_RAM_CONFIG_STRUCT pCP2114RamConfigStruct)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

    // NOTE: The firmware and this library code assumes that RAMConfig fits in 
    // one packet. Since byte[0] is the report ID, the maximum RAMConfig size 
    // (including the 2-byte length) is 63 bytes.

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof(report));

    report[0] = SET_RAM_CONFIG;         // Report ID
    BYTE configSize;
    if (device->cp2114ConfigFormat == 0x01)
    {
        configSize = SIZE_RAM_CONFIG_B01;
    }
    else if (device->cp2114ConfigFormat == 0x02)
    {
        configSize = SIZE_RAM_CONFIG_B02;
    }
    else
    {
        // Unexpected configFormat ID
        status = HID_UART_UNSUPPORTED_CONFIG_FORMAT;
        return status;
    }
    memcpy(&report[1], pCP2114RamConfigStruct, configSize);

    // Set feature report on the device
    if (HidDevice_SetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        BYTE requestStatus = 0x00;
        if (Get2114Status(device, &requestStatus) == HID_DEVICE_SUCCESS)
        {
            status = requestStatus;
        }
    }

    return status;
}

HID_UART_STATUS Get2114RamConfig(CHIDtoUART* device, PCP2114_RAM_CONFIG_STRUCT pCP2114RamConfigStruct)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof(report));

    // Report ID
    report[0] = GET_RAM_CONFIG;

    // Get feature report from the device
    if (HidDevice_GetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        BYTE configSize;
        if (device->cp2114ConfigFormat == 0x01)
        {
            configSize = SIZE_RAM_CONFIG_B01;
        }
        else if (device->cp2114ConfigFormat == 0x02)
        {
            configSize = SIZE_RAM_CONFIG_B02;
        }
        else
        {
            // Unexpected configFormat ID
            status = HID_UART_DEVICE_NOT_SUPPORTED;
            return status;
        }
        memcpy(pCP2114RamConfigStruct, &report[1], configSize);

        status = HID_UART_SUCCESS;
    }

    return status;
}

HID_UART_STATUS SetDacRegisters(CHIDtoUART* device, BYTE* pDacConfigBuffer, BYTE dacConfigBufferLength)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;
    BYTE blockLength = 0;
    BYTE bytesCopied = 0;
    BYTE bytesToCopy = dacConfigBufferLength;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof(report));

    // Report ID
    report[0] = SET_DAC_REGISTERS;
    
    while (bytesCopied < bytesToCopy)
    {
        // We will break up the buffer into blocks for writing to the HID
        // device, and each block will contain report ID, size of payload, then
        // at most 62 bytes of payload - we keep copying blocks of data until we
        // have transferred the entire buffer
        blockLength = bytesToCopy - bytesCopied;
        if (blockLength > DAC_REGISTERS_PAYLOAD_MAX_LEN)
        {
            blockLength = DAC_REGISTERS_PAYLOAD_MAX_LEN;
        }
        report[1] = blockLength;
        memset(&report[2], 0x00, sizeof(report) - 2);
        memcpy(&report[2], pDacConfigBuffer + bytesCopied, blockLength);

        // Set feature report on the device
        if (HidDevice_SetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
        {
            bytesCopied += blockLength;	    
        }
        else
        {
            break;
        }
    }

    status = HID_DEVICE_SUCCESS;
    return status;
}

HID_UART_STATUS GetDacRegisters(CHIDtoUART* device, BYTE dacStartAddress, BYTE dacRegistersToRead, BYTE* pDacConfigBuffer)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;
    BYTE bytesCopied = 0;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof(report));

    // Report ID
    report[0] = SET_PARAMS_FOR_NEXT_GET;
    // Next ReportID + Params
    report[1] = GET_DAC_REGISTERS;
    report[2] = 2;	// Number of parameters
    report[3] = dacStartAddress;
    report[4] = dacRegistersToRead;

    if (HidDevice_SetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        memset(report, 0x00, sizeof(report));

        // Report ID
        report[0] = GET_DAC_REGISTERS;

        while (bytesCopied < dacRegistersToRead)
        {
            // Get feature report from the device
            if (HidDevice_GetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
            {
                if ((report[0] == GET_DAC_REGISTERS) &&
                    (report[1] <= DAC_REGISTERS_PAYLOAD_MAX_LEN))
                {
                    memcpy(pDacConfigBuffer + bytesCopied, &report[2], report[1]);
                    bytesCopied += report[1];
                }
            }
        }

        BYTE requestStatus = 0x00;

        if (Get2114Status(device, &requestStatus) == HID_DEVICE_SUCCESS)
        {
            status = requestStatus;
        }
    }

    return status;
}


HID_UART_STATUS Get2114OtpConfig(CHIDtoUART* device, BYTE ConfigNumber, PCP2114_CONFIG_STRUCT pConfigStruct)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;
    WORD bytesCopied = 0;  // BUGFIX: CP2114FW-74 (Failure to read config larger than 256 bytes)

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof(report));

    // Report ID
    report[0] = SET_PARAMS_FOR_NEXT_GET;
    // Next ReportID + Params
    report[1] = GET_OTP_CONFIG;
    report[2] = 1;	// Number of parameters
    report[3] = ConfigNumber;

    // Set feature report for the index needed
    if (HidDevice_SetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        memset(report, 0xCC, sizeof(report));

        // Report ID
        report[0] = GET_OTP_CONFIG;

        // Get feature report from the device
        if (HidDevice_GetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
        {
            if ((report[0] == GET_OTP_CONFIG) &&
                (report[1] <= 62))
            {
                memcpy((BYTE*)pConfigStruct, &report[2], report[1]);
                bytesCopied += report[1];

                // Adjust endianess from what the firmware returns for the WORD
                // length
                //WORD length = pConfigStruct->ramConfig.Length;
            }
        }

        // Now we have the header, so we will loop on any other packets, if
        // they exist
        while (bytesCopied < pConfigStruct->ramConfig.Length)
        {
            memset(&report[1], 0x00, sizeof(report) - 1);

            // Get feature report from the device
            if (HidDevice_GetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
            {
                if ((report[0] == GET_OTP_CONFIG) &&
                    (report[1] <= 62))
                {
                    memcpy((BYTE*)pConfigStruct + bytesCopied, &report[2], report[1]);
                    bytesCopied += report[1];
                }
            }
            else
            {
                break;
            }
        }

        BYTE requestStatus = 0x00;

        if (Get2114Status(device, &requestStatus) == HID_DEVICE_SUCCESS)
        {
            status = requestStatus;
        }
    }

    return status;
}

HID_UART_STATUS CreateNew2114OtpConfig(CHIDtoUART* device, WORD configBufferLength, BYTE* pConfigBuffer)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;
    WORD blockLength = 0;
    WORD bytesCopied = 0;
    WORD bytesToCopy = configBufferLength+2; // Include the length bytes itself
    WORD bufferOffset = 0;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof(report));

    // Report ID
    report[0] = CREATE_OTP_CONFIG;

    while (bytesCopied < bytesToCopy)
    {
        // We will break up the buffer into blocks for writing to the HID
        // device, and each block will contain report ID, size of payload, then
        // at most 62 bytes of payload - we keep copying blocks of data until we
        // have transferred the entire buffer. This contains a special case where
        // the first packed needs to contain some header information, the rest of
        // the packets contain the buffer data
        blockLength = (bytesToCopy - bytesCopied);
        if (blockLength > 62)
        {
            blockLength = 62;
        }

        report[1] = (BYTE)blockLength;

        if (bytesCopied > 0)
        {
            memset(&report[2], 0x00, sizeof(report) - 2);
            memcpy(&report[2], pConfigBuffer + bufferOffset, blockLength);
            bufferOffset += blockLength;
        }
        else
        {
            report[2] = (configBufferLength+2) & 0xFF;
            report[3] = ((configBufferLength+2) & 0xFF00) >> 8;
            memcpy(&report[4], pConfigBuffer, blockLength-2);
            bufferOffset = blockLength-2;
        }
        
        // Set feature report on the device
        if (HidDevice_SetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
        {
            bytesCopied += blockLength;
        }
        else
        {
            break;
        }
    }

    BYTE requestStatus = 0x00;

    if (Get2114Status(device, &requestStatus) == HID_DEVICE_SUCCESS)
    {
        status = requestStatus;
    }

    return status;
}

HID_UART_STATUS SetBoot2114(CHIDtoUART* device, BYTE cp2114ConfigNumber)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof(report));

    // Report ID
    report[0] = SET_BOOT_CONFIG;
    report[1] = cp2114ConfigNumber;

    // Get feature report from the device
    if (HidDevice_SetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        BYTE requestStatus = 0x00;

        if (Get2114Status(device, &requestStatus) == HID_DEVICE_SUCCESS)
        {
            status = requestStatus;
        }
    }

    return status;
}

HID_UART_STATUS ReadOTP(CHIDtoUART* device, UINT cp2114Address ,BYTE* pReadBuffer, UINT readLength)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;
    UINT bytesCopied = 0;
    
    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof(report));

    // Report ID
    report[0] = SET_PARAMS_FOR_NEXT_GET;
    // Next ReportID + Params
    report[1] = GET_OTP_ALLCONFIG;
    report[2] = 4;	// Number of parameters
    report[3] = (readLength & 0xFF00) >> 8;// blocklength
    report[4] = readLength & 0xFF; 
    report[5] = (cp2114Address & 0xFF00) >> 8;//dacConfigNumber;
    report[6] = cp2114Address & 0xFF;


    // Set feature report for the index needed
    
    if (HidDevice_SetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        memset(report, 0xCC, sizeof(report));

        // Report ID
        report[0] = GET_OTP_ALLCONFIG; 
        // Get feature report from the device
        if (HidDevice_GetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
        {
            if ((report[0] == GET_OTP_ALLCONFIG) &&
                (report[1] <= 62))
            {
                //&report[3], pWriteBuffer + bytesCopied, blockLength
                memcpy(pReadBuffer + bytesCopied, &report[2], report[1]);
                bytesCopied += report[1];
            }
        }

        while (bytesCopied < readLength)
        {
            memset(&report[1], 0x00, sizeof(report) - 1);

            // Get feature report from the device
            if (HidDevice_GetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
            {
                if ((report[0] == GET_OTP_ALLCONFIG) &&
                    (report[1] <= 62))
                {
                    memcpy(pReadBuffer  + bytesCopied, &report[2], report[1]);
                    bytesCopied += report[1];
                }
            }
            else
            {
                break;
            }
        }
        BYTE requestStatus = 0x00;

        if (Get2114Status(device, &requestStatus) == HID_DEVICE_SUCCESS)
        {
            status = requestStatus;
        }

    }
    return status;
}

HID_UART_STATUS WriteOTP(CHIDtoUART* device, UINT cp2114Address ,BYTE* pWriteBuffer, UINT writeLength)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;
    UINT blockLength = 0;
    UINT bytesCopied = 0;
    UINT bytesToCopy = writeLength;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof(report));

    // Report ID
    report[0] = SET_OTP_ALLCONFIG;
    
    while (bytesCopied < bytesToCopy)
    {
        // We will break up the buffer into blocks for writing to the HID
        // device, and each block will contain report ID, size of payload, 
        // then at most 62 bytes of payload - we keep copying blocks of data until we
        // have transferred the entire buffer. This contains a special case where
        // the first packed needs to contain some header information, the rest of
        // the packets contain the buffer data
        blockLength = (bytesToCopy - bytesCopied);
        if (blockLength > 62)
        {
            blockLength = 62;
        }
        report[1] = blockLength;	
        if (bytesCopied > 0)
        {
            memset(&report[2], 0x00, sizeof(report) - 2);
            memcpy(&report[2], pWriteBuffer + bytesCopied, blockLength);
        }
        else
        {
            report[2] = (writeLength & 0xFF00) >> 8;// blocklength
            report[3] = writeLength & 0xFF; 
            report[4] = (cp2114Address & 0xFF00) >> 8;
            report[5] = cp2114Address & 0xFF;
            blockLength -= 4;				// actual length = 62 - 4
            report[1] = blockLength;
            memcpy(&report[6], pWriteBuffer, blockLength);
        }
        
        // Set feature report on the device
        if (HidDevice_SetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
        {
            bytesCopied += blockLength;
        }
        else
        {
            break;
        }
    }

    BYTE requestStatus = 0x00;
    if (Get2114Status(device, &requestStatus) == HID_DEVICE_SUCCESS)
    {
        status = requestStatus;
    }
    return status;
}

// I2cWriteData(): Perform I2C write transfer to arbitrary slave device
//
// The USB Report_Buffer format is:
//
// Offset | Contents
// =============================================================================
//   0    | Report ID (I2C_WRITE_DATA)
//   1    | I2C Slave Address.
//   2    | Number of bytes to write (not including SLA). Max is 61.
// 3..63  | Data to write (not including SLA).
HID_UART_STATUS I2cWriteData(CHIDtoUART* device, BYTE slaveAddress, BYTE* pWriteBuffer, BYTE writeLength)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;
    //BYTE blockLength = 0;
    //BYTE bytesCopied = 0;
    //BYTE bytesToCopy = writeLength;

    BYTE report[FEATURE_REPORT_LENGTH];
    memset(report, 0x00, sizeof(report));

    report[0] = I2C_WRITE_DATA; // Report ID
    report[1] = slaveAddress;
    report[2] = writeLength;
    memcpy(&report[3], pWriteBuffer, writeLength);

    status = HidDevice_SetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH);
    return status;
}


// I2c_read_data()  Perform I2C Read transfer from arbitrary slave device
//
// The host first calls SetReport parameters for the Read are passed in the setParamsForNextGet
// structure, which is passed using a SetReport preceding the GetReport which executes the Read.
// The format of the setParamsForNextGet structure when used for Read is:
//
//   setParamsForNextGet.reportID  = I2C_READ_DATA
//   setParamsForNextGet.numParams = 5
//     params[0]: Slave Address
//     params[1]: Number of bytes to read (max 60)
//     params[2]: Number of bytes to write (max 2)
//     params[3]: Register Address (first byte)
//     params[4]: Register Address (second byte, if necessary)
//
// The USB Report_Buffer is used to return the result. The format is:
//
// Offset | Contents
// ========================================================
//   0    | Report ID (I2C_READ_DATA)
//   1    | I2C Slave Address that was used
//   2    | Transfer status (0 = success, nonzero = error).
//   3    | Number of bytes that were read.
// 4..63  | Data that was read.
HID_UART_STATUS I2cReadData(CHIDtoUART* device, BYTE slaveAddress, BYTE* pWriteBuffer, BYTE writeLength, BYTE* pReadBuffer, BYTE readLength)
{
    HID_UART_STATUS status = HID_UART_DEVICE_IO_FAILED;
    //BYTE bytesCopied = 0;
    BYTE report[FEATURE_REPORT_LENGTH];
    
    memset(report, 0x00, sizeof(report));

    report[0] = SET_PARAMS_FOR_NEXT_GET;    // Report ID
    report[1] = I2C_READ_DATA;  // ID of Report to follow SetParamsForNextGet
    report[2] = 5;  // Number of parameters
    report[3] = slaveAddress;
    report[4] = readLength;
    report[5] = writeLength;
    report[6] = pWriteBuffer[0];
    report[7] = pWriteBuffer[1];    // May not be used but copy it anyway

    // Bugfix CP2114FW-124: Write data is now passed in SetParamsForNextGet

    // Send 'SetParametersForNextGet' report with parameters for forthcoming Read
    if (HidDevice_SetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
    {
        memset(report, 0x00, sizeof(report));

        report[0] = I2C_READ_DATA;        // Report ID
        report[1] = slaveAddress;

        // Read I2C data
        if (HidDevice_GetFeatureReport_Control(device->hid, report, FEATURE_REPORT_LENGTH) == HID_DEVICE_SUCCESS)
        {
            if ((report[2] == HID_DEVICE_SUCCESS) && (report[3] == readLength))
            {
                status = HID_DEVICE_SUCCESS;
                memcpy(pReadBuffer, &report[4], readLength);
            }

            BYTE requestStatus = 0x00;
            if (Get2114Status(device, &requestStatus) == HID_DEVICE_SUCCESS)
            {
                status = requestStatus;
            }
        }
    }
    return status;
}


/////////////////////////////////////////////////////////////////////////////
// Exported Library Functions
/////////////////////////////////////////////////////////////////////////////

// HidUart_GetVersions
HID_UART_STATUS CP2114_GetVersions(HID_UART_DEVICE device, BYTE* api_version, BYTE* fw_version, BYTE* config_version)
{
    HID_UART_STATUS     status      = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART*         hidUart     = ((CHIDtoUART*)device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (api_version && fw_version && config_version)
        {
            // CP2114 Only
            if (hidUart->partNumber == HID_UART_PART_CP2114)
            {
                status = GetVersions(hidUart, api_version, fw_version, config_version);
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

// CP2114_SetPinConfig
HID_UART_STATUS CP2114_SetPinConfig(HID_UART_DEVICE device, BYTE* pinConfig, BOOL useSuspendValues, WORD suspendValue, WORD suspendMode,BYTE clkDiv)
{
    HID_UART_STATUS		status		= HID_UART_UNKNOWN_ERROR;
    CHIDtoUART*			hidUart		= ((CHIDtoUART*)device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (pinConfig)
        {
            // CP2114 Only
            if (hidUart->partNumber == HID_UART_PART_CP2114)
            {
                status = SetPinConfig(hidUart, pinConfig, useSuspendValues, suspendValue, suspendMode,clkDiv);
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

// CP2114_GetPinConfig
HID_UART_STATUS CP2114_GetPinConfig(HID_UART_DEVICE device, BYTE* pinConfig, BOOL* useSuspendValues, WORD* suspendValue, WORD* suspendMode,BYTE* clkDiv)
{
    HID_UART_STATUS		status		= HID_UART_UNKNOWN_ERROR;
    CHIDtoUART*			hidUart		= ((CHIDtoUART*)device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (pinConfig && useSuspendValues && suspendValue && suspendMode && clkDiv)
        {
            // CP2114 Only
            if (hidUart->partNumber == HID_UART_PART_CP2114)
            {
                status = GetPinConfig(hidUart, pinConfig, useSuspendValues, suspendValue, suspendMode,clkDiv);
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

// CP2114_GetDeviceStatus
HID_UART_STATUS CP2114_GetDeviceStatus(HID_UART_DEVICE device, BYTE* pCP2114Status)
{
    HID_UART_STATUS		status		= HID_UART_UNKNOWN_ERROR;
    CHIDtoUART*			hidUart		= ((CHIDtoUART*)device);
    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (pCP2114Status)
        {
            // CP2114 Only
            if (hidUart->partNumber == HID_UART_PART_CP2114)
            {
                status = Get2114Status(hidUart, pCP2114Status);
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

// CP2114_GetDeviceCaps
HID_UART_STATUS CP2114_GetDeviceCaps(HID_UART_DEVICE device, PCP2114_CAPS_STRUCT pCP2114CapsStruct)
{
    HID_UART_STATUS		status		= HID_UART_UNKNOWN_ERROR;
    CHIDtoUART*			hidUart		= ((CHIDtoUART*)device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (pCP2114CapsStruct)
        {
            // CP2114 Only
            if (hidUart->partNumber == HID_UART_PART_CP2114)
            {
                status = Get2114Caps(hidUart, pCP2114CapsStruct);
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

// CP2114_SetRamConfig
HID_UART_STATUS CP2114_SetRamConfig(HID_UART_DEVICE device, PCP2114_RAM_CONFIG_STRUCT pCP2114RamConfigStruct)
{
    HID_UART_STATUS		status		= HID_UART_UNKNOWN_ERROR;
    CHIDtoUART*			hidUart		= ((CHIDtoUART*)device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (pCP2114RamConfigStruct)
        {
            // CP2114 Only
            if (hidUart->partNumber == HID_UART_PART_CP2114)
            {
                status = Set2114RamConfig(hidUart, pCP2114RamConfigStruct);
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

// CP2114_GetRamConfig
HID_UART_STATUS CP2114_GetRamConfig(HID_UART_DEVICE device, PCP2114_RAM_CONFIG_STRUCT pCP2114RamConfigStruct)
{
    HID_UART_STATUS		status		= HID_UART_UNKNOWN_ERROR;
    CHIDtoUART*			hidUart		= ((CHIDtoUART*)device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (pCP2114RamConfigStruct)
        {
            // CP2114 Only
            if (hidUart->partNumber == HID_UART_PART_CP2114)
            {
                status = Get2114RamConfig(hidUart, pCP2114RamConfigStruct);
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

// CP2114_SetDacRegisters
HID_UART_STATUS CP2114_SetDacRegisters(HID_UART_DEVICE device, BYTE* pDacConfigBuffer, BYTE dacConfigBufferLength)
{
    HID_UART_STATUS		status		= HID_UART_UNKNOWN_ERROR;
    CHIDtoUART*			hidUart		= ((CHIDtoUART*)device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (pDacConfigBuffer)
        {
            // CP2114 Only
            if (hidUart->partNumber == HID_UART_PART_CP2114)
            {
                status = SetDacRegisters(hidUart, pDacConfigBuffer, dacConfigBufferLength);
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

// CP2114_GetDacRegisters
HID_UART_STATUS CP2114_GetDacRegisters(HID_UART_DEVICE device, BYTE dacStartAddress, BYTE dacRegistersToRead, BYTE* pDacConfigBuffer)
{
    HID_UART_STATUS		status		= HID_UART_UNKNOWN_ERROR;
    CHIDtoUART*			hidUart		= ((CHIDtoUART*)device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (pDacConfigBuffer)
        {
            // CP2114 Only
            if (hidUart->partNumber == HID_UART_PART_CP2114)
            {
                status = GetDacRegisters(hidUart, dacStartAddress, dacRegistersToRead, pDacConfigBuffer);
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

// CP2114_GetOtpConfig
HID_UART_STATUS CP2114_GetOtpConfig(HID_UART_DEVICE device, BYTE cp2114ConfigNumber, PCP2114_CONFIG_STRUCT pCP2114ConfigStruct)
{
    HID_UART_STATUS		status		= HID_UART_UNKNOWN_ERROR;
    CHIDtoUART*			hidUart		= ((CHIDtoUART*)device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (pCP2114ConfigStruct)
        {
            // CP2114 Only
            if (hidUart->partNumber == HID_UART_PART_CP2114)
            {
                status = Get2114OtpConfig(hidUart, cp2114ConfigNumber, pCP2114ConfigStruct);
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

// CP2114_CreateOtpConfig
HID_UART_STATUS CP2114_CreateOtpConfig(HID_UART_DEVICE device, WORD configBufferLength, BYTE* pConfigBuffer)
{
    HID_UART_STATUS		status		= HID_UART_UNKNOWN_ERROR;
    CHIDtoUART*			hidUart		= ((CHIDtoUART*)device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (pConfigBuffer)
        {
            // CP2114 Only
            if (hidUart->partNumber == HID_UART_PART_CP2114)
            {
                status = CreateNew2114OtpConfig(hidUart, configBufferLength, pConfigBuffer);
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

// CP2114_SetBootConfig
HID_UART_STATUS CP2114_SetBootConfig(HID_UART_DEVICE device, BYTE cp2114ConfigNumber)
{
    HID_UART_STATUS		status		= HID_UART_UNKNOWN_ERROR;
    CHIDtoUART*			hidUart		= ((CHIDtoUART*)device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // CP2114 Only
        if (hidUart->partNumber == HID_UART_PART_CP2114)
        {
            status = SetBoot2114(hidUart, cp2114ConfigNumber);
        }
        else
        {
            status = HID_UART_DEVICE_NOT_SUPPORTED;
        }        
    }
    else
    {
        status = HID_UART_INVALID_DEVICE_OBJECT;
    }

    return status;
}

// CP2114_ReadOTP
HID_UART_STATUS CP2114_ReadOTP(HID_UART_DEVICE device, UINT cp2114Address ,BYTE* pReadBuffer, UINT readLength)
{
    HID_UART_STATUS		status		= HID_UART_UNKNOWN_ERROR;
    CHIDtoUART*			hidUart		= ((CHIDtoUART*)device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (pReadBuffer)
        {
            // CP2114 Only
            if (hidUart->partNumber == HID_UART_PART_CP2114)
            {
                status = ReadOTP(hidUart, cp2114Address, pReadBuffer,readLength);
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

// CP2114_WriteOTP
HID_UART_STATUS CP2114_WriteOTP(HID_UART_DEVICE device, UINT cp2114Address ,BYTE* pWriteBuffer, UINT writeLength)
{
    HID_UART_STATUS		status		= HID_UART_UNKNOWN_ERROR;
    CHIDtoUART*			hidUart		= ((CHIDtoUART*)device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (pWriteBuffer)
        {
            // CP2114 Only
            if (hidUart->partNumber == HID_UART_PART_CP2114)
            {
                status = WriteOTP(hidUart, cp2114Address, pWriteBuffer, writeLength);
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

// CP2114_I2cWriteData
HID_UART_STATUS CP2114_I2cWriteData(HID_UART_DEVICE device, BYTE slaveAddress, BYTE* pWriteBuffer, BYTE writeLength)
{
    HID_UART_STATUS     status      = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART*         hidUart     = ((CHIDtoUART*)device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (pWriteBuffer)
        {
            // CP2114 Only
            if (hidUart->partNumber == HID_UART_PART_CP2114)
            {
                status = I2cWriteData(hidUart, slaveAddress, pWriteBuffer, writeLength);
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

// CP2114_I2cReadData
HID_UART_STATUS CP2114_I2cReadData(HID_UART_DEVICE device, BYTE slaveAddress, BYTE* pWriteBuffer, BYTE writeLength, BYTE* pReadBuffer, BYTE readLength)
{
    HID_UART_STATUS     status      = HID_UART_UNKNOWN_ERROR;
    CHIDtoUART*         hidUart     = ((CHIDtoUART*)device);

    // Check device object
    if (DeviceList.Validate(hidUart))
    {
        // Check pointers
        if (pReadBuffer)
        {
            // CP2114 Only
            if (hidUart->partNumber == HID_UART_PART_CP2114)
            {
                status = I2cReadData(hidUart, slaveAddress, pWriteBuffer, writeLength, pReadBuffer, readLength);
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
