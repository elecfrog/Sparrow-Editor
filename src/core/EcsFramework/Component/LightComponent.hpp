//
// Created by ZhangHao on 2023/2/20.
//
#pragma once
#include "ComponentI.h"

#include <glm/glm.hpp>

namespace SPW {
    // light types
    enum LightType {
        PointLightType,
        DirectionalLightType
    };

    // store light info
    // 1. type
    // 2. intensity
    // 3. color
    class LightComponent : public ComponentI {
    public:
        LightComponent() = delete;
        explicit LightComponent(LightType type) : lightType(type) { }

        float constant = 1;
        float linear = 1;
        float quadratic = 1;  

        glm::vec3 ambient = {1, 1, 1};
        glm::vec3 diffuse = {1, 1, 1};
        glm::vec3 specular = {1, 1, 1};

        [[nodiscard]] LightType getType() const {
            return lightType;
        };
    private:
        LightType lightType;
    };
}