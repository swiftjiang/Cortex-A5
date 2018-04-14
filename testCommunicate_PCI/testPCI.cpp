#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include "OperateCP2110.h"

#include <iostream>
using namespace std;

void printMenu()
{
	printf("************************************\n");
	printf("*******menu for operating pci*******\n");
	printf("#1#  write data to register 1-31   *\n");
        printf("#2#  sava parameter to flash       *\n");
        printf("#3#  restore parameter from flash  *\n");
        printf("#4#  read register 0-31 and display*\n");
        printf("#5#  restore parameter default     *\n");
	printf("#6#  set ramp table                *\n");
	printf("#7#  restore ramp table from flash *\n");
        printf("#0#  exit                          *\n");
	printf("**************menu end**************\n");
	printf("************************************\n");

}

int main()
{	//int x;
	//cin>>x;
	//cout<<x<<endl;
	//printf("==========test menu==========\n");
	//printMenu();
	//printf("==========test menu==========\n");
	OperateCP2110 ex;//接口操作实例化对象
	int op=-1;
	int regAddress=-1;
	int regValue=0;
	BYTE *regData=new BYTE[70];

	int page=0;


	//打开设备
	if(!ex.OpenDevice())
	  {
		printf("open device failed !\n");
		return 0;
	  }
	//同步主机与设备字节流
	if(!ex.SyncDataStream())
	  {
		printf("sync data stream failed !\n");
		return 0;
	  }
	while(1)
	{
		printMenu();
		printf("please input a number:");
		fflush(stdin);
		cin>>op;//scanf("%d",&op);
		getchar();

		switch(op)
		{
		case 1:
			cout<<"@@@input register address and value:"<<endl;


			cout<<"@@@register address:";
			cin>>regAddress;
			//cout<<"@@@register address:"<<regAddress<<endl;
			while(regAddress<1 || regAddress>52)
			{
				cout<<"@@@register address error !"<<endl;
                               	cout<<"@@@input register address again:"<<endl;
                               	cout<<"@@@register address:";
				cin>>regAddress;
			}

			cout<<"@@@register value  :";
                        cin>>regValue;
			//cout<<"@@@register value  :"<<regValue<<endl;
			if(!ex.Write1_31Register(regAddress,regValue))
				printf("@@@write failed !\n");
			else
				printf("@@@write successfully !\n");
			break;
                case 2:
			if(!ex.Sava2Flash())
				printf("@@@save parameter to flash failed !\n");
			else
				printf("@@@save parameter to flash successfully !\n");
			break;
                case 3:
			if(!ex.RestoreRegsitersFromFlash())
                                printf("@@@restore parameter from flash failed !\n");
                        else
                                printf("@@@restore parameter from flash successfully !\n");
                        break;
                case 4:
			if(!ex.ReadRegisters(regData))
				printf("@@@read register failed !\n");
			else
			{
				printf("@@@register 0-31 data\n\n\n");
				for(int i=0;i<7;i++)
				{
					for(int j=0;j<10;j=j+2)
					{
						if( (i*10+j)/2 > 27 )continue;
						if( (i*10+j)/2 == 6 || (i*10+j)/2 == 9 || (i*10+j)/2 == 10 || (i*10+j)/2 == 15 || (i*10+j)/2 == 19 || (i*10+j)/2 == 25)continue;
						printf("@register[%02d]=%02x%02x  ",(i*10+j)/2,regData[i*10+j],regData[i*10+j+1]);
					}
					printf("\n");
				}
				
				printf("\n\n\n");
			}
			break;
                case 5:
			if(!ex.RestoreDefaults())
				printf("@@@restore default failed !\n");
			else
			{
				printf("@@@restore default successfully !\n");
				if(!ex.SyncDataStream())
					printf("@@@after restore default sync failed !\n");
			}
			break;
		case 6:
			if(!ex.ReadRegisters(regData))
				cout<<"read register failed !"<<endl;
			else
			{
				int reg11=(int)regData[22]*256+(int)regData[23];//(int)( (regData[22]<<8) & 0xFF00 | (regData[23] & 0xFF ) );
				int reg12=(int)regData[24]*256+(int)regData[25];//(int)( (regData[24]<<8) & 0xFF00 | (regData[25] & 0xFF ) );
				int reg13=(int)regData[26]*256+(int)regData[27];//(int)( (regData[26]<<8) & 0xFF00 | (regData[27] & 0xFF ) );
				//int reg14=(int)regData[28]*256+(int)regData[29];//(int)( (regData[28]<<8) & 0xFF00 | (regData[29] & 0xFF ) );
				//cout<<reg11<<"  "<<reg12;
				for(int i=0;i<reg11;i++)
				{
					if(!ex.WriteRampTable(0))
					{
						cout<<"write reg11 to ramp failed !"<<endl;
						break;
					}
				}
				for(int i=reg11;i<2048;i++)
				{
					if(!ex.WriteRampTable(  (int)(reg12+reg13*((i-reg11)*1.0/(2048-reg11)))*(65535*1.0/10000)  ))
					{
						cout<<"write reg~ to ramp failed !"<<endl;
						break;
					}
				}
				cout<<"write ramp successfully !"<<endl;
			}
			break;
			case 7:
				if(!ex.RestoreTableFromFlash())
					cout<<"restore table from flash failed !"<<endl;
				else
					cout<<"successfully !"<<endl;
				break;
                case 0:
			if(!ex.CloseDevice())
                		printf("@@@close device failed !\n");
			else
				printf("@@@close devise successfully !\n");
			printf("------------------------------------------------------------------------------------------------------------------\n");
			printf("------------------------------------------------------------------------------------------------------------------\n");
			cout<<"--------------------------------------------------------"<<page++<<"---------------------------------------------------------"<<endl;
			printf("------------------------------------------------------------------------------------------------------------------\n");
			printf("------------------------------------------------------------------------------------------------------------------\n");
			exit(0);
		default:
			printf("@@@input error !\n");
			break;
		}
		printf("------------------------------------------------------------------------------------------------------------------\n");
		printf("------------------------------------------------------------------------------------------------------------------\n");
		cout<<"--------------------------------------------------------"<<page++<<"---------------------------------------------------------"<<endl;
		printf("------------------------------------------------------------------------------------------------------------------\n");
		printf("------------------------------------------------------------------------------------------------------------------\n");
	}
	return 0;
}
