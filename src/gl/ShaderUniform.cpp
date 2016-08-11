//
// Created by Matt Blair on 4/26/16.
//
#include "ShaderUniform.hpp"

namespace stock {

ShaderUniform::ShaderUniform(int value)
    : m_type(ValueType::SCALAR_INT), m_scalarInt(value) {}

ShaderUniform::ShaderUniform(float value)
    : m_type(ValueType::SCALAR_FLOAT), m_scalarFloat(value) {}

ShaderUniform::ShaderUniform(int* array, size_t count)
    : m_type(ValueType::ARRAY_INT), m_vectorInt(array, array + count) {}

ShaderUniform::ShaderUniform(float* array, size_t count)
    : m_type(ValueType::ARRAY_FLOAT), m_vectorFloat(array, array + count) {}

ShaderUniform::~ShaderUniform() {
    switch (m_type) {
        case ValueType::ARRAY_INT:
            m_vectorInt.~vector<int>();
            break;
        case ValueType::ARRAY_FLOAT:
            m_vectorFloat.~vector<float>();
            break;
        default:
            break;
    }
}

size_t ShaderUniform::count() {
    switch (m_type) {
        case ValueType::SCALAR_INT:
        case ValueType::SCALAR_FLOAT:
            return 1;
        case ValueType::ARRAY_INT:
            return m_vectorInt.size();
        case ValueType::ARRAY_FLOAT:
            return m_vectorFloat.size();
    }
}

bool ShaderUniform::equals(const ShaderUniform& other) {

    if (m_type != other.m_type) {
        return false;
    }

    switch (m_type) {
        case ValueType::SCALAR_INT:
            return (m_scalarInt == other.m_scalarInt);
        case ValueType::SCALAR_FLOAT:
            return (m_scalarFloat == other.m_scalarFloat);
        case ValueType::ARRAY_INT:
            return m_vectorInt == other.m_vectorInt;
        case ValueType::ARRAY_FLOAT:
            return m_vectorFloat == other.m_vectorFloat;
    }
}

}
