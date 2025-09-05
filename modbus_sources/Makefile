CC = gcc
CFLAGS = -Wall -g
LIBS = -lmodbus

TARGET = modbus_poller
SOURCES = modbus-connect.c

WRITE_TARGET = ../utils/modbus_write_register
WRITE_SOURCES = modbus_write_rtu.c

poll: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LIBS)

write: $(WRITE_SOURCES)
	$(CC) $(CFLAGS) -o $(WRITE_TARGET) $(WRITE_SOURCES) $(LIBS)



clean:
	rm -f $(TARGET) $(WRITE_SOURCES)

install:
	sudo apt-get install libmodbus-dev

.PHONY: all clean install