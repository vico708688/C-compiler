DST = build
SRC = source
INC = include

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(DST)/%.o, $(SOURCES))

main: $(OBJECTS)
	@gcc -o $(DST)/main $(OBJECTS)

$(DST)/%.o: $(SRC)/%.c | $(DST)
	@gcc -c $< -o $@ -I$(INC)

$(DST):
	@mkdir -p $(DST)

.PHONY: clean

clean:
	@rm -rf $(DST)/*.o $(DST)/main