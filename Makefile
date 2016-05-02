PYTHON ?= python
VANETZA_DIR = extern/vanetza
VEINS_DIR = extern/veins

$(VEINS_DIR)/src/Makefile:
	cd $(VEINS_DIR) && $(PYTHON) configure

veins: $(VEINS_DIR)/src/Makefile
	$(MAKE) -C $(VEINS_DIR)/src depend
	$(MAKE) -C $(VEINS_DIR)

$(VANETZA_DIR)/build:
	mkdir $(VANETZA_DIR)/build

$(VANETZA_DIR)/build/CMakeCache.txt: $(VANETZA_DIR)/build
	cd $< && cmake -DCMAKE_BUILD_TYPE=Release ..

vanetza: $(VANETZA_DIR)/build/CMakeCache.txt
	cmake --build $(VANETZA_DIR)/build
