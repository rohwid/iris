CC = g++
CFLAGS = `pkg-config opencv --cflags --libs`
TARGET = iris

all:
	$(CC) -o $(TARGET) $(TARGET).cpp $(CFLAGS)

clean:
	$(RM) $(TARGET)
