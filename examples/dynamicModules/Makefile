include ../../Makefile.inc

TARGET = dynamicModules

INCLUDE += \
	-I ./src

SOURCES += $(shell find ./src -type f \( -name '*.cc' ! -name test.cc \) )

OBJS += $(shell find ./src -type f \( -name '*.cc' ! -name test.cc \) | sed 's/.cc/.o/g')

all: $(TARGET)

default: $(TARGET)

-include $(OBJS:.o=.d)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $(TARGET)

%.o: %.cc
	gcc -c $(CFLAGS) $(INCLUDE) $*.cc -o $*.o
	gcc -MM $(CFLAGS) $(INCLUDE) $*.cc > $*.d

clean:
	@rm -f ../../src/*.o ../../src/*.d
	@rm -f ../../src/json/*.o ../../src/json/*.d
	@rm -f ../../src/webserver/*.o ../../src/webserver/*.d
	@rm -f ./src/*.o ./src/*.d
	@rm -rf $(TARGET).dSYM
	@rm -f $(TARGET)

.PHONY: all clean
