//
// Created by Matt Blair on 4/26/16.
//
#include "ShaderUniform.hpp"
#include <string.h> // For memcmp and memcpy.

namespace stock {

ShaderUniform::ShaderUniform() {}

ShaderUniform::~ShaderUniform() {
    deleteArrays();
}

ShaderUniform::ShaderUniform(const ShaderUniform& other)
    : m_type(other.m_type), m_count(other.m_count) {
    switch (m_type) {
        case ValueType::SCALAR_INT: m_scalarInt = other.m_scalarInt; break;
        case ValueType::SCALAR_FLOAT: m_scalarFloat = other.m_scalarFloat; break;
        case ValueType::ARRAY_INT:
            m_arrayInt = new int[m_count];
            memcpy(m_arrayInt, other.m_arrayInt, arrayByteCount());
            break;
        case ValueType::ARRAY_FLOAT:
            m_arrayFloat = new float[m_count];
            memcpy(m_arrayFloat, other.m_arrayFloat, arrayByteCount());
            break;
    }
}

ShaderUniform::ShaderUniform(ShaderUniform&& other)
    : m_type(other.m_type), m_count(other.m_count) {
    switch (m_type) {
        case ValueType::SCALAR_INT: m_scalarInt = other.m_scalarInt; break;
        case ValueType::SCALAR_FLOAT:m_scalarFloat = other.m_scalarFloat; break;
        case ValueType::ARRAY_INT:
            m_arrayInt = other.m_arrayInt;
            other.m_arrayInt = nullptr;
            break;
        case ValueType::ARRAY_FLOAT:
            m_arrayFloat = other.m_arrayFloat;
            other.m_arrayFloat = nullptr;
            break;
    }
}

bool ShaderUniform::update(int value) {
    bool changed = (m_type != ValueType::SCALAR_INT || m_scalarInt != value);
    if (changed) {
        deleteArrays();
        m_type = ValueType::SCALAR_INT;
        m_scalarInt = value;
        m_count = 0;
    }
    return changed;
}

bool ShaderUniform::update(float value) {
    bool changed = (m_type != ValueType::SCALAR_FLOAT || m_scalarFloat != value);
    if (changed) {
        deleteArrays();
        m_type = ValueType::SCALAR_FLOAT;
        m_scalarFloat = value;
        m_count = 0;
    }
    return changed;
}

bool ShaderUniform::update(int* array, size_t count) {
    bool changedType = (m_type != ValueType::ARRAY_INT || m_count != count);
    if (changedType) {
        deleteArrays();
        m_arrayInt = new int[count];
        m_count = count;
    }
    bool changed = changedType || memcmp(m_arrayInt, array, arrayByteCount()) != 0;
    if (changed) {
        memcpy(m_arrayInt, array, arrayByteCount());
    }
    return changed;
}

bool ShaderUniform::update(float* array, size_t count) {
    bool changedType = (m_type != ValueType::ARRAY_FLOAT || m_count != count);
    if (changedType) {
        deleteArrays();
        m_arrayFloat = new float[count];
        m_count = count;
    }
    bool changed = changedType || memcmp(m_arrayFloat, array, arrayByteCount()) != 0;
    if (changed) {
        memcpy(m_arrayFloat, array, arrayByteCount());
    }
    return changed;
}

size_t ShaderUniform::count() {
    return m_count;
}

void ShaderUniform::deleteArrays() {
    switch (m_type) {
        case ValueType::ARRAY_INT:
            if (m_arrayInt != nullptr) { delete m_arrayInt; }
            break;
        case ValueType::ARRAY_FLOAT:
            if (m_arrayFloat != nullptr) { delete m_arrayFloat; }
            break;
        default:
            break;
    }
}

size_t ShaderUniform::arrayByteCount() {
    switch (m_type) {
        case ValueType::SCALAR_INT: return 0;
        case ValueType::SCALAR_FLOAT: return 0;
        case ValueType::ARRAY_INT: return m_count * sizeof(m_arrayInt[0]);
        case ValueType::ARRAY_FLOAT: return m_count * sizeof(m_arrayFloat[0]);
    }
}

} // namespace stock
