CC = gcc
CFLAGS = -Wall -g
SRCS = main.c customer.c repair.c fileio.c utils.c report.c
OBJS = $(SRCS:.c=.o)
TARGET = motorbike_shop

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
