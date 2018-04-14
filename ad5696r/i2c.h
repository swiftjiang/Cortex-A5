#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>


/*
关闭i2c设备文件
成功返回一个int型设备描述符
失败返回负值
参数：@*device 指向设备文件的绝对路径字符串
*/
int i2c_fd_open(char *device);

/*
关闭i2c设备文件
成功返回0
失败返回负值
参数：@fd 设备描述符
*/
int i2c_fd_close(int fd);

/*
指定从机地址
成功返回0
失败返回负值
参数：@device 设备结构体
*/
int i2c_slave_addr(int fd,uint8_t slave_addr);



/*
i2c设备写命令
成功返回0
失败返回负值
参数：@fd 设备文件描述符
     @*buf 数据缓冲区
     @len 待写入数据长度
*/
int i2c_fd_write(int fd,uint8_t addr,uint8_t *buf,int len);
