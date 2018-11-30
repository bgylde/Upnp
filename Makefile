CC = gcc
TARGET = upnp
CFLAGS = -lpthread -lcurl 
SRC = upnp.c handle.c

$(TARGET) : $(SRC)
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS)

clean:
	rm -rf $(TARGET)
