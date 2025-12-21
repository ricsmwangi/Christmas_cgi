# Santa's Christmas CGI Web Generator
# Pure C CGI applications for festive web content

CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99 -D_GNU_SOURCE
LDFLAGS =

# CGI executables
TARGETS = main.cgi tree.cgi card.cgi santa.cgi countdown.cgi

# Source files
SRCS = cgi_utils.c html_utils.c main.c tree.c card.c santa.c countdown.c
HDRS = cgi_utils.h html_utils.h

# Object files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGETS)

# Compile CGI executables
main.cgi: main.o cgi_utils.o html_utils.o
	$(CC) $(LDFLAGS) -o $@ $^

tree.cgi: tree.o cgi_utils.o html_utils.o
	$(CC) $(LDFLAGS) -o $@ $^

card.cgi: card.o cgi_utils.o html_utils.o
	$(CC) $(LDFLAGS) -o $@ $^

santa.cgi: santa.o cgi_utils.o html_utils.o
	$(CC) $(LDFLAGS) -o $@ $^

countdown.cgi: countdown.o cgi_utils.o html_utils.o
	$(CC) $(LDFLAGS) -o $@ $^

# Compile object files
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJS) $(TARGETS)

# Install to cgi-bin (adjust path as needed)
install: $(TARGETS)
	cp $(TARGETS) /usr/lib/cgi-bin/
	chmod +x /usr/lib/cgi-bin/*.cgi

# Test server (requires python3)
test-server:
	python3 -m http.server --cgi 8000

# Run tests
test: $(TARGETS)
	@echo "Testing CGI programs..."
	./test_cgi.sh

.PHONY: all clean install test-server test