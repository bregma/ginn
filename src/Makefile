headers=$(wildcard *.h)
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

CFLAGS=-g -Wall -I/usr/include/libxml2

ginn: $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -lX11 -lXtst -lutouch-geis -lxml2 -lm -g -o ginn

$(OBJ): %.o: %.c $(headers)

clean:
	rm -f ginn $(OBJ)
