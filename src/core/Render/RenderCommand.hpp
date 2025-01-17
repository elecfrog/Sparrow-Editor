//
// Created by ZhangHao on 2023/2/19.
//

#ifndef SPARROW_RENDERCOMMAND_HPP
#define SPARROW_RENDERCOMMAND_HPP

#include <memory>
#include <functional>

namespace SPW {
    template <class API>
    class RenderCommand {
    public:
        RenderCommand() = default;

        // APIName from RenderBackEndI
        template<typename ...Args>
        explicit RenderCommand(void(API::*MenFn)(Args ...), Args ...args) {
            apiCaller = [=](std::shared_ptr<API> &api){
                ((*api).*MenFn)(static_cast<Args>(args) ...);
            };
        }
        // move
        template<typename ...Args>
        explicit RenderCommand(RenderCommand &&other) {
            apiCaller = other.apiCaller;
        }

        // copy
        template<typename ...Args>
        explicit RenderCommand(const RenderCommand &other) {
            apiCaller = other.apiCaller;
        }

        // execute with an api
        void execute(std::shared_ptr<API> &api) {
            apiCaller(api);
        }

    private:
        std::function<void(std::shared_ptr<API> &api)> apiCaller;
    };
}

#endif //SPARROW_RENDERCOMMAND_HPP
