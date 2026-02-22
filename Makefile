DST = build
SRC = source
INC = include

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(DST)/%.o, $(SOURCES))

C_FLAGS = -I$(INC)
DEBUG_FLAGS = -g -pg -DDEBUG
DEFAULT_FLAGS = 

all: main

debug: DEFAULT_FLAGS += $(DEBUG_FLAGS)
debug: main

main: $(OBJECTS)
	gcc $(DEBUG_FLAGS) -o $(DST)/main $(OBJECTS)

$(DST)/%.o: $(SRC)/%.c | $(DST)
	gcc $(C_FLAGS) $(DEFAULT_FLAGS) -c $< -o $@

$(DST):
	@mkdir -p $(DST)

.PHONY: clean debug

clean:
	@rm -rf $(DST)/*.o $(DST)/main