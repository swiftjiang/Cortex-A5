#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<time.h>
#include <sys/time.h>
#include <sys/select.h>

#include "include.h"

/*-----------------------structure-------------------------------*/
//spi 设备描述
struct DeviceInfo{
	//gpio引脚映射
	unsigned int num_cov;
	unsigned int num_busy;
	unsigned int num_cs;
	unsigned int num_rest;
	unsigned int num_os0;
	unsigned int num_os1;
	unsigned int num_os2;

	//gpio设备文件描述符
	int fd_cov;
	int fd_busy;
	int fd_cs;
	int fd_rest;
	int fd_os0;
	int fd_os1;
	int fd_os2;


	//spi设备文件
	char *device;
	//spi设备文件描述符
	int fd_spi;

};



int gpio_detail_set(unsigned int gpio,int direc);
/*
初始化gpio：注册gpio-->创建gpio目录-->打开gpio设备-->gpio引脚电平初始化
参数：@*spi spi设备描述
执行成功返回1，失败返回0
*/
int gpio_init(struct DeviceInfo *spi);

/*
初始化spi设备：打开spi设备-->设置spi参数-->spi设备复位
参数：@*spi spi 设备描述
执行成功返回1，失败返回0
*/
int spi_init(struct DeviceInfo *spi);

/*
从spi中读取数据：
	流程描述：
		转换信号：拉低cov-->拉高cov
		获取busy电平：可以忽略
		片选信号：拉低cs
		读取数据
		恢复片选信号：拉高cs
参数：@spi spi 设备描述
	  @buf 数据缓冲区
	  @len 读取长度
执行成功返回1，失败返回0
*/
int spi_read_data(struct DeviceInfo *spi,uint8_t *buf,int len);

/*
接收指定个数的数据
执行成功返回1，失败返回0
*/
int spi_read_numdata(struct DeviceInfo *spi,uint8_t **buf,int num);

/*
关闭设备：
	关闭spi设备-->关闭gpio设备-->注销gpio引脚
参数：@*spi spi设备描述
执行成功返回1，失败返回0
*/
int close_device(struct DeviceInfo *spi);

/*
显示数据到终端
参数：@*buf 数据缓冲
		 @len  数据长度
*/
void show_data(uint8_t *buf,int len);
