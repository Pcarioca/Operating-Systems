CC = gcc
CFLAGS = -Wall -g
TARGET = build/treasure_manager
OBJS = treasure_manager.o treasure_io.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(TARGET)
