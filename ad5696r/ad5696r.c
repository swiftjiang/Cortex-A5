

#include "ad5696r.h"


/*
初始化i2c设备
成功返回0
失败返回负值
参数：@*i2c i2c设备结构体
*/
int i2c_init(struct i2c_device *i2c)
{
  int ret=i2c_fd_open(i2c->device);
  if(ret<0)return ret;
  else i2c->fd=ret;

  ret=i2c_slave_addr(i2c->fd,i2c->addr);
  if(ret<0)return ret;

  return 0;
}

/*
i2c设备软件复位
成功返回0
失败返回负值
参数：@i2c 设备结构体
*/
int i2c_software_reset(struct i2c_device *i2c)
{
  uint8_t reset_command=SW_RESET;
	struct i2c_msg reset_msg={
		.addr=i2c->addr,
		.flags=!I2C_M_RD,//write flags
		.len=1,//buffer length
		.buf=&reset_command,//pointer to buffer
	};
	struct i2c_rdwr_ioctl_data msgset={
		.msgs=&reset_msg,
		.nmsgs=1,
	};
	int reset=ioctl(i2c->fd,I2C_RDWR,&msgset);
	if(reset<0)return reset;

	return 0;
}

/*
i2c设备写操作
成功返回0
失败返回负值
参数：@i2c i2c设备结构体
     @value 待写入数据
     @chanel 通道选择
*/
int i2c_write_data(struct i2c_device *i2c,double value,int chanel)
{
  	uint8_t cha;//通道选择
	uint8_t data[3];//待写入数据

	if(chanel==1)cha=DAC_A;
	if(chanel==2)cha=DAC_B;
	if(chanel==3)cha=DAC_C;
	if(chanel==4)cha=DAC_D;
	data[0]=WU_CH_N | cha;//写入并更新通道

	//设置value的16位编码，D=( (65535/(GAIN*Vref) )*value),Vref=2.5 V
	//字节流对应的数组下标位置
	//int temp=(int)(65535.0*value/5.0);
	printf("accurate value is : %lf\n",value*13107.0);
	int temp=(int)(value*13107.0);
	if(temp>255)
	{
		data[1]=temp/256;
		data[2]=temp%256;
	}
	else
	{
		data[1]=0;
		data[2]=temp;
	}
	printf("temp = %d , \n [1]msb = 0x%02x , [2]lsb = 0x%02x , [0]command = 0x%02x\n",temp,data[1],data[2],data[0]);

  int status=i2c_slave_addr(i2c->fd,i2c->addr);
  if(status<0)return status;

	return 0;
}
