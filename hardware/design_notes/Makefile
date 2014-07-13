# Makefile for design notes

all: fbri-notes.pdf Parts.md

%.pdf: %.tex
	pdflatex $<

# Turns any farnell part numbers (specified as "Farnell | [xxxxxx] |")
# into hyperlinks.
#
.PHONY: Parts.md
Parts.md:
	sed -i 's/[Ff]arnell.*|.*\[\([0-9]*\)\].*|/Farnell | [\1](http:\/\/uk.farnell.com\/jsp\/search\/browse.jsp;jsessionid=0?N=0\&Ntk=partnumbers\&Ntt=*\1*) |/g' Parts.md
