PYTHON ?= python
VEINS_DIR = extern/veins

$(VEINS_DIR)/src/Makefile:
	cd $(VEINS_DIR) && $(PYTHON) configure

veins: $(VEINS_DIR)/src/Makefile
	$(MAKE) -C $(VEINS_DIR)
