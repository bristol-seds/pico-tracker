# Makefile for design notes

all: fbri-notes.pdf Parts.md

%.pdf: %.tex
	pdflatex $<

# Formats Farnell Order numbers as hyperlinks
#
.PHONY: Parts.md
Parts.md:
	@echo "Adding farnell links to parts list..."
	@sed 's/| \([0-9]\{7\}\) |/| [\1](http:\/\/uk.farnell.com\/jsp\/search\/browse.jsp;jsessionid=0?N=0\&Ntk=partnumbers\&Ntt=*\1*) |/g' Parts > Parts.md
