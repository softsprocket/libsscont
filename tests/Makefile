
additional_flags = -std=c11 -I../include

LDFLAGS = -L../lib 
LIBS = -lm -l$(package).$(version)

all check memtest: $(test_exec)

$(test_exec): main.c
	$(CC) main.c $(CFLAGS) $(additional_flags) $(LDFLAGS) -o $@ $(LIBS)

clean:
	-rm $(test_exec)

.PHONY: all clean
