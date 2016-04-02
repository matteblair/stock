//
// Created by Matt Blair on 4/26/16.
//
#include "ShaderUniform.hpp"

namespace stock {

ShaderUniform::ShaderUniform(int value)
    : type(ValueType::scalar_int), scalar_int(value) {}

ShaderUniform::ShaderUniform(float value)
    : type(ValueType::scalar_float), scalar_float(value) {}

ShaderUniform::ShaderUniform(int* array, size_t count)
    : type(ValueType::array_int), vector_int(array, array + count) {}

ShaderUniform::ShaderUniform(float* array, size_t count)
    : type(ValueType::array_float), vector_float(array, array + count) {}

ShaderUniform::~ShaderUniform() {
    switch (type) {
        case ValueType::array_int:
            vector_int.~vector<int>();
            break;
        case ValueType::array_float:
            vector_float.~vector<float>();
            break;
        default:
            break;
    }
}

size_t ShaderUniform::count() {
    switch (type) {
        case ValueType::scalar_int:
        case ValueType::scalar_float:
            return 1;
        case ValueType::array_int:
            return vector_int.size();
        case ValueType::array_float:
            return vector_float.size();
    }
}

bool ShaderUniform::equals(const ShaderUniform& other) {

    if (type != other.type) {
        return false;
    }

    switch (type) {
        case ValueType::scalar_int:
            return (scalar_int == other.scalar_int);
        case ValueType::scalar_float:
            return (scalar_float == other.scalar_float);
        case ValueType::array_int:
            return vector_int == other.vector_int;
        case ValueType::array_float:
            return vector_float == other.vector_float;
    }
}

}
