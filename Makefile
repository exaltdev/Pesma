# Directories
IDIR = inc
SDIR = src
ODIR = $(SDIR)/obj
BDIR = build

# Compiler & flags
CC = gcc
CFLAGS = -g -I$(IDIR) -std=c99
LDFLAGS = -L$(BDIR) -Wall -Wextra -Wl,-rpath=$(BDIR)

# Sources
SRC = p_buffer.c p_file.c p_network.c p_utils.c
OBJ = $(patsubst %,$(ODIR)/%,$(SRC:.c=.o))
TEST = $(SDIR)/pesma_test.c

# Ensure directories exist
$(shell mkdir -p $(ODIR) $(BDIR))

# Default target: build and run test
all: $(BDIR)/pesma_test

# Compile object files for the library
$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -fPIC -c $< -o $@ $(CFLAGS)

# Build shared library
$(BDIR)/libpesma.so: $(OBJ)
	$(CC) -shared -o $@ $^

# Build test executable
$(BDIR)/pesma_test: $(BDIR)/libpesma.so $(TEST)
	$(CC) $(TEST) -o $@ $(CFLAGS) $(LDFLAGS) -lpesma

# Clean
.PHONY: clean all
clean:
	rm -rf $(ODIR)/*.o $(BDIR)/libpesma.so pesma_test *~ core
