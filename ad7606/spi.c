#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
//#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "include.h"


static uint8_t SPI_MODE = 1;//spi 通信模式
static uint8_t SPI_BITS = 8;//8读写位，MSB first
static uint32_t SPI_SPEED = 50*1000*1000;//设置传输速度

/*
static const char *device = "/dev/spidev0.0";
*/

/*
打开一个spi设备
成功返回一个int型设备描述符
失败返回一个负值
参数：@*device 指向设备文件的绝对路径字符串
*/
int spi_fd_open(char *device)
{
	int fd;

    fd = open(device, O_RDWR | O_NONBLOCK );
    if (fd < 0) {
        perror("spi/fd_open");
    }
    return fd;
}

/*
设置spi设备工作模式及速率等参数
成功返回0
失败返回一个负值
参数：@fd 设备文件描述符
*/
int spi_fd_init(int fd)
{
	int ret=-1;
	//spi mode
	ret = ioctl(fd, SPI_IOC_WR_MODE, &SPI_MODE);
	if (ret<0){
		perror("can't set spi mode:");
		return ret;
	}

	//bits per word
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &SPI_BITS);
	if (ret<0){
		perror("can't set bits per word:");
		return ret;
	}

	//max speed hz
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &SPI_SPEED);
	if (ret<0){
		perror("can't set max speed hz:");
		return ret;
	}

	return 0;
}

/*
接收数据
成功返回接收数据的字节数
失败返回一个负值
参数：@fd 设备文件描述符
	  @*buf 数据接收缓冲区，无符号字符
	  @len 想要读取的数据长度
*/
int spi_fd_read(int fd,uint8_t *buf,int len)
{
	int ret=-1;

	ret = read(fd, buf, len);

	if (ret < 0)
	{
		perror("SPI read error:");
		return ret;
	}

	return 0;
}

/*
关闭spi设备
成功返回0
失败返回一个负值
参数：@fd 设备文件描述符
*/
int spi_fd_close(int fd)
{
	return close(fd);
}
