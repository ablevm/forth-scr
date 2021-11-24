BIN=scr

PREFIX?=/usr/local
BINDIR=${PREFIX}/bin

CFLAGS+=-g -O2 -std=c11 -D_POSIX_C_SOURCE=200112L -pedantic -Wall -Wno-strict-overflow

.PHONY: build clean install uninstall

build: ${BIN}

clean:
	-rm -vf ${BIN}

install: ${BIN}
	@mkdir -p ${BINDIR}
	cp ${BIN} ${BINDIR}/${BIN}

uninstall:
	-rm -vf ${BINDIR}/${BIN}
