CC=arm-none-eabi-gcc
CFLAGS=-mcpu=cortex-m3 -mthumb -Os -Wall -I../cmsis-header-stm32/stm32f1xx/Include -I../cmsis_stub

SRCS=main.c
OBJS=$(SRCS:.c=.o)

all: $(OBJS)
	@echo "Compilation successful"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)