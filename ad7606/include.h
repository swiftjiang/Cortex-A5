#include<sys/types.h>
#include<stdint.h>
/*-----------------------gpio.c-------------------------------*/
#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 1024
#define OUT     1
#define IN      0
#define H       1
#define L       0
/*
注册gpio设备
注册成功返回0
否则返回负值
参数：@gpio gpio号
*/
int gpio_export(unsigned int gpio);

/*
注销gpio设备
成功返回0
失败返回负值
参数：@gpio gpio号
 */
int gpio_unexport(unsigned int gpio);

/*
创建gpio引脚目录
成功返回0
失败返回负值
参数：@gpio gpio号
	  @out_flag  输入或输出，取值为1（输出）或0（输入）
 */
int gpio_set_dir(unsigned int gpio ,int out_flag);

/*
设置gpio引脚上的电平
成功返回0
参数：@fd 设备文件描述符
	  @gpio gpio号
	  @value 输出电平，取值0或1
 */
int gpio_set_value(int fd, unsigned int value);

/*
获取gpio引脚电平的值
成功返回0
参数：@fd 设备文件描述符
	  @gpio 引脚号
	  @value int型指针，指向保存获取的引脚值的变量
 */
int gpio_get_value(int fd, unsigned int gpio, unsigned int *value);

/*
设置引脚触发方式
成功返回0
 */
int gpio_set_edge(unsigned int gpio ,char *edge);

/*
打开一个已经注册成功的gpio设备
成功返回一个int型设备描述符
失败返回负值
参数：@gpio gpio号
 */
int gpio_fd_open(unsigned int gpio);

/*
关闭设备描述符指向的设备文件
成功返回0
失败返回负值
参数：@fd 已经打开的设备文件描述符
 */
int gpio_fd_close(int fd);


/*-----------------------spi.c-------------------------------*/

/*
打开一个spi设备
成功返回一个int型设备描述符
失败返回一个负值
参数：@*device 指向设备文件的绝对路径字符串
*/
int spi_fd_open(char *device);

/*
设置spi设备工作模式及速率等参数
成功返回0
失败返回一个负值
参数：@fd 设备文件描述符
*/
int spi_fd_init(int fd);

/*
接收数据
成功返回接收数据的字节数
失败返回一个负值
参数：@fd 设备文件描述符
	  @*buf 数据接收缓冲区，无符号字符
	  @len 想要读取的数据长度
*/
int spi_fd_read(int fd,uint8_t *buf,int len);


/*
关闭spi设备
成功返回0
失败返回一个负值
参数：@fd 设备文件描述符
*/
int spi_fd_close(int fd);
