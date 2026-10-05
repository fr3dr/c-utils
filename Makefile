CC = gcc
CFLAGS = -Wall -Wextra
SRCDIR = ./src
BINDIR = ./bin

$(BINDIR):
	mkdir --parents --verbose $(BINDIR)

%: $(SRCDIR)/%.c $(BINDIR)
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $<
