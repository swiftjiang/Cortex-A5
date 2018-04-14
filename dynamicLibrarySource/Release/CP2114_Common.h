//-----------------------------------------------------------------------------
// CP2114_Common.h
//-----------------------------------------------------------------------------
// Copyright 2012 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Header file shared by both CP2114 firmware and the Configurator.
//
//
// How To Test:    See Readme.txt
//
//
// Target:         C8051F381/T627
// Tool chain:     Keil C51
//                 Silicon Laboratories IDE
// Command Line:   See Readme.txt
// Project Name:   CP2114
//
//-----------------------------------------------------------------------------

#ifndef _CP2114_DATA_COMMON_H_
#define _CP2114_DATA_COMMON_H_

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

// API version: this is used to detect compatibility between the firmware and
// the Configurator. If API change is significant enough to require the Configurator
// to change, the firmware API version and the Configurator API version (the integer
// part) must be changed at the same time. Otherwise, Configurator won't connect to
// the device.

/* API Evolving History
 *  1 - implied without explicit API number in SetID.
 *  2 - First explicit API number of Configurator, internal only
 *  3 - Overhaul of Configurator
 */

#define  API_VERSION       0x05
#define  PART_NUMBER       0x0E

//--------------------------
// Locations
//--------------------------
// Locations must match settings of 'Project -> Toolchain -> Linker cmd line args'
#define DEFAULTDATA_ADDRESS  0x6600
#define USERDATA_ADDRESS     0x6800

#define CUSTOM_DATA_SIZE     (6*1024)

// Global errors & status
#define kStatusSuccess                 0x00  // Last command produced no error

// NOTE:Any changes to the following definitions must also be made in the PC software 'HIDtoUART.h' file.

//  Config
#define kInvalidConfigNumber           0x20  // Requested config number >= MAX_CONFIGS
#define kBootIndicesDepleted           0x21  // All Dev boot indexes have been used
#define kRequestedConfigNotPresent     0x22  // Pointer to requested Dev config is 0xFFFF
#define kRequestedConfigInvalid        0x23  // Config is invalid or not supported
#define kConfigPointersDepleted        0x24  // All Config pointer slots have been used
#define kConfigSpaceDepleted           0x25  // Not enough space to save the new config
#define kBootIndexUnchanged            0x26  // The user-specified boot index is already the current boot index
#define kConfigUnchanged               0x27  // The Config is already as the user requested


#define kInvalidNumberOfCachedParams   0x40  // Specified tSetParamsForNextGet.params > MAX_CACHED_PARAMS
#define kUnexpectedCacheData           0x41  // Something in tSetParamsForNextGet was not as expected

//-------------------------------------------------------
// 2114 Capabilities
// Note: 
//-------------------------------------------------------
typedef struct 
{
    U8    availableBootIndices;
    U8    availableOtpConfigs;
    U8    currentBootConfig;
    U8    availableOtpConfigSpace_LSB;
    U8    availableOtpConfigSpace_MSB;
}tDeviceCaps;

#define SIZE_DEVICE_CAPS   (sizeof(tDeviceCaps))

// Config block
//
// NOTE: There is no Config typedef; the following
// definitions describe the Config block.
//
//
#define CONFIG_LENGTH_LSB_OFFSET         0
#define CONFIG_LENGTH_MSB_OFFSET         1
#define CONFIG_VERSION_OFFSET            2     // Offset to DAC config version
#define CONFIG_USER_RESERVED_OFFSET      3     // Offset to user reserved byte
#define CONFIG_I2C_SLAVE_ADDRESS_OFFSET  4
#define CONFIG_PROPERTY_OFFSET           5     // offset to codec property bitfields
// bit 0 Audio format (0=I2S, 1=left justified)
    #define CONFIG_PROPERTY_AUDIO_FORMAT_MASK              0x01
    #define CONFIG_PROPERTY_AUDIO_FORMAT_POS               0x00
    #define CONFIG_PROPERTY_AUDIO_FORMAT_I2S                  0
    #define CONFIG_PROPERTY_AUDIO_FORMAT_LJ                   1

// bit 1 Audio mode for LJ, 16bit or 32bit
    #define CONFIG_PROPERTY_LJ_AUDIO_MODE_MASK             0x02
    #define CONFIG_PROPERTY_LJ_AUDIO_MODE_POS              0x01
    #define CONFIG_PROPERTY_LJ_AUDIO_MODE_16BIT               0
    #define CONFIG_PROPERTY_LJ_AUDIO_MODE_24BIT               1

// bit 2 DAC mute control (0=Off, 1=On)
    #define CONFIG_PROPERTY_DAC_VOLUME_CONTROL_MASK        0x04
    #define CONFIG_PROPERTY_DAC_VOLUME_CONTROL_POS         0x02
    #define CONFIG_PROPERTY_DAC_VOLUME_CONTROL_OFF            0
    #define CONFIG_PROPERTY_DAC_VOLUME_CONTROL_ON             1

// bit 3 I2C register size (0=8bit, 1=16 bit)
    #define CONFIG_PROPERTY_I2C_REGISTER_SIZE_MASK         0x08
    #define CONFIG_PROPERTY_I2C_REGISTER_SIZE_POS          0x03
    #define CONFIG_PROPERTY_I2C_REGISTER_SIZE_8BIT            0
    #define CONFIG_PROPERTY_I2C_REGISTER_SIZE_16BIT           1

// bit 4 I2C protocol on read (0=stop, 1=repeated start)
    #define CONFIG_PROPERTY_I2C_PROTOCOL_MASK              0x10
    #define CONFIG_PROPERTY_I2C_PROTOCOL_POS               0x04
    #define CONFIG_PROPERTY_I2C_PROTOCOL_STOP                 0
    #define CONFIG_PROPERTY_I2C_PROTOCOL_SR                   1

// bit 5 I2C max speed (0=400KHz, 1=100KHz)
    #define CONFIG_PROPERTY_I2C_MAX_SPEED_MASK             0x20
    #define CONFIG_PROPERTY_I2C_MAX_SPEED_POS              0x05
    #define CONFIG_PROPERTY_I2C_MAX_SPEED_400KHZ              0
    #define CONFIG_PROPERTY_I2C_MAX_SPEED_100KHZ              1

// bit 6 stream sync type (0=ASYNC, 1=SYNC)
    #define CONFIG_PROPERTY_STREAM_SYNC_MASK               0x40
    #define CONFIG_PROPERTY_STREAM_SYNC_POS                0x06
    #define CONFIG_PROPERTY_STREAM_ASYNC                      0
    #define CONFIG_PROPERTY_STREAM_SYNC                       1

// bit 7 DAC mute control via mute bit (0=do not use mute bit, 1=use mute bit)
// If this bit is set to 1, offset 14, 15, 16, 17, 19 apply.
    #define CONFIG_PROPERTY_DAC_MUTE_VIA_MUTEBIT_MASK              0x80
    #define CONFIG_PROPERTY_DAC_MUTE_VIA_MUTEBIT_POS               0x07
    #define CONFIG_PROPERTY_DAC_MUTE_VIA_MUTEBIT_DISABLE              0
    #define CONFIG_PROPERTY_DAC_MUTE_VIA_MUTEBIT_ENABLE               1

#define CONFIG_VOLUME_MIN_OFFSET         6      // 8bit Signed  
#define CONFIG_VOLUME_MAX_OFFSET         7      // 8bit Signed
#define CONFIG_VOLUME_RES_OFFSET         8      // Offset to RES element, encoded as steps/dB
#define CONFIG_SYS_PROPERTY_OFFSET       9      // offset to SYS settings such as sysclk
// bit 0 MCK/LRCK ratio (0=250, 1=256)
    #define CONFIG_SYS_PROPERTY_MCLK_LRCK_RATIO_MASK       0x01
    #define CONFIG_SYS_PROPERTY_MCLK_LRCK_RATIO_POS        0x00
    #define CONFIG_SYS_PROPERTY_MCLK_LRCK_RATIO_250           0
    #define CONFIG_SYS_PROPERTY_MCLK_LRCK_RATIO_256           1

// bit 1 SYSCLK frequency (0=48MHz, 1=49.152MHz)
    #define CONFIG_SYS_PROPERTY_SYSCLK_FREQ_MASK           0x02
    #define CONFIG_SYS_PROPERTY_SYSCLK_FREQ_POS            0x01
    #define CONFIG_SYS_PROPERTY_SYSCLK_FREQ_48M               0
    #define CONFIG_SYS_PROPERTY_SYSCLK_FREQ_49M               1   // Actually 49.152 MHz

// bit 2 SYSCLK source (0=Internal oscillator, 1=external clock)
    #define CONFIG_SYS_PROPERTY_SYSCLK_SOURCE_MASK         0x04
    #define CONFIG_SYS_PROPERTY_SYSCLK_SOURCE_POS          0x02
    #define CONFIG_SYS_PROPERTY_SYSCLK_SOURCE_INTOSC          0
    #define CONFIG_SYS_PROPERTY_SYSCLK_SOURCE_EXTOSC          1


// bit 3 USBCLK source (0=Internal oscillator, 1=external clock)
    #define CONFIG_SYS_PROPERTY_USBCLK_SOURCE_MASK         0x08
    #define CONFIG_SYS_PROPERTY_USBCLK_SOURCE_POS          0x03
    #define CONFIG_SYS_PROPERTY_USBCLK_SOURCE_INTOSC          0
    #define CONFIG_SYS_PROPERTY_USBCLK_SOURCE_EXTOSC          1

//bit 4 VOL_UPDATE     (0=Not needed, 1=needed)
    #define CONFIG_SYS_PROPERTY_VOL_UPDATE_MASK            0x10
    #define CONFIG_SYS_PROPERTY_VOL_UPDATE_POS             0x04
    #define CONFIG_SYS_PROPERTY_VOL_UPDATE_OFF                0
    #define CONFIG_SYS_PROPERTY_VOL_UPDATE_ON                 1

//bit 5 SECONDARY_REG_DIFF    (0=SECONDARY not different from PRIMARY, 1=diff)
    #define CONFIG_SYS_PROPERTY_SECONDARY_REG_DIFF_MASK    0x20
    #define CONFIG_SYS_PROPERTY_SECONDARY_REG_DIFF_POS     0x05
    #define CONFIG_SYS_PROPERTY_SECONDARY_REG_DIFF_OFF        0
    #define CONFIG_SYS_PROPERTY_SECONDARY_REG_DIFF_ON         1

//bit 6 RECORD_ENABLE (0=Record disable, 1=Record enable)
    #define CONFIG_SYS_PROPERTY_RECORD_ENABLE_MASK         0x40
    #define CONFIG_SYS_PROPERTY_RECORD_ENABLE_POS          0x06
    #define CONFIG_SYS_PROPERTY_RECORD_DISABLE                0
    #define CONFIG_SYS_PROPERTY_RECORD_ENABLE                 1

//bit 7 VOL_REG_SIGNED (0=Unsigned, 1=signed)
    #define CONFIG_SYS_PROPERTY_VOL_REG_SIGNED_MASK        0x80
    #define CONFIG_SYS_PROPERTY_VOL_REG_SIGNED_POS         0x07
    #define CONFIG_SYS_PROPERTY_VOL_REG_UNSIGNED              0
    #define CONFIG_SYS_PROPERTY_VOL_REG_SIGNED                1

#define CONFIG_DAC_PRIMARY_VOL_L_REGADDR_OFFSET              10
#define CONFIG_DAC_PRIMARY_VOL_R_REGADDR_OFFSET              11
#define CONFIG_DAC_SECONDARY_VOL_L_REGADDR_OFFSET            12
#define CONFIG_DAC_SECONDARY_VOL_R_REGADDR_OFFSET            13
#define CONFIG_DAC_PRIMARY_MUTE_L_REGADDR_OFFSET             14
#define CONFIG_DAC_PRIMARY_MUTE_R_REGADDR_OFFSET             15
#define CONFIG_DAC_SECONDARY_MUTE_L_REGADDR_OFFSET           16
#define CONFIG_DAC_SECONDARY_MUTE_R_REGADDR_OFFSET           17
#define CONFIG_DAC_VOL_BSIZE_BPOS_OFFSET                     18
//bit 3:0 volume bits start location
    #define CONFIG_DAC_VOL_BITS_START_POS_MASK             0x0F
    #define CONFIG_DAC_VOL_BITS_START_POS_POS              0x00
//bit 7:4 volume bits size                            
    #define CONFIG_DAC_VOL_BITS_SIZE_MASK                  0xF0
    #define CONFIG_DAC_VOL_BITS_SIZE_POS                   0x04

#define CONFIG_DAC_MUTE_BPOS_OFFSET                          19  
//bit 3:0 right mute bit location
    #define CONFIG_DAC_R_MUTE_BIT_POS_MASK                 0x0F
    #define CONFIG_DAC_R_MUTE_BIT_POS_POS                  0x00
//bit 7:4 left mute bit location
    #define CONFIG_DAC_L_MUTE_BIT_POS_MASK                 0xF0
    #define CONFIG_DAC_L_MUTE_BIT_POS_POS                  0x04
#define CONFIG_DAC_VOL_REG_SETTING_MIN                       20
#define CONFIG_DAC_VOL_REG_SETTING_MAX                       21

// Update registers required if CONFIG_SYS_PROPERTY_VOL_UPDATE_ON
#define CONFIG_DAC_UPDATE_BPOS_OFFSET                        22  
//bit 3:0 right update bit location
    #define CONFIG_DAC_R_UPDATE_BIT_POS_MASK               0x0F
    #define CONFIG_DAC_R_UPDATE_BIT_POS_POS                0x00
//bit 7:4 left update bit location
    #define CONFIG_DAC_L_UPDATE_BIT_POS_MASK               0xF0
    #define CONFIG_DAC_L_UPDATE_BIT_POS_POS                0x04
#define CONFIG_DAC_PRIMARY_UPDATE_L_REGADDR_OFFSET           23
#define CONFIG_DAC_PRIMARY_UPDATE_R_REGADDR_OFFSET           24
#define CONFIG_DAC_SECONDARY_UPDATE_L_REGADDR_OFFSET         25
#define CONFIG_DAC_SECONDARY_UPDATE_R_REGADDR_OFFSET         26

#define CONFIG_DAC_MUTE_PROP_1_OFFSET                      27    // Offset to GPIO mute
// bit 0 Mute via GPIO enable (0=disable, 1=enable)
    #define CONFIG_DAC_MUTE_VIA_GPIO_ENABLE_MASK           0x01
    #define CONFIG_DAC_MUTE_VIA_GPIO_ENABLE_POS            0x00
    #define CONFIG_DAC_MUTE_VIA_GPIO_DISABLE                  0
    #define CONFIG_DAC_MUTE_VIA_GPIO_ENABLE                   1
// bit 1 GPIO state when mute
    #define CONFIG_DAC_MUTE_VIA_GPIO_STATE_MASK            0x02
    #define CONFIG_DAC_MUTE_VIA_GPIO_STATE_POS             0x01
// bit 7:4 GPIO number
    #define CONFIG_DAC_MUTE_VIA_GPIO_NUMBER_MASK           0xF0
    #define CONFIG_DAC_MUTE_VIA_GPIO_NUMBER_POS            0x04

#define CONFIG_DAC_MUTE_PROP_2_OFFSET                      28
// bit 0 mute via volume enable
// If set, offset 10, 11, 12, 13 will apply instead of 14, 15, 16, 17.
// Vol register setting when mute is specified at offset 29.
    #define CONFIG_DAC_MUTE_VIA_VOL_ENABLE_MASK            0x01
    #define CONFIG_DAC_MUTE_VIA_VOL_ENABLE_POS             0x00
    #define CONFIG_DAC_MUTE_VIA_VOL_DISABLE                   0
    #define CONFIG_DAC_MUTE_VIA_VOL_ENABLE                    1
// bit 1 mute by zero'ing playback samples
    #define CONFIG_DAC_MUTE_VIA_ZERO_SAMPLES_ENABLE_MASK   0x02
    #define CONFIG_DAC_MUTE_VIA_ZERO_SAMPLES_ENABLE_POS    0x01
    #define CONFIG_DAC_MUTE_VIA_ZERO_SAMPLES_DISABLE          0
    #define CONFIG_DAC_MUTE_VIA_ZERO_SAMPLES_ENABLE           1
// bit 7:2 reserved

#define  CONFIG_DAC_VOL_REG_SETTING_MUTE_OFFSET              29
  
// Reserve 2 bytes
#define CONFIG_DAC_ADDRESS_OFFSET                            32     // Offset to first address element
#define CONFIG_DAC_DATA_OFFSET                               33     // Offset to first data element
//-------------------------------------------------------
#define SIZE_RAM_CONFIG             CONFIG_DAC_ADDRESS_OFFSET

// This struct must match the definitions above
struct _RAM_CONFIG_STRUCT
{
    U16 Length;
    U8  Version;
    U8  UserReserved;
    U8  I2CAddress;
    U8  Prop;
    U8  VolMin;
    U8  VolMax;
    U8  VolRes;
    U8  SysProp;
    U8  DacPrimaryVolLRegaddr;
    U8  DacPrimaryVolRRegaddr;
    U8  DacSecondaryVolLRegaddr;
    U8  DacSecondaryVolRRegaddr;
    U8  DacPrimaryMuteLRegaddr;
    U8  DacPrimaryMuteRRegaddr;
    U8  DacSecondaryMuteLRegaddr;
    U8  DacSecondaryMuteRRegaddr;
    U8  DacVolBSizeBPos;
    U8  DacMuteBPos;
    U8  DacVolRegSettingMin;
    U8  DacVolRegSettingMax;
    U8  DacUpdateBPos;
    U8  DacPrimaryUpdateLRegaddr;
    U8  DacPrimaryUpdateRRegaddr;
    U8  DacSecondaryUpdateLRegaddr;
    U8  DacSecondaryUpdateRRegaddr;
    U8  DacMuteProp1;
    U8  DacMuteProp2;
    U8  DacVolRegSettingMute;
    U8  Reserved[2];
};

// Configuration Report ID's

#define RESET_DEVICE          0x40    // Reset Device
#define GETSET_UART_ENABLE    0x41    // Get Set Receive Status
#define GET_UART_STATUS       0x42    // Get UART Status
#define PURGE_FIFOS           0x43    // Purge FIFOs
#define GET_GPIO_VALUES       0x44    // Get GPIO Values
#define SET_GPIO_VALUES       0x45    // Set GPIO Values
#define GET_VER_INFO          0x46    // Get Version Information
#define GETSET_LOCK_BYTE      0x47    // Get Set Lock Byte

#define GETSET_UART_CONFIG    0x50    // Get Set UART Config
#define TRANS_LINE_BREAK      0x51    // Transmit Line Break
#define STOP_LINE_BREAK       0x52    // Stop Line Break

#define GETSET_USB_CONFIG     0x60    // All customizable data that's not strings
#define GETSET_MANU1          0x61    // Get Set Manufacturing String 1
#define GETSET_MANU2          0x62    // Get Set Manufacturing String 2
#define GETSET_PROD1          0x63    // Get Set Product String 1
#define GETSET_PROD2          0x64    // Get Set Product String 2
#define GETSET_SERSTR         0x65    // Get Set Serial String
#define GETSET_PIN_CONFIG     0x66    // GPIO configuration

// 2114 Specific commands                   Parameter                            Description
#define GET_DEVICE_STATUS           0x70 //   none                       1 byte sticky status, cleared in firmware on a read
#define GET_DEVICE_CAPS             0x71 //   none                       Gets CP2114 capabilities
#define GET_RAM_CONFIG              0x72 //   none                       Get current CP2114 Config from RAM
#define SET_RAM_CONFIG              0x73 //   config till DAC pairs      Set CP2114 Configuration to RAM
#define SET_DAC_REGISTERS           0x74 //   register values            Set DAC Config
#define GET_DAC_REGISTERS           0x75 //   register addr/count        Get DAC registers
#define GET_OTP_CONFIG              0x76 //   index                      Return CP2114 OTP Config based on the current index
#define GET_DEVICE_VERSIONS         0x77 //   none                       Return API version and firmware version
#define CREATE_OTP_CONFIG           0x78 //   config block               Create New Configuration Block for CP2114
#define SET_BOOT_CONFIG             0x79 //   index                      Set CP2114 Boot Configuration
#define SET_PARAMS_FOR_NEXT_GET     0x7A //                              Parameters to be used for subsequent Get
#define GET_OTP_ALLCONFIG           0x7B //                              Get OTP all configuration
#define SET_OTP_ALLCONFIG           0x7C //                              Set OTP all configuration

enum 
{
    INBAND_COMMAND_SUSPEND = 0xFA,
    INBAND_COMMAND_ACTIVE = 0xFB,
    INBAND_COMMAND_START = 0xFC,
    INBAND_COMMAND_REENUMERATE = INBAND_COMMAND_START,
    INBAND_COMMAND_DELAY_US = 0xFD,
    INBAND_COMMAND_SET_GPIO = 0xFE,
    INBAND_COMMAND_DELAY_MS = 0xFF,
};

#endif             // _CP2114_COMMON_H_

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
