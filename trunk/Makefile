CFLAGS = -W -Wall -Wshadow -pedantic -Werror -O2
TARGET = openfm
SOURCES = common.c openfm.c
OBJECTS = $(patsubst %.c,%.o,$(SOURCES))
CC = gcc


$(TARGET): $(OBJECTS)

$(OBJECTS): $(SOURCES)

openfm.pot: $(SOURCES)
	echo $(SOURCES) | sed 's| |\n|g' | xgettext \
		--output=openfm.pot \
		--output-dir=po \
		--keyword=_ --keyword=PRINTLN --keyword=PRINTLN1 \
		--keyword=PRINTLN2 --keyword=PRINTLN3 \
		--language=C --no-location \
		--msgid-bugs-address="Slava Semushin <php-coder at altlinux.ru>" \
		--files-from=-

clean:
	-rm -f $(TARGET) $(OBJECTS)

.PHONY: clean

