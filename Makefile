CC = gcc
TARGET = upnp
CFLAGS = 
SRC = upnp.c

$(TARGET) : $(SRC)
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS)

clean:
	rm -rf $(TARGET)
