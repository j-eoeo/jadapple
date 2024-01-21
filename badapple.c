#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <fcntl.h>

#include <X11/Xlib.h>

#include <png.h>

#include "jblack.xbm"

#undef MONO

char* strcat2(const char* str1, const char* str2){
	char* str = malloc(strlen(str1) + strlen(str2) + 1);
	strcpy(str, str1);
	strcpy(str + strlen(str1), str2);
	str[strlen(str1) + strlen(str2)] = 0;
	return str;
}

int main(int argc, char** argv){
	int size = 10;
	int jsize = jblack_width;
	if(argc > 1){
		size = atoi(argv[1]);
		if(size <= 0){
			fprintf(stderr, "%s: size must be >0\n", argv[0]);
			return 1;
		}
	}
	struct dirent** namelist;
	int n = scandir("frames", &namelist, NULL, alphasort);
	int i;
	Display* ds = XOpenDisplay(NULL);
	Window w = XCreateSimpleWindow(ds, RootWindow(ds, 0), 0, 0, 480 / size * jsize, 360 / size * jsize, 1, 0, 0xffffff);
#ifndef MONO
#define CREATE_PIXMAP(a, c) 	a = XCreatePixmapFromBitmapData(ds, RootWindow(ds, 0), jblack_bits, jblack_width, jblack_height, c, 0xffffff - c, 24);
	Pixmap pixmaps[256];
	for(i = 0; i < 256; i++){
		int g = i;
		g |= (g << 16) | (g << 8);
		CREATE_PIXMAP(pixmaps[i], g);
	}
#else
#define CREATE_PIXMAP(a, c) 	Pixmap a = XCreatePixmapFromBitmapData(ds, RootWindow(ds, 0), jblack_bits, jblack_width, jblack_height, c, 0xffffff - c, 24);
	CREATE_PIXMAP(jblack, 0);
	CREATE_PIXMAP(jgray1, 0x444444);
	CREATE_PIXMAP(jgray2, 0x888888);
	CREATE_PIXMAP(jgray3, 0xcccccc);
	CREATE_PIXMAP(jwhite, 0xffffff);
#endif
	GC gc = XCreateGC(ds, RootWindow(ds, 0), 0, 0);

	XMapWindow(ds, w);
	XSync(ds, False);

	for(i = 0; i < n; i++){
		struct dirent* d = namelist[i];
		if(strcmp(d->d_name, "..") != 0 && strcmp(d->d_name, ".") != 0){
			struct timeval tv;
			gettimeofday(&tv, NULL);
			char* p = strcat2("frames/", d->d_name);
			FILE* f = fopen(p, "rb");
			png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
			png_infop info = png_create_info_struct(png);
			setjmp(png_jmpbuf(png));
			png_init_io(png, f);
			png_read_info(png, info);
			int width, height;
			png_byte color_type;
			png_byte bit_depth;
			width = png_get_image_width(png, info);
			height = png_get_image_width(png, info);
			color_type = png_get_color_type(png, info);
			bit_depth = png_get_bit_depth(png, info);
			if(!(color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)) png_set_rgb_to_gray(png, 1, 54 / 256, 183 / 256);
			png_bytep* row = NULL;
			row = (png_bytep*)malloc(sizeof(*row) * height);
			int y;
			for(y = 0; y < height; y++){
				row[y] = (png_byte*)malloc(png_get_rowbytes(png, info));
			}
			png_read_image(png, row);
			int x;
			for(y = 0; y < height; y++) {
				if(y % size == 0){
					for(x = 0; x < width; x += size){
						Pixmap px;
						int a = row[y][x];
#ifndef MONO
						px = pixmaps[a];
#else
						if(a >= 128){
							px = jwhite;
						}else{
							px = jblack;
						}
#endif
						XCopyArea(ds, px, w, gc, 0, 0, jsize, jsize, x / size * jsize, y / size * jsize);
					}
				}
				free(row[y]);
			}
			XFlush(ds);
			free(row);
			fclose(f);
			png_destroy_read_struct(&png, &info, NULL);
			free(p);
			struct timeval tv2;
			gettimeofday(&tv2, NULL);
			int slep = (1000 * 1000 / 30) - (((tv2.tv_sec * 1000 * 1000) + tv2.tv_usec) - ((tv.tv_sec * 1000 * 1000) + tv.tv_usec));
			if(slep > 0) usleep(slep);
		}
		free(d);
	}
	free(namelist);
	printf("\x1b[2J\x1b[1;1H");
	fflush(stdout);
}
