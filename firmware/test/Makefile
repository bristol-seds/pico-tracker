#
# Makes new test cases
#

ECHO	:= echo
SED	:= sed

#
#
#
.PHONY: new
new:
ifdef name
	@$(ECHO)
	@$(ECHO) "Creating $(name)_tc..."
	@$(SED) "s/\[template\]/$(name)/g" template/template.h > tc/$(name).h
	@$(SED) "s/\[template\]/$(name)/g" template/template.py > tc/$(name).py
	@$(ECHO) "Done"
else
	@$(ECHO) "Please specify a name for the test case!! Like 'make name=my_tc'"
endif
