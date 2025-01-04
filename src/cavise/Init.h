#pragma once

/*
 Entrypoint for CAVISE submodule, initializes static library state
 and prepares static state for consequent API calls. 
*/

#define CAVISE_STUB() \
    PLOG(plog::debug) << "Local STUB: " << __PRETTY_FUNCTION__

namespace cavise {

    void init();

}