
# Compiler, Flags and Libraries to be used/linked
CC           = gcc
OPT          = -O3
#DEBUG        = -Wall -g
DEBUG        = -g
CFLAGS       = $(DEBUG) $(OPT) 
#LIBDIR       = -L$(LIBPATH)
#LIBS         = -lSDL -lelf -lz -lrt -lpthread -lutil

# Directories, Sources and Executable names
BASE_DIR     = ${PWD}
OBJECTS      = $(addprefix $(BUILD_DIR)/, $(SOURCES:.c=.o))
INC_DIR      = -I./includes
BUILD_DIR    = ./objects
SOURCES		 = compiler.c
EXE          = compiler.x

$(shell mkdir -p $(BUILD_DIR))

.SUFFIXES: .c .o .x
.PHONY: clean all 

all: $(EXE)

$(EXE): $(OBJECTS)
	@echo "Building $@ ..."
	@$(CC) $(CFLAGS) $(LIBDIR) -o $@ $(OBJECTS) $(LIBS) 2>&1 

$(BUILD_DIR)/%.o:$(BASE_DIR)/%.c
	@echo "Compiling $(notdir $<) ..."
	@$(CC) $(CFLAGS) $(INC_DIR) -o $@ -c $<

clean:
	@echo "Cleaning ... $(EXE) $(OBJECTS)"
	@rm -f $(EXE) $(OBJECTS) *~
	@rmdir $(BUILD_DIR)

