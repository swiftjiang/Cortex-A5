
CC = arm-linux-gnueabihf-gcc
OBJS = testDAC.o ad5696r.o i2c.o
EXEC = testDAC
all:$(EXEC)
$(EXEC):$(OBJS)
	$(CC) -o $@ $(OBJS)
ad7606.o:ad5696r.c ad5696r.h
i2c.o:i2c.c i2c.h
clean:
	rm *.o
