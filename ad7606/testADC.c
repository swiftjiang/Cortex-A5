#include "ad7606.h"

//spi0设备
static struct DeviceInfo spi0={
	//gpio引脚映射
	.num_cov    =58,
	.num_busy   =146,
	.num_cs     =112,
	.num_rest   =147,
	.num_os0    =59,
	.num_os1    =60,
	.num_os2    =61,

	//gpio设备文件描述符
	.fd_cov     =-1,
	.fd_busy    =-1,
	.fd_cs      =-1,
	.fd_rest    =-1,
	.fd_os0     =-1,
	.fd_os1     =-1,
	.fd_os2     =-1,
	//spi设备文件
	.device     ="/dev/spidev32766.1",
	//spi设备文件描述符
	.fd_spi     =-1,
};
//spi1设备
static struct DeviceInfo spi1={
	//gpio引脚映射
	.num_cov    =153,
	.num_busy   =76,
	.num_cs     =92,
	.num_rest   =75,
	.num_os0    =114,
	.num_os1    =113,
	.num_os2    =154,

	//gpio设备文件描述符
	.fd_cov     =-1,
	.fd_busy    =-1,
	.fd_cs      =-1,
	.fd_rest    =-1,
	.fd_os0     =-1,
	.fd_os1     =-1,
	.fd_os2     =-1,
	//spi设备文件
	.device     ="/dev/spidev32765.0",
	//spi设备文件描述符
	.fd_spi     =-1,
};

/*
spi 设备测试主函数
*/

int main()
{
	uint8_t buf[MAX_BUF];
	uint8_t **maxbuf;
	int num=1024;
	int len=16;
	char flag;
	//计算时间
	clock_t start,end;
	//动态分配二维数组
  int i, j;
  maxbuf=(uint8_t**)malloc(sizeof(uint8_t*)*num);//为二维数组分配1024行
  for (i=0;i<num;i++)
	{//为每列分配16个大小空间
      maxbuf[i]=(uint8_t*)malloc(sizeof(uint8_t)*len);
  }



	gpio_init(&spi0);
	spi_init(&spi0);

  /*----------读取指定数目的数据并存入文件------*/
	//按指定时间间隔读取数据
	struct timeval begin,finish,timer={.tv_sec=0,.tv_usec=200,};
	// gettimeofday(&begin,NULL);
	// for(i=0;i<num;i++)
	// {
	// 	//select(0,NULL,NULL,NULL,&timer);
	// 	spi_read_data(&spi0,maxbuf[i],len);
	// }
	// gettimeofday(&finish,NULL);

	//读取指定数目的数据
	gettimeofday(&begin,NULL);
	start=clock();
	spi_read_numdata(&spi0,maxbuf,num);
	end=clock();
	gettimeofday(&finish,NULL);

	printf("\n\n#####maxbuf[0]#####\n");
	show_data(maxbuf[0],len-6);
	printf("\n\n#####maxbuf[1023]#####\n");
	show_data(maxbuf[1023],len-6);

	printf("\n\ngettimeofday() use time = %lf s\n\n",(double)(end-start)/CLOCKS_PER_SEC);
	printf("\n\nclock() use time = %lf s\n\n",(double)(finish.tv_usec-begin.tv_usec)/CLOCKS_PER_SEC/CLOCKS_PER_SEC);
	close_device(&spi0);

	//将读取的数据写入文件
	FILE *datafile;
	double data[num][8],temp[8];


	datafile=fopen("adcdata.txt","w+");
	if((datafile)==NULL)
	{
    printf("open data file error\n");
	}
	else
	{
		printf("open data file successfully\n");
		for(i=0;i<num;i++)
		{
			for(j=0;j<16;j=j+2)
			{
				data[i][j/2]=( ((int)maxbuf[i][j])*256+((int)maxbuf[i][j+1]) )*1.0/6553.6/2.0;
				//if(i==1)printf("(msb,lsb) = hex(0x%02x,0x%02x),dec(%d,%d),value=%lf\n",maxbuf[i][j],maxbuf[i][j+1],(int)maxbuf[i][j],(int)maxbuf[i][j+1],data[i][j/2]);
				fprintf(datafile,"%lf	",data[i][j/2]);
			}
			fprintf(datafile,"\n");
		}
		printf("write file over,and data save in file \"adcdata.txt\"\n");

		fclose(datafile);
	}

	/*----------spi0测试------------*/
  /*
	gpio_init(&spi0);
	spi_init(&spi0);
  scanf("%c",&flag);
	while(1)
  {
		if(flag=='0')	break;
		spi_read_data(&spi0,buf,len);
		show_data(buf,len);
		scanf("%c",&flag);
	}
	//show_data(buf,len);
	close_device(&spi0);
  */

  /*-----------spi1测试------------------*/
  /*
	gpio_init(&spi1);
  spi_init(&spi1);
  scanf("%c",&flag);
  while(1)
  {
    if(flag=='0')   break;
    spi_read_data(&spi1,buf,len);
    show_data(buf,len);
    scanf("%c",&flag);
  }
  //show_data(buf,len);
  close_device(&spi1);
  */

  //释放数据缓冲区
  for (i=0;i<num;i++)
  {
    free(maxbuf[i]);
  }
  free(maxbuf);

	return 0;
}
