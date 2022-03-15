SRC_DIR = src
BUILD_DIR = build
DEP_DIR = third_party
GAME_NAME = game

CC = clang++
DEPS = raylib
INCLUDE = -I$(DEP_DIR)
CFLAGS = -Wall -Wextra -Wpedantic -Werror -Wextra-semi -Wsuggest-override -std=c++17 $(shell pkg-config --cflags $(DEPS))
LDFLAGS = -lm $(shell pkg-config --libs $(DEPS))

SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
HEADERS := $(SRC_DIR)/*.hpp
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

all: dir $(BUILD_DIR)/$(GAME_NAME)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(GAME_NAME): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -o $@ $^

dir:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: dir clean
