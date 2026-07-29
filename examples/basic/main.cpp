#include <memory>

#include "beaver/app/application.h"
#include "beaver/app/entry.h"

class BasicApplication : public bvr::app::Application {
public:
    BasicApplication(const bvr::app::ApplicationDescriptor& desc) : bvr::app::Application(desc) {}
};

std::unique_ptr<bvr::app::Application> bvr::app::create_application(bvr::app::CommandLineArgs& args) {
    bvr::app::ApplicationDescriptor desc = {
        .window_desc = {
            .title = "Basic Application",
            .width = 680,
            .height = 400,
            .resizable = false,
        },
        .vsync = true,
    };

    return std::make_unique<BasicApplication>(desc);
}
