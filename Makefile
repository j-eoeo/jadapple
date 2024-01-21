CC := cc
CFLAGS := -g -Iinclude
LDFLAGS :=
LIBS := -Wl,-R/usr/pkg/lib -L/usr/pkg/lib -lpng -Wl,-R/usr/X11R7/lib -L/usr/X11R7/lib -lX11

./badapple: ./badapple.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

./badapple.o: ./badapple.c ./jblack.xbm
	$(CC) $(CFLAGS) -c -o $@ $<
