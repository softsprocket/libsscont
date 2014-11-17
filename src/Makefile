
additional_flags = -std=c11 -I../include -fpic
LIBS = -lm

all: lib$(package).$(version).so

objects = auto_array.o hash.o set.o string.o

auto_array.o: auto_array.c
	 $(CC) -c auto_array.c $(CFLAGS) $(additional_flags) -o $@ 

hash.o: hash.c
	$(CC) -c hash.c $(CFLAGS) $(additional_flags) -o $@ 

set.o: set.c
	$(CC) -c set.c $(CFLAGS) $(additional_flags) -o $@ 

string.o: string.c
	$(CC) -c string.c $(CFLAGS) $(additional_flags) -o $@

lib$(package).$(version).so: $(objects)
	mkdir ../lib
	$(CC) $(CFLAGS) $(additional_flags) -shared -o ../lib/$@ $(objects) $(LIBS)

clean:
	-rm *.o
	-rm ../lib/lib$(package).$(version).so

install:
	install -d  $(DESTDIR)$(prefix)/lib/softsprocket
	install -d  $(DESTDIR)$(prefix)/include/softsprocket
	install -m 0644 ../lib/lib$(package).$(version).so  $(DESTDIR)$(prefix)/lib/softsprocket
	install -m 0644 ../include/container.h $(DESTDIR)$(prefix)/include/softsprocket
	install -m 0644 ../include/debug_utils.h $(DESTDIR)$(prefix)/include/softsprocket
	
uninstall:
	-rm  $(DESTDIR)$(prefix)/lib/softsprocket/lib$(package).$(version).so
	-rm -d  $(DESTDIR)$(prefix)/lib/softsprocket
	-rm $(DESTDIR)$(prefix)/include/softsprocket/container.h
	-rm $(DESTDIR)$(prefix)/include/softsprocket/debug_utils.h
	-rm -d $(DESTDIR)$(prefix)/include/softsprocket
	

.PHONY: all clean install uninstall


