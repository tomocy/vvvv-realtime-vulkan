ARGS ?=

SRC_DIR := .
BUILD_DIR := build

EXE_NAME := vvvv

EXE := $(BUILD_DIR)/$(EXE_NAME)

CMAKE_LISTS := $(SRC_DIR)/CMakeLists.txt
CMAKE_CACHE := $(BUILD_DIR)/CMakeCache.txt

$(CMAKE_CACHE): $(CMAKE_LISTS)
	cmake -S $(SRC_DIR) -B $(BUILD_DIR)

.PHONY: build
build: $(CMAKE_CACHE)
	cmake --build $(BUILD_DIR) --target $(EXE_NAME) --parallel

.PHONY: run
run: build
	$(EXE) $(ARGS)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
