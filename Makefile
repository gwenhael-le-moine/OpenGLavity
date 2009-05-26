#compilation
CC = g++ -g # g++ because glui is made of C++
CCLIBS += -lGL -lGLU -lglut -L/usr/X11R6/lib -lglui# -lX11 -lXi -lXmu
# CC = gcc
# CCLIBS += -lstdc++ -lGL -lGLU -lglut -L/usr/X11R6/lib -lglui -lX11 -lXi -lXmu
CCINC += -I/usr/X11R6/include
CCOPTIMISATION += #-O3 -funroll-loops -march=athlon-xp
CCFLAGS += -Wall #-s #-DHAVE_GLUI -DSEPARATE_CONTROLS

#directories
SOURCES_DIR = ./src/
HTML_DIR = ./html/
UNIVERSES_DIR = ./universes/

WD = $(shell pwd)

#files
BIN = ./OpenGLavity
SOURCES = \
 ./src/main.cpp\
 ./src/grav.h\
 ./src/es.h\
 ./src/glutstuff.h\
 ./src/globalvars.h\
 ./src/gluistuff.h\
 ./src/config.h
HTML = \
 ./src/main.cpp.html\
 ./src/grav.h.html\
 ./src/es.h.html\
 ./src/glutstuff.h.html\
 ./src/globalvars.h.html\
 ./src/gluistuff.h.html\
 ./src/config.h.html

all:
	$(MAKE) $(BIN)

$(BIN):$(SOURCES) Makefile
	@$(CC) -o $@ $< $(CCLIBS) $(CCINC) $(CCOPTIMISATION) $(CCFLAGS)
	@echo $(BIN) pour lancer le programme
	@echo il serais judicieux de lire le README

clean:
	@rm -f ./*~ $(SOURCES_DIR)*~ $(SOURCES_DIR)*.o $(UNIVERSES_DIR)*~ $(HTML_DIR)*~

real-clean:
	$(MAKE) clean
	@rm -f $(BIN) ./*.unv

archive:
	$(MAKE) clean
	$(MAKE) html
	cd ..; \
	tar jcf $(join $(WD), .tar.bz2) ./$(shell basename $(WD)); \
	tar zcf $(join $(WD), .tar.gz) ./$(shell basename $(WD)); \
	cd $(WD)

#html:$(SOURCES)
#	./build_html.sh
html:
	cd $(SOURCES_DIR) ; \
	webcpp main.cpp main.cpp.html -l -m -H ; \
	cd .. ; \
	mv $(SOURCES_DIR)/*.html $(HTML_DIR)
