PYTHON ?= python
PYTHON2 ?= python2
INET_DIR = extern/inet
SIMULTE_DIR = extern/simulte
VANETZA_DIR = extern/vanetza
VANETZA_BUILD_TYPE ?= Release
VANETZA_BUILD_DIR ?= $(VANETZA_DIR)/build
VEINS_DIR = extern/veins

all: inet vanetza veins

clean:
	-$(MAKE) -C $(INET_DIR) cleanall
	-$(MAKE) -C $(VEINS_DIR) cleanall
	-rm -rf $(VANETZA_BUILD_DIR)

$(INET_DIR)/.oppfeaturestate: $(INET_DIR)/.oppfeatures
	cd $(INET_DIR); $(PYTHON) bin/inet_featuretool repair

$(INET_DIR)/src/Makefile: $(INET_DIR)/.oppfeaturestate
	$(MAKE) -C $(INET_DIR) makefiles

inet: $(INET_DIR)/src/Makefile
	$(MAKE) -C $(INET_DIR)/src

$(SIMULTE_DIR)/src/Makefile: $(SIMULTE_DIR)/Version
	$(MAKE) -C $(SIMULTE_DIR) makefiles INET_PROJ=$(INET_DIR)
	$(MAKE) -C $(SIMULTE_DIR)/src depend

simulte: $(SIMULTE_DIR)/src/Makefile
	$(MAKE) -C $(SIMULTE_DIR)/src

$(VEINS_DIR)/src/Makefile: $(VEINS_DIR)/configure
	cd $(VEINS_DIR); $(PYTHON2) configure
	$(MAKE) -C $(VEINS_DIR)/src depend

veins: $(VEINS_DIR)/src/Makefile
	$(MAKE) -C $(VEINS_DIR)

$(VANETZA_BUILD_DIR):
	mkdir -p $(VANETZA_BUILD_DIR)

$(VANETZA_BUILD_DIR)/Makefile: $(VANETZA_BUILD_DIR)
	cd $<; cmake -DCMAKE_BUILD_TYPE=$(VANETZA_BUILD_TYPE) -DBUILD_SHARED_LIBS=ON ..

vanetza: $(VANETZA_BUILD_DIR)/Makefile
	$(MAKE) -C $(VANETZA_BUILD_DIR)
