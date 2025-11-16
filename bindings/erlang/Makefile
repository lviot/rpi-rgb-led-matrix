RGB_LIBDIR=../../lib
RGB_INCLUDEDIR=../../include
RGB_LIBRARY_NAME=rgbmatrix
RGB_LIBRARY=$(RGB_LIBDIR)/lib$(RGB_LIBRARY_NAME).so.1

build: $(RGB_LIBRARY)
	gcc -o $(RGB_LIBRARY_NAME)_nif.so -fpic -shared bindings.c options.c types.c -L $(RGB_LIBDIR) -l$(RGB_LIBRARY_NAME) -I $(RGB_INCLUDEDIR) -I .

$(RGB_LIBRARY):
	$(MAKE) -C $(RGB_LIBDIR)

library: $(RGB_LIBRARY)
