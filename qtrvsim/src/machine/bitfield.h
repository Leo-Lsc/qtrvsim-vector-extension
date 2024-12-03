/**
 * Utility to parse and encode binary encoded fields.
 *
 * Use BitField if the field is guaranteed to be continuous.
 * Use SplitBitField if the field is not guaranteed to be continuous.
 *
 * @file
 */
/**
 * 工具类：用于解析和编码二进制表示的字段。
 *
 * 如果字段是连续的，使用 BitField。
 * 如果字段不连续，使用 SplitBitField。
 *
 * @file
 */

#ifndef QTRVSIM_BITFIELD_H
#define QTRVSIM_BITFIELD_H

#include "common/containers/cvector.h"

#include <cstdint>

struct BitField {
    uint8_t count;
    uint8_t offset;

    // 从输入的值 val 中提取出从 offset 位开始的 count 个二进制位
    template<typename T>
    [[nodiscard]] T decode(T val) const {
        return (val >> offset) & (((uint64_t)1 << count) - 1);
    }

    /** 
     * 将给定的值 val 编码到目标位置。
     * 先确保 val 的低 count 位有效，清除其他无关位。
     * 然后将这些有效位左移到指定的 offset 位置。
     */ 
    template<typename T>
    [[nodiscard]] T encode(T val) const {
        return ((val & (((uint64_t)1 << count) - 1)) << offset);
    }

    [[nodiscard]] uint64_t mask() const {
        return (((uint64_t)1 << count) - 1) << offset;
    }
};

// SplitBitField 结构体表示一个由多个不连续字段组成的位字段
template<size_t MAX_FIELD_PARTS>
struct SplitBitField {
    cvector<BitField, MAX_FIELD_PARTS> fields; // 存储多个 BitField 对象的容器
    size_t shift = 0; // 解码时的额外偏移量

    // 获取容器的开始迭代器
    [[nodiscard]] typename decltype(fields)::const_iterator begin() const {
        return fields.cbegin();
    }

    // 获取容器的结束迭代器
    [[nodiscard]] typename decltype(fields)::const_iterator end() const { return fields.cend(); }
    
    // 解码：将 32 位的指令拆分成多个字段
    [[nodiscard]] uint32_t decode(uint32_t ins) const {
        uint32_t ret = 0;
        size_t offset = 0;
        for (BitField field : *this) {
            ret |= field.decode(ins) << offset;
            offset += field.count;
        }
        return ret << shift;
    }

    // 编码：将立即数编码成 32 位指令
    [[nodiscard]] uint32_t encode(uint32_t imm) const {
        uint32_t ret = 0;
        imm >>= shift;
        for (BitField field : *this) {
            ret |= field.encode(imm);
            imm >>= field.count;
        }
        return ret;
    }
};

#endif // QTRVSIM_BITFIELD_H
