ASN1C?=asn1c
ASN1C_ARGS=-fcompound-names -Werror -gen-PER -pdu=auto
GEN_DIR=gen
ASN_SOURCES=cdd_ts102894-2v1.2.1.asn1 cam_en302637-2v1.3.2.asn1 denm_en302637-3v1.2.2.asn1

ASN1C_VERSION:=$(shell ${ASN1C} -version 2>&1 | grep -Eo '[0-9]\.[0-9]\.[0-9]+')
ASN1C_VERSION_TUPLE=$(subst ., ,${ASN1C_VERSION})
ASN1C_VERSION_MAJOR=$(word 1,${ASN1C_VERSION_TUPLE})
ASN1C_VERSION_MINOR=$(word 2,${ASN1C_VERSION_TUPLE})
ASN1C_VERSION_REV=$(word 3,${ASN1C_VERSION_TUPLE})

# asn1c prior 0.9.26 has to be called with -fnative-types flag
ASN1C_NATIVE_TYPES:=$(shell echo "$(ASN1C_VERSION_MAJOR) <= 0 && $(ASN1C_VERSION_MINOR) <= 9 && $(ASN1C_VERSION_REV) < 26" | bc)
ifeq ($(ASN1C_NATIVE_TYPES),1)
    ASN1C_ARGS+=-fnative-types
endif

all: $(ASN_SOURCES)
	cd $(GEN_DIR); $(ASN1C) $(ASN1C_ARGS) $(addprefix ../,$(filter-out $(GEN_DIR),$^))

clean:
	rm  $(GEN_DIR)/*

.PHONY: clean
