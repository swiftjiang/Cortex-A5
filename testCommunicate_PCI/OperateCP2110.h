

#include "CP2114_Common.h"
#include "SLABCP2110.h"
#include "SLABCP2114.h"
#include "SLABHIDtoUART.h"
#include "Types.h"



#define VID 0x10c4
#define PID 0xea80

class OperateCP2110
{
private:
	
	DWORD numDevices;//设备个数
    HID_UART_STATUS status;//函数执行后返回的状态值
    HID_UART_DEVICE device;//设备实例
    BOOL isOpened;//设备的开合状态
    BYTE registerData[65];//查看寄存器的数据,前64个字节为0-31号寄存器的当前数据，每两个字节对应一个寄存器，第65个字节为校验和，有前64字节做异或得到
    BYTE command[4];//可执行的协议指令
    DWORD numByte;//期望读、写的字节个数
    DWORD written;//实际写入字节数
    DWORD read;//实际读取字节数
	
public:
	OperateCP2110();//构造函数
	~OperateCP2110();//析构函数
	
	BOOL OpenDevice();//打开设备
	BOOL CloseDevice();//关闭设备
	
	//0-31号寄存器  6、9、10、15、19、25、29、30、31 unused
	BOOL SyncDataStream();//同步主机到PCI字节流
	BOOL Write1_31Register(int reg,int data);//写数据到0-31号寄存器

	
	//32-52号寄存器 只写  34-47unused
	BOOL SetRampPointer();//设置Ramp指针
	BOOL WriteRampTable(int value);//写入数据到Ramp表
	BOOL Sava2Flash();//保存参数到flash
	BOOL RestoreRegsitersFromFlash();//从flash恢复参数
	BOOL RestoreTableFromFlash();//从flash恢复表
	BOOL ReadRegisters(BYTE* &registerDataArray);//读取寄存器到registerDataArray数组
	BOOL RestoreDefaults();//恢复参数为默认值
	
	
};



