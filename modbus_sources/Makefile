CC = gcc
CFLAGS = -Wall -g
LIBS = -lmodbus

TARGET = modbus_poller
SOURCES = modbus-connect.c

WRITE_TARGET = ../utils/modbus_write_register
WRITE_SOURCES = modbus_write_rtu.c

WRITE_TARGET32 = ../utils/modbus_write_32register
WRITE_SOURCES32 = modbus_write_32rtu.c


poll: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LIBS)

write: $(WRITE_SOURCES)
	$(CC) $(CFLAGS) -o $(WRITE_TARGET) $(WRITE_SOURCES) $(LIBS)


write32: $(WRITE_SOURCES)
	$(CC) $(CFLAGS) -o $(WRITE_TARGET32) $(WRITE_SOURCES32) $(LIBS)



clean:
	rm -f $(TARGET) $(WRITE_SOURCES) $(WRITE_SOURCES32)

install:
	sudo apt-get install libmodbus-dev

.PHONY: all clean install