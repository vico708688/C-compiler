DST = build
SRC = source
INC = include

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(DST)/%.o, $(SOURCES))

lexer: $(OBJECTS)
	@gcc -o $(DST)/lexer $(OBJECTS)

$(DST)/%.o: $(SRC)/%.c | $(DST)
	@gcc -c $< -o $@ -I$(INC)

$(DST):
	@mkdir -p $(DST)

.PHONY: clean

clean:
	@rm -rf $(DST)/*.o $(DST)/lexer