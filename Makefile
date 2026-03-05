PROJECT_NAME := lua_win_pipe_v_2_2_lua54_win64
DLL_NAME := lua_win_pipe_v2
ZIP_NAME := lib.zip

CC := g++
LIBS := lua54

BUILD_DIR := ./build
LIB_OUT_DIR := $(BUILD_DIR)/lib
PKG_DIR := $(LIB_OUT_DIR)/$(PROJECT_NAME)

SRC_LUA_PREFIX := ./src-lua
SRC_CPP_PREFIX := ./src-cpp

SRC_CPP_DIRS := $(SRC_CPP_PREFIX) $(SRC_CPP_PREFIX)/functions
INC_DIRS := ./include-cpp
LIB_DIRS := ./lib-cpp

OUTPUT_DLL := $(PKG_DIR)/$(DLL_NAME).dll
PROXY_LUA := $(LIB_OUT_DIR)/$(PROJECT_NAME).lua
OUTPUT_ZIP := $(BUILD_DIR)/$(ZIP_NAME)

SRCS := $(foreach dir,$(SRC_CPP_DIRS),$(wildcard $(dir)/*.cpp))
HEADERS := $(foreach dir,$(SRC_CPP_DIRS),$(wildcard $(dir)/*.h))
OBJ_PREFIX := $(BUILD_DIR)/obj
OBJS := $(patsubst $(SRC_CPP_PREFIX)/%.cpp,$(OBJ_PREFIX)/%.o,$(SRCS))

LUAS := $(wildcard $(SRC_LUA_PREFIX)/*.lua)
LUAB := $(patsubst $(SRC_LUA_PREFIX)/%.lua,$(PKG_DIR)/%.lua,$(LUAS))

INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CFLAGS := $(INC_FLAGS) -std=c++17 -O2 -Wall

LIB_FLAGS := $(addprefix -L,$(LIB_DIRS)) $(addprefix -l,$(LIBS))
LDFLAGS := $(LIB_FLAGS) -static-libgcc -static-libstdc++

.PHONY: all clean just_dll tree zip

all: $(OUTPUT_DLL) $(LUAB) $(PROXY_LUA) zip

just_dll: $(OUTPUT_DLL)

$(OUTPUT_DLL): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) -shared $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_PREFIX)/%.o: $(SRC_CPP_PREFIX)/%.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(PKG_DIR)/%.lua: $(SRC_LUA_PREFIX)/%.lua
	@mkdir -p $(dir $@)
	cp $< $@

$(PROXY_LUA):
	@mkdir -p $(dir $@)
	@echo 'local path = ...' > $@
	@echo "return require(path .. '.init')" >> $@

zip: $(OUTPUT_DLL) $(LUAB) $(PROXY_LUA)
	@echo "Creating ZIP archive using tar: $(ZIP_NAME)..."
	@cd $(LIB_OUT_DIR) && tar -a -c -f ../$(ZIP_NAME) *
	@echo "Archive created: $(OUTPUT_ZIP)"

tree:
	@mkdir -p $(BUILD_DIR)
	@git ls-files --others --cached --exclude-standard > $(BUILD_DIR)/project_structure.txt

clean:
	@if [ -d "$(BUILD_DIR)" ]; then rm -rf $(BUILD_DIR); fi
	@echo "Cleaned."
