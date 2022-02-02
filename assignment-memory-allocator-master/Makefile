CC=gcc
CFLAGS=--std=c17 -Wall -pedantic -Isrc/ -ggdb -Wextra -Werror -DDEBUG

BUILDDIR=build
SRCDIR=src
EXECUTABLE_FILE_NAME=main

.PHONY: all clean test

SRCS := $(wildcard $(SRCDIR)/*.c)
OBJS := $(SRCS:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all clean

all: $(BUILDDIR)/$(EXECUTABLE_FILE_NAME)

-include $(DEPS)

# Creates build directory
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Creates obj files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) -c $(CFLAGS) $< -o $@ -MMD

# Creates executable file
$(BUILDDIR)/$(EXECUTABLE_FILE_NAME): $(OBJS)
	$(CC) -o $(BUILDDIR)/$(EXECUTABLE_FILE_NAME) $^

clean:
	rm -rf $(BUILDDIR)

run:
	./$(BUILDDIR)/$(EXECUTABLE_FILE_NAME)

test:
	@+cd tester; make CC=$(CC)
