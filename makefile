CC=mpicc
CLIBS= -L/usr/local/lib -lglfw -lGL -lGLU -lX11 -lGLEW -lXxf86vm -lXrandr -lXi -lfreetype -lm
CINCLUDES= -I/usr/include/freetype2
CFLAGS= -DSIMPLE -DGLFW -O3 -mfloat-abi=hard -ffast-math -mfpu=vfp -I/usr/local/include -I/usr/include/libdrm


all:
	mkdir -p bin
	cd src; $(CC) $(CINCLUDES) $(CFLAGS) $(CLIBS) ogl_utils.c dividers_gl.c particles_gl.c mover_gl.c font_gl.c lodepng.c exit_menu_gl.c rectangle_gl.c renderer.c liquid_gl.c glfw_utils.c image_gl.c cursor_gl.c background_gl.c controls.c geometry.c hash.c communication.c fluid.c -o ../bin/sph.out $(CLIBS)

clean:
	rm -f ./sph.out
	rm -f ./*.o
