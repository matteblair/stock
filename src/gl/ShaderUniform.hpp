//
// Created by Matt Blair on 4/2/16.
//
#pragma once

#include <string>
#include <vector>

namespace stock {

class UniformLocation {

public:
    UniformLocation(const std::string& name) : name(name) {}

private:
    const std::string name;

    mutable int location = -1;
    mutable int generation = -1;

    friend class ShaderProgram;
};

class ShaderUniform {

public:
    ShaderUniform(int value);
    ShaderUniform(float value);
    ShaderUniform(int* array, size_t count);
    ShaderUniform(float* array, size_t count);

    ~ShaderUniform();

    size_t count();

    bool equals(const ShaderUniform& _other);

private:
    enum class ValueType : uint8_t {
        scalar_int,
        scalar_float,
        array_int,
        array_float,
    };

    union {
        int scalar_int = 0;
        float scalar_float;
        std::vector<int> vector_int;
        std::vector<float> vector_float;
    };

    ValueType type = ValueType::scalar_int;

    friend class ShaderProgram;
};

} // namespace stock
