DEBUG=yes
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)
EXEC=../bowman

CLIB=-Wall  -lGL -lX11 -lXxf86vm -lXrandr -lpthread -lXi -lXinerama -lXcursor -ldl -lrt -lglfw -lGLEW -lm  -lfreetype

ifeq ($(DEBUG),yes)
	CFLAGS=-Wall  -Wwrite-strings -Wstrict-prototypes -Wuninitialized -Wunreachable-code -Wshadow  -c -g 
else
	CFLAGS=-Wall  -Wwrite-strings -Wstrict-prototypes -Wuninitialized -Wunreachable-code -Wshadow -Wfloat-equal -c -03 
endif

CFLAGS += $(FLAG)

all:$(EXEC) 
ifeq ($(DEBUG),yes)
	@echo "Debug mode"
else 
	@echo "Release mode"
endif

$(EXEC): $(OBJ)
	gcc -o $@ $^ $(CLIB)

%.o: %.c
	gcc $(CFLAGS) $*.c

clean:
	rm -f $(OBJ)

