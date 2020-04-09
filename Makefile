# Extensions
SRC_EXT      := cc
OBJ_EXT		 := o

# Target
# LINK_TARGET	:= function
# EXAMPLE 	:= 0_Function

# LINK_TARGET	:= memfunction
# EXAMPLE 	:= 1_MemFunction

# LINK_TARGET	:= memfunctionOptimized
# EXAMPLE 	:= 2_MemFunctionOptimized

# LINK_TARGET	:= modify_object
# EXAMPLE 	:= 3_Modify_Object

# LINK_TARGET	:= parallel
# EXAMPLE 	:= 4_Parallel

# LINK_TARGET	:= test
# EXAMPLE 	:= test

LINK_TARGET	:= omp_prototype
EXAMPLE 	:= omp_prototype
# ----------------------------------
# Directories
SRC_DIR		:= src
EXAMPLE_DIR	:= examples
OBJ_DIR		:= obj
BIN_DIR		:= bin
INC_DIR		:= include
BACKUP_DIR	:= ../Backup

# ----------------------------------
# Compiler and Libraries
ifeq ($(shell uname), Linux)
	CC 			:= g++
	ROOT_LIB	:= /home/zaki/HEP_Tools/root_dir/lib
	ROOT_INC	:= /home/zaki/HEP_Tools/root_dir/include
	OUT_LIB_DIR	:= lib/linux
	ZAKI_LIB	:= dependencies/lib/zaki/linux 
else
	CC 			:= clang++-mp-9.0
	ROOT_LIB	:= /opt/local/lib/root6
	ROOT_INC	:= /opt/local/include/root6
	OUT_LIB_DIR	:= lib/darwin
	ZAKI_LIB	:= dependencies/lib/zaki/darwin 
endif
# ----------------------------------

# Flags, Libraries and Includes
DEP_INC		:= dependencies/include

ROOT_FLAG	:= -L$(ROOT_LIB) -lCore -lHist -lGpad -lGraf
CCFLAGS		:= --std=c++14 -Wall -g -fopenmp -fPIC
ZAKI_FLAG	:= -L$(ZAKI_LIB) -lzaki

# Making library
OUT_LIB		:= confind


# Compile command
COMPILE		:= $(CC) $(CCFLAGS) -I$(DEP_INC) -I$(INC_DIR) -I$(ROOT_INC) 
COMPILE_FLAG:= $(ROOT_FLAG) $(ZAKI_FLAG)
# ================================================================================

SOURCES     := $(shell find $(SRC_DIR) -type f -name '*.$(SRC_EXT)')
OBJECTS     := $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(SOURCES:.$(SRC_EXT)=.$(OBJ_EXT)))

# INC_SUB_DIR := $(addprefix $(INC_DIR)/,$(MODULES))

# vpath %.h $(INC_SUB_DIR) 
# .......................................................................

all: directories $(BIN_DIR)/$(LINK_TARGET)

directories :
	@echo "Creating the directories..." 
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)

$(BIN_DIR)/$(LINK_TARGET):	$(OBJECTS)  $(OBJ_DIR)/$(EXAMPLE).$(OBJ_EXT)
	@echo "Linking the objects..."
	$(COMPILE)  $^ -o $@ $(COMPILE_FLAG)

$(OBJ_DIR)/%.$(OBJ_EXT) : $(SRC_DIR)/%.$(SRC_EXT) $(INC_DIR)/%.h
	@echo "Compiling the dependencies..."
	$(COMPILE) -c -o $@ $<

$(OBJ_DIR)/$(EXAMPLE).$(OBJ_EXT) : $(EXAMPLE_DIR)/$(EXAMPLE).$(SRC_EXT)
	@echo "Compiling the Main file..."
	$(COMPILE) -c -o $@ $<

# in case only header file changes
$(INC_DIR)/%.h : 
	$(COMPILE) -c -o $(OBJ_DIR)/%.$(OBJ_EXT) $(SRC_DIR)/%.$(SRC_EXT)
# .......................................................................

genlib: mk_lib_dirs complib

complib: $(OBJECTS)
	@echo "Creating static library..."
	@rm -f $(OUT_LIB_DIR)/lib$(OUT_LIB).a
	ar -cvq $(OUT_LIB_DIR)/lib$(OUT_LIB).a $^
	# gcc -shared  $^ -o $(OUT_LIB_DIR)/lib$(OUT_LIB).so

mk_lib_dirs: 
	@echo "Creating directories..."
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OUT_LIB_DIR)

# .......................................................................

backup:	
	@echo "Backing up..."
	@mkdir -p $(BACKUP_DIR)/$(BACK_SUB_DIR)
	@cp -r $(SRC_DIR) $(BACKUP_DIR)/$(BACK_SUB_DIR)
	@cp -r $(INC_DIR) $(BACKUP_DIR)/$(BACK_SUB_DIR)
	@cp Makefile $(BACKUP_DIR)/$(BACK_SUB_DIR)
	# @zip -r $(BACKUP_DIR)/$(BACK_SUB_DIR).zip $(BACKUP_DIR)/$(BACK_SUB_DIR)
	# @$(RM) -rf $(BACKUP_DIR)/$(BACK_SUB_DIR)
	@echo "Backed up in:" $(BACKUP_DIR)/$(BACK_SUB_DIR)

# .......................................................................

help:
	@echo "\n * -------------------------------------------------------- * "
	@echo " |		***  Welcome to CONFIND!   ***              |"
	@echo " * -------------------------------------------------------- * "
	@echo " |  Available Commands: \n |"
	@echo " |  - Use 'make help' to show this message."
	@echo " |  - Use 'make' to compile the main cc target."
	@echo " |  - Use 'make genlib' to generate a static library."
	@echo " |  - Use 'make BACK_SUB_DIR=XXX backup' to back up the code,\n |    inside '../Backup/XXX'."
	@echo " |  - Use 'make clean' to clean the objects."
	@echo " |  - Use 'make cleaner' to clean the objects, the binaries,\n |     & the library."
	@echo " * -------------------------------------------------------- * \n"

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