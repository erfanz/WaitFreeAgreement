CC = g++
LD = g++

CPPFLAGS = -std=gnu++14 -Wall -Wconversion -Wextra -Wno-ignored-qualifiers -DGTEST_OS_CYGWIN=1
BUILDDIR = build/widgets

#MODULES   := bases_classes base_types tests util
MODULES   := base_types base_classes util tests
SRC_DIR   := $(addprefix src/,$(MODULES))
BUILD_DIR := $(addprefix build/,$(MODULES))
EXE_DIR   := exe

SRC       := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))
OBJ       := $(patsubst src/%.cpp,build/%.o,$(SRC))
INCLUDES  := $(addprefix -I,$(SRC_DIR))


# -------------------------------------------------
# telling the make to seach for cpp files in these folders
vpath %.cpp $(SRC_DIR)

# -------------------------------------------------
# universal compilie commands
define make-goal
$1/%.o: %.cpp
	$(CC) $(CPPFLAGS) $(INCLUDES) -c $$< -o $$@
endef

.PHONY: all checkdirs clean

# -------------------------------------------------
all: checkdirs $(EXE_DIR)/test.exe

# -------------------------------------------------
# linking all object files to create the executable
$(EXE_DIR)/test.exe: $(OBJ)
	$(LD) $^ -o $@

# -------------------------------------------------
# Creating needed folders if don't exist
checkdirs: $(BUILD_DIR) $(EXE_DIR)

$(BUILD_DIR):
	@mkdir -p $@

$(EXE_DIR):
	@mkdir -p $@
# -------------------------------------------------
# Cleaning the object files
clean:
	#rm -f $(OBJ)
	rm -rf $(BUILD_DIR)
	

# this line creates the implicit rules for each build using the function make-goal,
# and it is not necessary write them one by one
$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))