PYTHON ?= python
INET_DIR = extern/inet
INET_DISABLE_FEATURES ?= packetdrill SCTP SCTP_examples
VANETZA_DIR = extern/vanetza
VANETZA_BUILD_TYPE ?= Release
VANETZA_BUILD_DIR ?= $(VANETZA_DIR)/build
VEINS_DIR = extern/veins

all: inet vanetza veins

clean:
	-$(MAKE) -C $(INET_DIR) cleanall
	-$(MAKE) -C $(VEINS_DIR) cleanall
	-rm -rf $(VANETZA_BUILD_DIR)

$(INET_DIR)/src/Makefile: $(INET_DIR)/.gitrepo
	cd $(INET_DIR); $(PYTHON) inet_featuretool disable $(INET_DISABLE_FEATURES)
	$(MAKE) -C $(INET_DIR) makefiles

inet: $(INET_DIR)/src/Makefile
	$(MAKE) -C $(INET_DIR)/src

$(VEINS_DIR)/src/Makefile: $(VEINS_DIR)/.gitrepo
	cd $(VEINS_DIR); $(PYTHON) configure
	$(MAKE) -C $(VEINS_DIR)/src depend

veins: $(VEINS_DIR)/src/Makefile
	$(MAKE) -C $(VEINS_DIR)

$(VANETZA_BUILD_DIR):
	mkdir -p $(VANETZA_BUILD_DIR)

$(VANETZA_BUILD_DIR)/Makefile: $(VANETZA_BUILD_DIR)
	cd $<; cmake -DCMAKE_BUILD_TYPE=$(VANETZA_BUILD_TYPE) -DBUILD_SHARED_LIBS=ON ..

vanetza: $(VANETZA_BUILD_DIR)/Makefile
	$(MAKE) -C $(VANETZA_BUILD_DIR)
