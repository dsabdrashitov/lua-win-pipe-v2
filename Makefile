OUTPUT_NAME := lua-win-pipe-v_2_0-lua54-win64
DLL_NAME := lua_win_pipe_v2

CC := g++
LIBS := lua54

BUILD_DIR := ./build
SRC_LUA_PREFIX := ./src-lua
SRC_CPP_PREFIX := ./src-cpp

SRC_CPP_DIRS := $(SRC_CPP_PREFIX) $(SRC_CPP_PREFIX)/functions
INC_DIRS := ./include-cpp
LIB_DIRS := ./lib-cpp

OUTPUT_DIR := $(BUILD_DIR)/$(OUTPUT_NAME)
OUTPUT_DLL := $(OUTPUT_DIR)/$(DLL_NAME).dll

SRCS := $(foreach dir,$(SRC_CPP_DIRS),$(wildcard $(dir)/*.cpp))
HEADERS := $(foreach dir,$(SRC_CPP_DIRS),$(wildcard $(dir)/*.h))
OBJ_PREFIX := $(BUILD_DIR)/obj
OBJS := $(patsubst $(SRC_CPP_PREFIX)/%.cpp,$(OBJ_PREFIX)/%.o,$(SRCS))

SRC_LUA_DIRS := $(SRC_LUA_PREFIX)
LUAS := $(foreach dir,$(SRC_LUA_DIRS),$(wildcard $(dir)/*.lua))
LUAB := $(patsubst $(SRC_LUA_PREFIX)/%.lua,$(OUTPUT_DIR)/%.lua,$(LUAS))

INC_FLAGS := $(addprefix -I,$(INC_DIRS))
LIB_FLAGS := $(addprefix -L,$(LIB_DIRS)) $(addprefix -l,$(LIBS))
LDFLAGS := $(LIB_FLAGS) -static-libgcc -static-libstdc++
CFLAGS := $(INC_FLAGS)

all: $(OUTPUT_DLL) $(LUAB)

just_dll: $(OUTPUT_DLL)

$(OUTPUT_DLL): $(OBJS)
	-mkdir -p $(dir $@)
	$(CC) -shared $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_PREFIX)/%.o: $(SRC_CPP_PREFIX)/%.cpp $(HEADERS)
	-mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTPUT_DIR)/%.lua: $(SRC_LUA_PREFIX)/%.lua
	-mkdir -p $(dir $@)
	cp $< $@

tree:
	-mkdir -p build
	git ls-files --others --cached --exclude-standard > ./build/project_structure.txt

clean:
	-if [ -d "$(BUILD_DIR)" ]; then rm -r $(BUILD_DIR); fi
