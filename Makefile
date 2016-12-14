CC            = gcc
CFLAGS        = -W -Wall -O3 -fPIC
INCLUDES      = -I.
SOURCE_DIR    = ./src
HEADER_DIR    = ./include
OBJ_DIR       = ./obj
LIB_DIR       = ./lib
SOURCE_FILES := $(wildcard $(SOURCE_DIR)/*.c)
OBJS         := $(patsubst %.c,%.o,$(subst $(SOURCE_DIR),$(OBJ_DIR),$(SOURCE_FILES)))
TARGET        = libflakeid.a
TARGET_FILE   = $(LIB_DIR)/$(TARGET)

.PHONY: all test clean

$TARGET_FILE: $(OBJS)
	ar r $(TARGET_FILE) $(OBJS)

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.c $(HEADER_DIR)/%.h
	$(CC) -c $(CFLAGS) -I$(HEADER_DIR) -o $@ $<

test: $(TARGET_FILE)
	gcc -g test/test.c $(TARGET_FILE) -I$(HEADER_DIR) -o ./test/test

clean:
	@rm $(OBJS) $(TARGET_FIELS) -f
