#include<stdio.h>
#include<stdlib.h>
//#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include <poll.h>

#include "include.h"



/*
注册gpio设备
注册成功返回0
否则返回负值
参数：@gpio gpio号
*/
int gpio_export(unsigned int gpio)
{
	int fd ,len;
	char buf[MAX_BUF];

	fd = open(SYSFS_GPIO_DIR "/export" ,O_WRONLY);
	if (fd < 0) {
		perror("gpio/export");
		return fd;
	}
	len = snprintf(buf ,sizeof(buf) ,"%d" ,gpio);
	write(fd ,buf ,len);
	close(fd);
	return 0;
}

/*
注销gpio设备
成功返回0
失败返回负值
参数：@gpio gpio号
 */   
int gpio_unexport(unsigned int gpio)
{
	int fd ,len;
	char buf[MAX_BUF];

	fd = open(SYSFS_GPIO_DIR "/unexport" ,O_WRONLY);
	if (fd < 0) {
		perror("gpio/unexport");
		return fd;
	}
	len = snprintf(buf ,sizeof(buf) ,"%d" ,gpio);
	write(fd ,buf ,len);
	close(fd);
	return 0;
}

/*
创建gpio引脚目录
成功返回0
失败返回负值
参数：@gpio gpio号
	  @out_flag  输入或输出，取值为1（输出）或0（输入）
 */ 
int gpio_set_dir(unsigned int gpio ,int out_flag)
{
	int fd ,len;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/direction", gpio);
	fd = open(buf ,O_WRONLY);
	if (fd < 0) {
		perror(buf);
		return fd;
	}
	if (out_flag) 
		write(fd ,"out" ,4);
	else 
		write(fd ,"in" ,3);
	close(fd);
	return 0;
}

/*
设置gpio引脚上的电平
成功返回0
参数：@fd 设备文件描述符
	  @gpio gpio号
	  @value 输出电平，取值0或1
 */   
int gpio_set_value(int fd, unsigned int value)  
{  
/*
    int fd, len;  
    char buf[MAX_BUF];  
   
    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);      
    fd = open(buf, O_WRONLY);
	if (fd < 0) {  
        perror("gpio/set-value");  
        return fd;  
    }  
*/   
	int ret=-1;
	
    if (value)  
        ret=write(fd, "1", 2);  
    else  
        ret=write(fd, "0", 2);  
   
    //close(fd);  
    return ret;  
}

/*
获取gpio引脚电平的值
成功返回0
参数：@fd 设备文件描述符
	  @gpio 引脚号
	  @value int型指针，指向保存获取的引脚值的变量
 */
int gpio_get_value(int fd, unsigned int gpio, unsigned int *value)
{
/*	
	int fd, len;
	char ch;
	char buf[MAX_BUF];

	len = snprintf(buf ,sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value" ,gpio); 
	fd = open(buf ,O_RDONLY);
	if (fd < 0) {
		perror("gpio_get_value");
		return fd;
	}
	*/
	int ret=-1;
	char buf[MAX_BUF];
	char ch;
	ret=read(fd ,&ch ,1);
	if (ch == '1') 
		*value = 1;
	else if(ch == '0') 
			*value = 0;
	//close(fd);
	return ret;
}

/*
设置引脚触发方式
成功返回0
 */
int gpio_set_edge(unsigned int gpio ,char *edge)
{
	int fd ,len;
	char buf[MAX_BUF];

	len = snprintf(buf ,sizeof(buf) ,SYSFS_GPIO_DIR "/gpio%d/edge" ,gpio);
	fd = open(buf ,O_WRONLY);
	if (fd < 0) {
		perror("gpio_set_edge");
		return fd;
	}
	write(fd ,edge ,strlen(edge) + 1);
	close(fd);
	return 0;
}

/*
打开一个已经注册成功的gpio设备
成功返回一个int型设备描述符
失败返回负值
参数：@gpio gpio号
 */   
int gpio_fd_open(unsigned int gpio)  
{  
    int fd, len;  
    char buf[MAX_BUF];  
  
    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);  
   
    fd = open(buf, O_RDWR | O_NONBLOCK );  
    if (fd < 0) {  
        perror("gpio/fd_open");  
    }  
    return fd;  
}  
  
/*
关闭设备描述符指向的设备文件
成功返回0
失败返回负值
参数：@fd 已经打开的设备文件描述符
 */
int gpio_fd_close(int fd)  
{  
    return close(fd);  
}  
