CC = gcc

# path #
SRC_PATH = src
BUILD_PATH = build
BIN_PATH = $(BUILD_PATH)/bin

# executable # 
BIN_NAME = runner

# extensions #
SRC_EXT = c

# code lists #
# Find all source files in the source directory, sorted by
# most recently modified
SOURCES = $(shell find $(SRC_PATH) -name '*.$(SRC_EXT)' | sort -k 1nr | cut -f2-)
# Set the object file names, with the source directory stripped
# from the path, and the build path prepended in its place
OBJECTS = $(SOURCES:$(SRC_PATH)/%.$(SRC_EXT)=$(BUILD_PATH)/%.o)
# Set the dependency files that will be used to add header dependencies
DEPS = $(OBJECTS:.o=.d)

# flags # add -flto later
COMPILE_FLAGS = -std=gnu11 -Wall -Wextra -O3 -fopenmp -fuse-ld=gold
COMPILE_FLAGS_DEBUG = -std=gnu11 -Wall -Wextra -g -O0 -fopenmp -fuse-ld=gold
INCLUDES = -I include/ -I /usr/local/include
# Space-separated pkg-config libraries used by this project
LIBS = lcglm 
LINKER_FLAGS = -lpthread -lX11 -lglfw -lGL -lXrandr -lXi -ldl -lXinerama -lXcursor \
			   -lrt -lxcb -lXau -lXdmcp -lm -lcglm -lassimp

.PHONY: default_target
default_target: release

.PHONY: release
release: export CCFLAGS := $(CCFLAGS) $(COMPILE_FLAGS)
release: dirs
	@$(MAKE) all

.PHONY: debug
debug: export CCFLAGS := $(CCFLAGS) $(COMPILE_FLAGS_DEBUG)
debug: dirs
	@$(MAKE) all

.PHONY: dirs
dirs:
	@echo "Creating directories"
	@mkdir -p $(dir $(OBJECTS))
	@mkdir -p $(BIN_PATH)

.PHONY: clean
clean:
	@echo "Deleting $(BIN_NAME) symlink"
	@$(RM) $(BIN_NAME)
	@echo "Deleting directories"
	@$(RM) -r $(BUILD_PATH)
	@$(RM) -r $(BIN_PATH)

# checks the executable and symlinks to the output
.PHONY: all
all: $(BIN_PATH)/$(BIN_NAME)
	@echo "Making symlink: $(BIN_NAME) -> $<"
	@$(RM) $(BIN_NAME)
	@ln -s $(BIN_PATH)/$(BIN_NAME) $(BIN_NAME)

# Creation of the executable
$(BIN_PATH)/$(BIN_NAME): $(OBJECTS)
	@echo "Linking: $@"
	$(CC) $(OBJECTS) -o $@ $(LINKER_FLAGS)

# Add dependency files, if they exist
-include $(DEPS)

# Source file rules
# After the first compilation they will be joined with the rules from the
# dependency files to provide header dependencies
$(BUILD_PATH)/%.o: $(SRC_PATH)/%.$(SRC_EXT)
	@echo "Compiling: $< -> $@"
	$(CC) $(CCFLAGS) $(INCLUDES) -MP -MMD -c $< -o $@
