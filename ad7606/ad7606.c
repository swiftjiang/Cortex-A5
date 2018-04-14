#include "ad7606.h"


int gpio_detail_set(unsigned int gpio,int direc)
{
	//注册gpio
	if(gpio_export(gpio)<0){
		printf("export gpio %d error\n",gpio);
		return 0;
	}
	//创建gpio目录
	if(gpio_set_dir(gpio,direc)<0){
		printf("set dir gpio %d error\n",gpio);
		return 0;
	}
	return 1;
}
/*
初始化gpio：注册gpio-->创建gpio目录-->打开gpio设备-->gpio引脚电平初始化
参数：@*spi spi设备描述
	  @direc 引脚方向
执行成功返回1，失败返回0
*/
int gpio_init(struct DeviceInfo *spi)
{
	//注册gpio-->创建gpio目录
	if(gpio_detail_set(spi->num_cov,OUT)==0)	return 0;
	if(gpio_detail_set(spi->num_busy,IN)==0)	return 0;
	if(gpio_detail_set(spi->num_cs,OUT)==00)	return 0;
	if(gpio_detail_set(spi->num_rest,OUT)==0) return 0;
	if(gpio_detail_set(spi->num_os0,OUT)==0)	return 0;
	if(gpio_detail_set(spi->num_os1,OUT)==0)	return 0;
	if(gpio_detail_set(spi->num_os2,OUT)==0)	return 0;
	//打开gpio设备
	spi->fd_cov=gpio_fd_open(spi->num_cov);
	if(spi->fd_cov<0){
		printf("open gpio %d error\n",spi->num_cov);
		return 0;
	}
	spi->fd_busy=gpio_fd_open(spi->num_busy);
	if(spi->fd_busy<0){
		printf("open gpio %d error\n",spi->num_busy);
		return 0;
	}
	spi->fd_cs=gpio_fd_open(spi->num_cs);
	if(spi->fd_cs<0){
		printf("open gpio %d error\n",spi->num_cs);
		return 0;
	}
	spi->fd_rest=gpio_fd_open(spi->num_rest);
	if(spi->fd_rest<0){
		printf("open gpio %d error\n",spi->num_rest);
		return 0;
	}
	spi->fd_os0=gpio_fd_open(spi->num_os0);
	if(spi->fd_os0<0){
		printf("open gpio %d error\n",spi->num_os0);
		return 0;
	}
	spi->fd_os1=gpio_fd_open(spi->num_os1);
	if(spi->fd_os1<0){
		printf("open gpio %d error\n",spi->num_os1);
		return 0;
	}
	spi->fd_os2=gpio_fd_open(spi->num_os2);
	if(spi->fd_os2<0){
		printf("open gpio %d error\n",spi->num_os2);
		return 0;
	}
	//gpio引脚电平初始化
	if(gpio_set_value(spi->fd_cov,H)<0){
		printf("init set value gpio %d error\n",spi->num_cov);
		return 0;
	}
	if(gpio_set_value(spi->fd_cs,H)<0){
		printf("init set value gpio %d error\n",spi->num_cs);
		return 0;
	}
	if(gpio_set_value(spi->fd_rest,L)<0){
		printf("init set value gpio %d error\n",spi->num_rest);
		return 0;
	}
	//初始化采样分辨率：
	if(gpio_set_value(spi->fd_os0,L)<0){
		printf("init set value gpio %d error\n",spi->num_os0);
		return 0;
	}
	if(gpio_set_value(spi->fd_os1,L)<0){
		printf("init set value gpio %d error\n",spi->num_os1);
		return 0;
	}
	if(gpio_set_value(spi->fd_os2,L)<0){
		printf("init set value gpio %d error\n",spi->num_os2);
		return 0;
	}
	return 1;
}

/*
初始化spi设备：打开spi设备-->设置spi参数-->spi设备复位
参数：@*spi spi 设备描述
执行成功返回1，失败返回0
*/
int spi_init(struct DeviceInfo *spi)
{
	//打开spi设备
	spi->fd_spi=spi_fd_open(spi->device);
	if(spi->fd_spi<0){
		printf("open spi error\n");
		return 0;
	}
	//设置spi参数
	if(spi_fd_init(spi->fd_spi)<0){
		printf("init spi error\n");
		return 0;
	}
	//spi设备复位
	if(gpio_set_value(spi->fd_rest,H)<0){
		printf("rest spi error\n");
		return 0;
	}
	if(gpio_set_value(spi->fd_rest,L)<0){
		printf("rest spi error\n");
		return 0;
	}
	return 1;
}

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
int spi_read_data(struct DeviceInfo *spi,uint8_t *buf,int len)
{
	//转换信号：拉低cov
	if(gpio_set_value(spi->fd_cov,L)<0){
		printf("begin cov signal set LOW error\n");
		return 0;
	}
	if(gpio_set_value(spi->fd_cov,H)<0){
		printf("begin cov signal set HIGH error\n");
    return 0;
	}

	//片选：拉低cs
	if(gpio_set_value(spi->fd_cs,L)<0){
		printf("select chip signal error\n");
		return 0;
	}
	//读数据
	if(spi_fd_read(spi->fd_spi,buf,len)<0){
		printf("read data error\n");
		return 0;
	}
	//恢复片选信号：拉高cs
	//片选：拉低cs
	if(gpio_set_value(spi->fd_cs,H)<0){
		printf("recover chip signal error\n");
		return 0;
	}
	return 1;
}
/*
接收指定个数的数据
执行成功返回1，失败返回0
*/
int spi_read_numdata(struct DeviceInfo *spi,uint8_t **buf,int num)
{
	int len=16;
	int flag=1;
	int i;
	for(i=0;i<num;i++)
	{
		int status=spi_read_data(spi,buf[i],len);
		if(status<0)flag=0;
	}
	return flag;
}
/*
关闭设备：
	关闭spi设备-->关闭gpio设备-->注销gpio引脚
参数：@*spi spi设备描述
执行成功返回1，失败返回0
*/
int close_device(struct DeviceInfo *spi)
{
	//关闭spi设备
	int flag=1;
	if(spi_fd_close(spi->fd_spi)<0){
		printf("close spi device error\n");
		flag=0;
	}
	//关闭gpio设备
	if(gpio_fd_close(spi->fd_cov)<0){
		printf("close gpio %d error\n",spi->num_cov);
		flag=0;
	}
	if(gpio_fd_close(spi->fd_busy)<0){
		printf("close gpio %d error\n",spi->num_busy);
		flag=0;
	}
	if(gpio_fd_close(spi->fd_cs)<0){
		printf("close gpio %d error\n",spi->num_cs);
		flag=0;
	}
	if(gpio_fd_close(spi->fd_rest)<0){
		printf("close gpio %d error\n",spi->num_rest);
		flag=0;
	}
	if(gpio_fd_close(spi->fd_os0)<0){
		printf("close gpio %d error\n",spi->num_os0);
		flag=0;
	}
	if(gpio_fd_close(spi->fd_os1)<0){
		printf("close gpio %d error\n",spi->num_os1);
		flag=0;
	}
	if(gpio_fd_close(spi->fd_os2)<0){
		printf("close gpio %d error\n",spi->num_os2);
		flag=0;
	}
	//注销gpio引脚
	if(gpio_unexport(spi->num_cov)<0){
		printf("unexport gpio %d error\n",spi->num_cov);
		flag=0;
	}
	if(gpio_unexport(spi->num_busy)<0){
		printf("unexport gpio %d error\n",spi->num_busy);
		flag=0;
	}
	if(gpio_unexport(spi->num_cs)<0){
		printf("unexport gpio %d error\n",spi->num_cs);
		flag=0;
	}
	if(gpio_unexport(spi->num_rest)<0){
		printf("unexport gpio %d error\n",spi->num_rest);
		flag=0;
	}
	if(gpio_unexport(spi->num_os0)<0){
		printf("unexport gpio %d error\n",spi->num_os0);
		flag=0;
	}
	if(gpio_unexport(spi->num_os1)<0){
		printf("unexport gpio %d error\n",spi->num_os1);
		flag=0;
	}
	if(gpio_unexport(spi->num_os2)<0){
		printf("unexport gpio %d error\n",spi->num_os2);
		flag=0;
	}

	return flag;
}

/*
显示数据到终端
参数：@*buf 数据缓冲
		 @len  数据长度
*/
void show_data(uint8_t *buf,int len)
{
	int i;
	int msb,lsb;
	double dec_code;
	printf("*############################################################*\n");
	printf("\nThe calculation formula is: CODE=(VIN/5.0V)*32768.0*(REF/2.5V)\nand the internal reference voltage is REF=2.5V\n\n");
	for(i=0;i<len;i=i+2){
		msb=(int)buf[i];
		lsb=(int)buf[i+1];
		printf("BYTE[%d~%d] HEX MSB LSB CODE is 0x%02x , 0x%02x\n",i,i+1,buf[i],buf[i+1]);
		printf("          DEC MSB LSB CODE is %04d , %04d\n",msb,lsb);
		dec_code=(msb*256+lsb)*1.0/6553.6/2.0;
		printf("--->VIN value is %lf\n",dec_code);
		printf("*-------------------data [%d]------------------------*\n\n",i/2+1);
	}
	return;
}
