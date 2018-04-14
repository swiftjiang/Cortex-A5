#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>

#include "i2c.h"

//命令字节
#define NOP				0x00
#define WR_CH_N			0x10
#define UP_CH_N			0x20
#define WU_CH_N			0x30
#define POWER_DU		0x40
#define B_LDAC			0x50
#define SW_RESET		0x60
#define INTER_REF		0x70
//通道选择
#define DAC_A			0x01
#define DAC_B			0x02
#define DAC_C			0x04
#define DAC_D			0x08
//地址
#define S_ADDR			0x0c
//定义设备结构体
struct i2c_device{
  int fd;       //设备文件描述符
  uint8_t addr; //从机地址
  char *device; //i2c设备文件绝对路径
};

/*
初始化i2c设备
成功返回0
失败返回负值
参数：@*i2c i2c设备结构体
*/
int i2c_init(struct i2c_device *i2c);

/*
i2c设备软件复位
成功返回0
失败返回负值
参数：@i2c 设备结构体
*/
int i2c_software_reset(struct i2c_device *i2c);

/*
i2c设备写操作
成功返回0
失败返回负值
参数：@i2c i2c设备结构体
     @value 待写入数据
     @chanel 通道选择
*/
int i2c_write_data(struct i2c_device *i2c,double value,int chanel);
