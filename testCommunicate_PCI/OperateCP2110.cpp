#include "OperateCP2110.h"

OperateCP2110::OperateCP2110()//构造函数
{
	numDevices=0;//设备个数
    status=0;//函数执行后返回的状态值
    //device;//设备实例
    isOpened=0;//设备的开合状态
    registerData[0]='\0';//查看寄存器的数据,前64个字节为0-31号寄存器的当前数据，每两个字节对应一个寄存器，第65个字节为校验和，有前64字节做异或得到
    //command[0]=;//可执行的协议指令
    numByte=0;//期望读、写的字节个数
    written=0;//实际写入字节数
    read=0;//实际读取字节数
}
OperateCP2110::~OperateCP2110()//析构函数
{
		;
}

BOOL OperateCP2110::OpenDevice()//打开设备
{
	status = HidUart_GetNumDevices(&numDevices, VID, PID);//查询当前连接到主机的cp2110设备的个数

    if (status == HID_UART_SUCCESS && numDevices>0)
    {
            status = HidUart_Open(&device, 0, VID, PID);//通过设备索引打开一个设备
			if(status != HID_UART_SUCCESS)
				return 0;
			else return 1;
	}
	else return 0;
}
BOOL OperateCP2110::CloseDevice()//关闭设备
{
	HidUart_IsOpened(device, &isOpened);
	if(isOpened)
	{
		if(HidUart_Close(device) == HID_UART_SUCCESS)
			return 1;
		else
			return 0;
	}
	else
		return 1;
}

//0-31号寄存器  6、9、10、15、19、25、29、30、31 unused
BOOL OperateCP2110::SyncDataStream()//同步主机到PCI字节流
{
	BYTE syncData[4];
	syncData[0]=syncData[1]=syncData[2]=syncData[3]=0;

	status=HidUart_Write(device, syncData, 4, &written);
	if(status == HID_UART_SUCCESS && written == 4)
	{
		written = 0;
		return 1;
	}
	else
	{
		written = 0;
		return 0;
	}
}
BOOL OperateCP2110::Write1_31Register(int reg,int data)//写数据到0-31号寄存器
{
	command[0] = reg;
	if(data > 255)
	{
		command[1] = data / 256;
		command[2] = data % 256;
	}
	else
	{
		command[1] = 0;
		command[2] = data;
	}
	command[3] = command[0] ^ command[1] ^ command[2];

	status=HidUart_Write(device, command, 4, &written);
	if(status == HID_UART_SUCCESS && written == 4)
	{
		written = 0;
		return 1;
	}
	else
	{
		written = 0;
		return 0;
	}
}


//32-52号寄存器 只写  34-47unused
BOOL OperateCP2110::SetRampPointer()//设置Ramp指针
{
	command[0] = 32;
	command[1] = 255;
	command[2] = 255;
	command[3] = command[0] ^ command[1] ^ command[2];

	status=HidUart_Write(device, command, 4, &written);
	if(status == HID_UART_SUCCESS && written == 4)
	{
		written = 0;
		return 1;
	}
	else
	{
		written = 0;
		return 0;
	}
}
BOOL OperateCP2110::WriteRampTable(int value)//写入数据到Ramp表
{

	command[0] = 33;
	if(value > 255)
	{
		command[1] = value / 256;
		command[2] = value % 256;
	}
	else
	{
		command[1] = 0;
		command[2] = value;
	}
	command[3] = command[0] ^ command[1] ^ command[2];

	status=HidUart_Write(device, command, 4, &written);
	if(status == HID_UART_SUCCESS && written == 4)
	{
		written = 0;
		return 1;
	}
	else
	{
		written = 0;
		return 0;
	}
}
BOOL OperateCP2110::Sava2Flash()//保存参数到flash
{
	command[0] = 48;
	command[1] = 255;
	command[2] = 255;
	command[3] = command[0] ^ command[1] ^ command[2];

	status=HidUart_Write(device, command, 4, &written);
	if(status == HID_UART_SUCCESS && written == 4)
	{
		written = 0;
		return 1;
	}
	else
	{
		written = 0;
		return 0;
	}
}
BOOL OperateCP2110::RestoreRegsitersFromFlash()//从flash恢复参数
{
	command[0] = 49;
	command[1] = 255;
	command[2] = 255;
	command[3] = command[0] ^ command[1] ^ command[2];

	status=HidUart_Write(device, command, 4, &written);
	if(status == HID_UART_SUCCESS && written == 4)
	{
		written = 0;
		return 1;
	}
	else
	{
		written = 0;
		return 0;
	}
}
BOOL OperateCP2110::RestoreTableFromFlash()//从flash恢复表
{
	command[0] = 50;
	command[1] = 255;
	command[2] = 255;
	command[3] = command[0] ^ command[1] ^ command[2];

	status=HidUart_Write(device, command, 4, &written);
	if(status == HID_UART_SUCCESS && written == 4)
	{
		written = 0;
		return 1;
	}
	else
	{
		written = 0;
		return 0;
	}
}
BOOL OperateCP2110::ReadRegisters(BYTE* &registerDataArray)//读取寄存器到registerDataArray数组
{
	command[0] = 51;
	command[1] = 255;
	command[2] = 255;
	command[3] = command[0] ^ command[1] ^ command[2];

	status=HidUart_Write(device, command, 4, &written);
	if(status != HID_UART_SUCCESS || written != 4)
	{
		written = 0;
		return 0;
	}

	status=HidUart_Read(device, registerData, 65, &read);
	if(status == HID_UART_SUCCESS && read == 65)
	{
		read = 0;
		for(int i=0;i<65;i++)
		{
			registerDataArray[i]=registerData[i];
		}
		return 1;
	}
	else
	{
		read = 0;
		return 0;
	}
}
BOOL OperateCP2110::RestoreDefaults()//恢复参数为默认值
{
	command[0] = 52;
	command[1] = 255;
	command[2] = 255;
	command[3] = command[0] ^ command[1] ^ command[2];

	status=HidUart_Write(device, command, 4, &written);
	if(status == HID_UART_SUCCESS && written == 4)
	{
		written = 0;
		return 1;
	}
	else
	{
		written = 0;
		return 0;
	}
}
