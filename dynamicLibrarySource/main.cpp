#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include "SLABCP2110.h"
#include "Profiler.h"

#define VID 0x10c4
#define PID 0xea80

/*
*	测试函数，按照协议像PCI发送  读取数据
*
**************************************************************
*************************协议内容*****************************
*
*通过主机usb接口函数像PCI发送数据，每次发送四个字节序列byte0,byte1，byte2,byte3
*其中：
*	byte0	寄存器地址，取值为1-255,寄存器0为保留寄存器
*	byte1	数据高八位，[15:8]
*	byte2	数据低八为，[ 7:0]
*	byte3	检验和，由byte 0,1,2 三个字节做XOR运算得到
*数据流同步：
	发送值为0的四个字节，实现PCI数据流与主机数据流的同步
*/

void test()
{
    printf("*****begin in function test()*****\n\n");


    DWORD numDevices = 0;
    HID_UART_STATUS status;
    HID_UART_DEVICE device;
    BOOL isOpened;
    BYTE rt[65];//查看寄存器的数据
    BYTE cx[4];//可执行的协议指令
    BYTE tx[64];//测试读取文本
    BYTE rx[65];//测试发送数据
    DWORD written = 0;//实际写入字节数
    DWORD read = 0;//实际读取字节数
    
    for (int i = 0; i < (int)sizeof(tx); i++)
    {
        tx[i] = i;
    }

    cx[0]=51;//写入寄存器51,读取所有寄存器的数据到rt
    cx[1]=0;
    cx[2]=1;
    cx[3]=cx[0] ^ cx[1] ^ cx[2];


    status = HidUart_GetNumDevices(&numDevices, VID, PID);//查询当前连接到主机的cp2110设备的个数
    
    if (status == HID_UART_SUCCESS)
    {
        if (numDevices > 0)
        {
            status = HidUart_Open(&device, 0, VID, PID);//通过设备索引打开一个设备
            
            if (status == HID_UART_SUCCESS)
            {

		//数据流同步
		BYTE syncsqu[4];
		syncsqu[0]=0;
		syncsqu[1]=0;
		syncsqu[2]=0;
		syncsqu[3]=0;

		status=HidUart_Write(device, syncsqu, 4, &written);
		printf("sync status %d\nwrite num %d",status,written);

                HID_UART_DEVICE_STR serial;
                
                status = HidUart_GetOpenedString(device, serial, HID_UART_GET_SERIAL_STR);//获取设备serial描述
                
                if (status == HID_UART_SUCCESS)
                    printf("Serial: %s\n", &serial);
                else
                    printf("Serial: Error!\n");
                
                CProfiler p;
                
                status = HidUart_SetTimeouts(device, 2000, 2000);
                printf("set timeout status %d\n",status);

                printf("Setting read timeouts to 2 secs\nSetting write timeouts to 2 secs");
                
                p.Start();                
                status = HidUart_Write(device, tx, sizeof(tx), &written);
		printf("write tx status %d\n",status);
                p.Stop();
                
                printf("Wrote %u bytes in %f secs\n", written, p.Elapsed());
                
                p.Start();
                status = HidUart_Read(device, rx, sizeof(rx), &read);
		printf("read rx status %d\n",status);
                p.Stop();
                
                printf("Read %u bytes in %f secs\n", read, p.Elapsed());
                for (DWORD i = 0; i < read; i++)
    		{
        	    printf("%02x", rx[i]);

        	    if (i % 16 == 15)
            	    printf("\n");
        	    else if (i < read - 1)
            	    printf(" ");
    		}


                printf("\n");
                



		//读取寄存器的值
		status=HidUart_Write(device, cx, 4, &written);
                printf("write cx status %d\nwrite num %d",status,written);
		
		status = HidUart_Read(device, rt, 65, &read);
		printf("read rt status %d\nread num %d",status,written);
		printf("rt content %s",rt);

                HidUart_IsOpened(device, &isOpened);
                
                HidUart_Close(device);
            }
        }
    }
    printf("******end in function test()******\n\n");
}

int main()
{
	printf("******************test USBtoUART communication****************\n");
	test();
	printf("**************************************************************\n");


	return 0;
}
