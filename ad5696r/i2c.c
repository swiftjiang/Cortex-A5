#include "i2c.h"



/*
关闭i2c设备文件
成功返回一个int型设备描述符
失败返回负值
参数：@*device 指向设备文件的绝对路径字符串
*/
int i2c_fd_open(char *device)
{
  int fd=open(device, O_RDWR | O_NONBLOCK);
	if(fd<0)
	{
		perror("open i2c device error");
		return fd;
	}
	return fd;
}

/*
关闭i2c设备文件
成功返回0
失败返回负值
参数：@fd 设备描述符
*/
int i2c_fd_close(int fd)
{
  return close(fd);
}

/*
指定从机地址
成功返回0
失败返回负值
参数：@device 设备结构体
*/
int i2c_slave_addr(int fd,uint8_t slave_addr)
{
  int status=ioctl(fd,I2C_SLAVE,slave_addr);
	if(status<0)
	{
		perror("set slave addr error");
		return status;
	}
	return 0;
}

/*
i2c设备写命令
成功返回0
失败返回负值
参数：@fd 设备文件描述符
     @*buf 数据缓冲区
     @len 待写入数据长度
*/
int i2c_fd_write(int fd,uint8_t addr,uint8_t *buf,int len)
{
  struct i2c_msg msg={
		.addr=addr,
		.flags=!I2C_M_RD,//write flags
		.len=len,//buffer length
		.buf=buf,//pointer to buffer
	};
	struct i2c_rdwr_ioctl_data msgset={
		.msgs=&msg,
		.nmsgs=1,
	};

	int status=ioctl(fd,I2C_RDWR,&msgset);
	if(status<0)
	{
		perror("write data error");
		return status;
	}

	return 0;
}
