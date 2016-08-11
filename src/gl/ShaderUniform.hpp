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
        SCALAR_INT,
        SCALAR_FLOAT,
        ARRAY_INT,
        ARRAY_FLOAT,
    };

    union {
        int m_scalarInt = 0;
        float m_scalarFloat;
        std::vector<int> m_vectorInt;
        std::vector<float> m_vectorFloat;
    };

    ValueType m_type = ValueType::SCALAR_INT;

    friend class ShaderProgram;
};

} // namespace stock
