

all: testPCI


CXX       = g++

CXXFLAGS  = -pthread -Wall -g

CPPOBJS   = testPCI.o OperateCP2110.o

OBJS      = $(CPPOBJS)
LIBS      = `pkg-config libusb-1.0 --libs` -lm -lrt -lslabhidtouart
INCLUDES  = -I./ -I./include `pkg-config libusb-1.0 --cflags`

testPCI: $(OBJS)
	$(CXX) $(CXXFLAGS) $() -o testPCI $^ $(LIBS)

$(CPPOBJS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $(INCLUDES) $< -o $@

clean:
	rm -f $(OBJS)

.PHONY: clean
