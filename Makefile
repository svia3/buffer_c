CC		:= gcc
CFLAGS	:= -Wall -Wextra -g

BIN		:= bin
SRC		:= .
INCLUDE		:= .
BUILDDIR	:= obj
SRCEXT		:= c
OBJEXT		:= o

ifeq ($(OS),Windows_NT)
EXECUTABLE	:= main.exe
RMDIR		:= rmdir /s /q
else
EXECUTABLE	:= main
RMDIR		:= rm -rf

endif

SOURCEDIR	:= $(SRC)
INCLUDEDIRS	:= $(INCLUDE)

CINCLUDES	:= $(patsubst %,-I%, $(INCLUDEDIRS:%/=%))
CLIBS		:= $(patsubst %,-L%, $(LIBDIRS:%/=%))

SOURCES		:= $(wildcard $(patsubst %,%/*.c, $(SOURCEDIR)))

OBJECTS		:= $(patsubst $(SOURCEDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

all: $(BIN)/$(EXACUTABLE)

.PHONY: clean
clean:
	$(RMDIR) $(BIN)
	$(RMDIR) $(BUILDDIR)

disp:
	@echo $(SOURCES)
	@echo $(OBJECTS)

run: all
	./$(BIN)/$(EXECUTABLE)


$(BIN)/$(EXACUTABLE): $(OBJECTS)
	@mkdir -p $(BIN)
	$(CC) -o $(BIN)/$(EXECUTABLE) $^

$(BUILDDIR)/%.$(OBJEXT): $(SOURCEDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(CINCLUDES) -c -o $@ $<
