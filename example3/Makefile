#DEBUG_ON=1

ifdef NEWLINE
	NEWLINE = @echo ""
endif

CXX=g++
PROGRAM=ShaderBox
DEL=rm -rf
EXT=*.bb *.bbg *.da *.d *.gcov *.o *.bmp *.o gmon.out
DFLAGS=-g -pg -fprofile-arcs  -fbranch-probabilities -ftest-coverage
DFLAGS=-g -pg
LIBS=-lm -lsfml-window -lsfml-system -lGLU
OP= -ffast-math \
-O3 \
-funroll-loops -ftree-vectorize -fvisibility=hidden -fvisibility-inlines-hidden \
-fmessage-length=0 \
-minline-all-stringops -funroll-loops -fforce-addr \
-mno-push-args -maccumulate-outgoing-args -mno-align-stringops \
-fsingle-precision-constant -ftracer \
-finline-limit=5001 \
#WARN=-Wall 
WARN=-w
#WARN= -Wreturn-type -Wformat -Wmissing-braces -Wparentheses -Wswitch -Wunused-function -Wunused-variable -Wunused-value -Wuninitialized -Wshadow -Wsign-compare -Wnewline-eof -Wno-trigraphs

ifdef DEBUG_ON
	FLAGS= $(WARN)  $(DFLAGS)
	DEBUG_CMD=/bin/sh ./debug_data/d.debug
else
	FLAGS= $(WARN) $(OP)
endif

# Make commands

# objects
COMMON_OBJECTS= $(patsubst %.cpp, %.o, $(wildcard ./*.cpp))
GLEW_OBJECTS= $(patsubst %.c, %.o, $(wildcard ./glew/*.c))

OBJECTS=$(COMMON_OBJECTS) $(GLEW_OBJECTS)

# build stuff
makeall:
	$(NEWLINE)
	make $(PROGRAM)
#	$(DEBUG_CMD)

debug:
	$(NEWLINE)
	make $(PROGRAM)
	$(CXX) $(FLAGS) $(LIBS) $(OP) -o $(PROGRAM) $(OBJECTS)
#	$(DEBUG_CMD)

$(PROGRAM): $(OBJECTS) 
	$(CXX) $(FLAGS) $(LIBS) $(OP) -o $(PROGRAM) $(OBJECTS)

%.o: %.cpp
	@echo "--$@--"
	$(CXX) -c  $(FLAGS) -o $@ $<
	$(NEWLINE)

%.o: %.c
	@echo "--$@--"
	$(CXX) -c  $(FLAGS) -o $@ $<
	$(NEWLINE)

# clean crap
.PHONEY: clean cleanall
clean:
	@echo $(OBJECTS)
	$(NEWLINE)
	$(DEL) $(OBJECTS)

cleanall:
	make clean
	ls
	find ./ -name "*.o" | xargs rm -rf 2> /dev/null
	find ./ -name "*bmp" | xargs rm -rf 2> /dev/null
	find ./ -name "*.bb" | xargs rm -rf 2> /dev/null
	find ./ -name "*.bbg" | xargs rm -rf 2> /dev/null
	find ./ -name "*.da" | xargs rm -rf 2> /dev/null
	find ./ -name "*.d" | xargs rm -rf 2> /dev/null
	find ./ -name "*.gcov" | xargs rm -rf 2> /dev/null
	find ./ -name "*.gcda" | xargs rm -rf 2> /dev/null
	find ./ -name "*.gcno" | xargs rm -rf 2> /dev/null
	#make -s -i -f ../Makefile -C $(FRONTDIR) clean-debug
