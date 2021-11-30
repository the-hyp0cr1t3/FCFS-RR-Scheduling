# -*- MakeFile -*-

SRC_DIR = src
OBJ_DIR = lib
BIN_DIR = bin

EXE := $(BIN_DIR)/main
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CC       = gcc
CPPFLAGS = -MMD -MP
CFLAGS 	 = -O3 -w
LDLIBS   = -pthread

INPT_TXTS = c2.txt c3.txt

.PHONY: all clean


all: $(INPT_TXTS) $(EXE)

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

$(INPT_TXTS):
	gcc ./utils/gen_txt.c -o ./gen_txt
	./gen_txt
	@rm ./gen_txt

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR) $(INPT_TXTS) *.shm stats.csv exec.log gen_txt

-include $(OBJ:.o=.d)