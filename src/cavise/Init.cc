// STD
#include <mutex>

// plog
#include <plog/Log.h>
#include <plog/Severity.h>
#include <plog/Initializers/RollingFileInitializer.h>

// local
#include <cavise/Init.h>

namespace {

    static struct State {
        std::once_flag init;
    } state;

}

void cavise::init() {
    std::call_once(state.init, []() {
        plog::init(plog::Severity::debug, "log.txt", 1024 * 1024, 2);
		PLOG(plog::info) << "initalized logging, recording API calls";
    });
}