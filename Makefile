# Compiler and Linker
CC          := clang++-mp-9.0

# Extensions
SRC_EXT      := cc
OBJ_EXT		 := o

# Target
LINK_TARGET	:= Function
EXAMPLE 	:= 0_Function

# Directories
SRC_DIR		:= src
EXAMPLE_DIR	:= examples
OBJ_DIR		:= obj
BIN_DIR		:= bin
INC_DIR		:= include

# Flags, Libraries and Includes
DEP_INC		:= dependencies/include/root
DEP_LIB_DIR	:= dependencies/lib

ROOT_FLAG	:= -L$(DEP_LIB_DIR) -lCore.6.18.04 -lHist.6.18.04 -lGpad.6.18.04
# OPENGL_LIB	:= -framework Cocoa -framework OpenGL -framework IOKit -framework CoreFoundation -framework CoreVideo
CCFLAGS		:=  -std=c++17 -Wall -g -fopenmp

# Making library
OUT_LIB		:= confind
OUT_LIB_DIR	:= lib

# Compile command
COMPILE		:= $(CC) $(CCFLAGS) -I$(DEP_INC) -I$(INC_DIR) 
COMPILE_FLAG:= $(ROOT_FLAG)
# ================================================================================

SOURCES     := $(shell find $(SRC_DIR) -type f -name '*.$(SRC_EXT)')
OBJECTS     := $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(SOURCES:.$(SRC_EXT)=.$(OBJ_EXT)))

# .......................................................................

all: directories $(BIN_DIR)/$(LINK_TARGET)

directories :
	# Make the Directories
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)

$(BIN_DIR)/$(LINK_TARGET):	$(OBJECTS)  $(OBJ_DIR)/$(EXAMPLE).$(OBJ_EXT)
	# Linking the objects
	$(COMPILE) $(COMPILE_FLAG) $? -o $@

$(OBJ_DIR)/%.$(OBJ_EXT) : $(SRC_DIR)/%.$(SRC_EXT)
	# Compiling the dependencies
	$(COMPILE) -c -o $@ $<

$(OBJ_DIR)/$(EXAMPLE).$(OBJ_EXT) : $(EXAMPLE_DIR)/$(EXAMPLE).$(SRC_EXT)
	# Compiling Main EXAMPLE
	$(COMPILE) -c -o $@ $<

# .......................................................................

genlib: mk_lib_dirs complib

complib: $(OBJECTS)
	# Creating library
	ar -cvq $(OUT_LIB_DIR)/lib$(OUT_LIB).a $?

mk_lib_dirs: 
	# Make the Directories
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OUT_LIB_DIR)

# .......................................................................

# Clean only Objects
clean:
	@echo "Cleaning objects..."
	@$(RM) -rf $(OBJ_DIR)

# .......................................................................

# Full Clean, Objects and Binaries
cleaner: clean
	@echo "Cleaning up objects, binaries & library..."
	@$(RM) -rf $(BIN_DIR)
	@$(RM) -rf $(OUT_LIB_DIR)

# .......................................................................

# Non-File Targets
.PHONY: all clean cleaner directories genlib mk_lib_dirs complib

# .......................................................................