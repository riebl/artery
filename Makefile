PYTHON ?= python

veins/src/Makefile:
	cd veins && $(PYTHON) configure

veins: veins/src/Makefile
	$(MAKE) -C veins
