PYTHON ?= python
VANETZA_DIR = extern/vanetza
VEINS_DIR = extern/veins

$(VEINS_DIR)/src/Makefile: $(VEINS_DIR)/.gitrepo
	cd $(VEINS_DIR) && $(PYTHON) configure
	$(MAKE) -C $(VEINS_DIR)/src depend

veins: $(VEINS_DIR)/src/Makefile
	$(MAKE) -C $(VEINS_DIR)

$(VANETZA_DIR)/build:
	mkdir $(VANETZA_DIR)/build

$(VANETZA_DIR)/build/CMakeCache.txt: $(VANETZA_DIR)/build
	cd $< && cmake -DCMAKE_BUILD_TYPE=Release ..

vanetza: $(VANETZA_DIR)/build/CMakeCache.txt
	cmake --build $(VANETZA_DIR)/build
