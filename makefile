CC = gcc
CSTD = c90

DEBUG ?= 0

SRC_DIR = src
INC_DIR = include /opt/homebrew/include
DEBUG_BIN_DIR = bin/debug
DEBUG_BUILD_DIR = build/debug
RELEASE_BIN_DIR = bin/release
RELEASE_BUILD_DIR = build/release

LIBS = -lcurl -lsqlite3 -lcjson -L/opt/homebrew/lib
CFLAGS = -Wall -Wextra -Werror -Wpedantic $(addprefix -I,$(INC_DIR)) -std=$(CSTD) -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wcast-qual -Wswitch-default -Wswitch-enum -Wunreachable-code -Wfloat-equal -Wconversion -Wdouble-promotion -Wmissing-prototypes -Wundef -Wunused-macros -Wwrite-strings

TARGET = music_guesser

ifeq ($(DEBUG), 1)
	BIN_DIR = $(DEBUG_BIN_DIR)
	BUILD_DIR = $(DEBUG_BUILD_DIR)
	CFLAGS += -g -O0
else
	BIN_DIR = $(RELEASE_BIN_DIR)
	BUILD_DIR = $(RELEASE_BUILD_DIR)
	CFLAGS += -O2
endif

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

all: $(BUILD_DIR) $(BIN_DIR)/$(TARGET)

debug:
	$(MAKE) all DEBUG=1

release:
	$(MAKE) all DEBUG=0

$(BIN_DIR) $(BUILD_DIR):
	mkdir -p $@

$(BIN_DIR)/$(TARGET): $(OBJ_FILES) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(DEBUG_BUILD_DIR) $(DEBUG_BIN_DIR) $(RELEASE_BUILD_DIR) $(RELEASE_BIN_DIR)
