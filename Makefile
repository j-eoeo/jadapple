CC := cc
CFLAGS := -g -Iinclude -I/usr/X11R7/include -I/usr/pkg/include/libpng16
LDFLAGS :=
LIBS := -Wl,-R/usr/pkg/lib -L/usr/pkg/lib -lpng16 -Wl,-R/usr/X11R7/lib -L/usr/X11R7/lib -lX11

./badapple: ./badapple.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

./badapple.o: ./badapple.c ./jblack.xbm
	$(CC) $(CFLAGS) -c -o $@ $<
