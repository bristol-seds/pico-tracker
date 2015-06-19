all: Parts.md schematic board

Parts.md: parts
	ruby ../partslist/parts.rb

schematic: bristol_longshot.sch.pdf bristol_longshot.sch.png
board: bristol_longshot.brd.pdf bristol_longshot.brd.png

.PHONY: clean
clean:
	rm Parts.md
	rm bristol_longshot.sch.pdf
	rm bristol_longshot.sch.png
	rm bristol_longshot.brd.pdf
	rm bristol_longshot.brd.png

#
# PNG from .sch
#
%.sch.png: %.sch
	rm -f $@
	eagle -C "export image $@ 300; display all; quit" $<
#
# PDF from .sch
# 1 page lanscape
#
%.sch.pdf: %.sch
	rm -f $@
	eagle -C "print -1 landscape file $@; quit" $<
#
# PNG from .brd
# Ratsnest, maximum dpi
#
%.brd.png: %.brd
	rm -f $@
	eagle -C "ratsnest; export image $@ 1800; display all; quit" $<
#
# PDF from .brd
# -1 limits to one page
#
%.brd.pdf: %.brd
	rm -f $@
	eagle -C "ratsnest; print 100 -1 file $@; quit" $<
