

#include "ad5696r.h"

int main()
{
  struct i2c_device i2c={
    .fd=-1,
    .addr=S_ADDR,
    .device="/dev/i2c-1",
  };

  if(i2c_init(&i2c)<0) return 0;
  else printf("init i2c devicce successfully!\n");
  if(i2c_software_reset(&i2c)<0) return 0;
  else printf("reset i2c device successfully!\n");

  while(1)
  {
    printf("please input value and chanel:\n");
    double value;
    int chanel;
    scanf("%lf,%d",&value,&chanel);

    if(value >5.0 || value <0)break;

    i2c_write_data(&i2c,value,chanel);
  }
  return 0;
}
