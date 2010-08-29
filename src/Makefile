headers=$(wildcard *.h)
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

CFLAGS=-g -Wall 

ginn: $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -lutouch-geis -lm -g -o ginn

$(OBJ): %.o: %.c $(headers)

clean:
	rm -f ginn $(OBJ)
