CC = arm-linux-gnueabihf-gcc
OBJS = testADC.o ad7606.o gpio.o spi.o
EXEC = testADC
all:$(EXEC)
$(EXEC):$(OBJS)
	$(CC) -o $@ $(OBJS)
ad7606.o:ad7606.c ad7606.h include.h
gpio.o:gpio.c include.h
spi.o:spi.c include.h
clean:
	rm *.o
