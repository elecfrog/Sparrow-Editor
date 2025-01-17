//
// Created by sgsdi on 2023/2/14.
//

#ifndef SPARROW_SHADER_H
#define SPARROW_SHADER_H
#include "string"
#include "glm/glm.hpp"
#include <type_traits>
#include <glad/glad.h>
#include <memory>
#include <unordered_map>
#include <utility>

#include "Light.h"
#include "StorageBuffer.h"

namespace SPW
{
    class Shader
    {
    public:
        virtual ~Shader() = default;
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
        template<class T>
        void SetUniformValue(std::string str,T value)
        {
            if constexpr (std::is_same<T,bool>::value)
                setBool(str,value);
            else if constexpr (std::is_same<T,int>::value)
                setInt(str,value);
            else if constexpr (std::is_same<T,float>::value)
                setFloat(str,value);
            else if constexpr (std::is_same<T,glm::vec2>::value)
                setVec2(str,value);
            else if constexpr(std::is_same<T,glm::vec3>::value)
                setVec3(str,value);
            else if constexpr (std::is_same<T,glm::vec4>::value)
                setVec4(str,value);
            else if constexpr (std::is_same<T,glm::mat2>::value)
                setMat2(str,value);
            else if constexpr (std::is_same<T,glm::mat3>::value)
                setMat3(str,value);
            else if constexpr (std::is_same<T,glm::mat4>::value)
                setMat4(str,value);
        }

    public:
        virtual void setBool(const std::string& name, bool value) const = 0;
        virtual void setInt(const std::string& name, int value) const = 0;
        virtual void setFloat(const std::string& name, float value) const = 0;
        virtual void setVec2(const std::string& name, glm::vec2 value) const = 0;
        virtual void setVec3(const std::string& name, glm::vec3 value) const = 0;
        virtual void setVec4(const std::string& name, glm::vec4 value) const = 0;
        virtual void setMat2(const std::string& name, glm::mat2 mat) const = 0;
        virtual void setMat3(const std::string& name, glm::mat3 mat) const = 0;
        virtual void setMat4(const std::string& name, glm::mat4 mat) const = 0;
        virtual void setDLight(unsigned int idx, const DLight &light) const = 0;
        virtual void setPLight(unsigned int idx, const PLight &light) const = 0;
        virtual void setStorageBuffer(std::shared_ptr<StorageBuffer> uniformBuffer) = 0;
        virtual void bindTex2D(unsigned int slot,unsigned int texid) const = 0;
        virtual void bindTexArray(unsigned int slot,unsigned int texid) const = 0;
    };

    struct ShaderHandle {
        std::string name;
        std::string vertex_shader_path;
        std::string frag_shader_path;
        ShaderHandle()= default;
        ShaderHandle(std::string aName,
                              std::string vert,
                              std::string frag):
            name(std::move(aName)), vertex_shader_path(std::move(vert)), frag_shader_path(std::move(frag))
        {}
        [[nodiscard]] bool isValid() const {
            return !vertex_shader_path.empty() && !frag_shader_path.empty();
        }

        bool operator==(const ShaderHandle& other) const
        {
            return vertex_shader_path == other.vertex_shader_path &&
                   frag_shader_path == other.frag_shader_path;
        }
    };

    struct ShaderHash {
        std::size_t operator()(const ShaderHandle& handle) const
        {
            return std::hash<std::string>()(handle.vertex_shader_path) ^
                   (std::hash<std::string>()(handle.frag_shader_path) << 1);
        }
    };

    using ShaderTable = std::unordered_map<ShaderHandle, std::shared_ptr<Shader>, ShaderHash>;
}


#endif //SPARROW_SHADER_H
