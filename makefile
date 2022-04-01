# final
EXE=final

# Main target
all: $(EXE)

#  Msys/MinGW
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall -DUSEGLEW -I/mingw64/include/opencv4
LIBS=-lopencv_videoio -lopencv_core -lglfw3 -lglew32 -lglu32 -lopengl32 -lm
CLEAN=rm -f *.exe *.o *.a
else
#  OSX
ifeq "$(shell uname)" "Darwin"
CFLG=-O3 -Wall -Wno-deprecated-declarations  -DUSEGLEW -I/usr/include/opencv4
LIBS=-lopencv_videoio -lopencv_core -lglfw -lglew -framework Cocoa -framework OpenGL -framework IOKit
#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall -I/usr/include/opencv4
LIBS=-lopencv_videoio -lopencv_core -lglfw -lGLU -lGL -lm
endif
#  OSX/Linux/Unix/Solaris
CLEAN=rm -f $(EXE) *.o *.a
endif

x239lib/CSCIx239.a:
	cd x239lib && $(MAKE)

# Dependencies
final.o: final.c x239lib/CSCIx239.h
sciencefictionnoise.o: sciencefictionnoise.c mylib.h

# Create archive
mylib.a: sciencefictionnoise.o
	ar -rcs $@ $^

# Compile rules
.c.o:
	gcc -c $(CFLG) -I /usr/include/opencv4 $<
.cpp.o:
	g++ -c $(CFLG) -I /usr/include/opencv4 $<

# Link
final:final.o x239lib/CSCIx239.a mylib.a
	g++ $(CFLG) -o $@ $^ -lopencv_videoio -lopencv_core $(LIBS)

# Clean
clean:
	cd x239lib && $(CLEAN)
	$(CLEAN)