#pragma once

#include <memory>

#include "beaver/app/application.h"
#include "beaver/core/platform.h"

#if BEAVER_PLATFORM_WEB
    #include <emscripten.h>
#endif

extern std::unique_ptr<bvr::app::Application> bvr::app::create_application(bvr::app::CommandLineArgs& args);

// NOLINTNEXTLINE(misc-definitions-in-headers)
int main(int argc, char** argv) {
    bvr::app::CommandLineArgs args = {
        .count = argc,
        .args = argv,
    };

    std::unique_ptr<bvr::app::Application> app = bvr::app::create_application(args);
    app->run();

#if BEAVER_PLATFORM_WEB
    emscripten_exit_with_live_runtime();
#else
    app.reset();
#endif
}
