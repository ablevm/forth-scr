BIN=scr

PREFIX?=/usr/local
BINDIR=${PREFIX}/bin

.PHONY: build clean install uninstall

build: ${BIN}

clean:
	-rm ${BIN}

install: ${BIN}
	@mkdir -p ${BINDIR}
	cp ${BIN} ${BINDIR}/${BIN}

uninstall:
	rm ${BINDIR}/${BIN}
