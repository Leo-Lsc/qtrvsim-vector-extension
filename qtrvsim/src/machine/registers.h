#ifndef REGISTERS_H
#define REGISTERS_H

#include "memory/address.h"
#include "register_value.h"
#include "simulator_exception.h"

#include <QObject>
#include <array>
#include <cstdint>

namespace machine {

/**
 * General-purpose register count
 */
constexpr size_t REGISTER_COUNT = 32;

/**
 * General-purpose register identifier
 */
class RegisterId {
public:
    inline constexpr RegisterId(uint8_t value); // NOLINT(google-explicit-constructor)
    inline RegisterId();

    constexpr operator size_t() const { return data; }; // NOLINT(google-explicit-constructor)

private:
    uint8_t data;
};

// class V_RegisterId {
// public:
//     inline constexpr V_RegisterId(uint8_t value);
//     inline V_RegisterId();

//     constexpr operator size_t() const { return data; };

// private:
//     uint8_t data;
// };

inline constexpr RegisterId::RegisterId(uint8_t value) : data(value) {
    // Bounds on the id are checked at creation time and its value is immutable.
    // Therefore, all check at when used are redundant.
    // Main advantage is, that possible errors will appear when creating the
    // value, which is probably close to the bug source.

    SANITY_ASSERT(
        value < REGISTER_COUNT, QString("Trying to create register id for out-of-bounds register ")
                                    + QString::number(data));
}
inline RegisterId::RegisterId() : RegisterId(0) {}

// inline constexpr V_RegisterId::V_RegisterId(uint8_t value) : data(value) {
//     SANITY_ASSERT(
//         value < REGISTER_COUNT, QString("Trying to create register id for out-of-bounds register ")
//                                     + QString::number(data));
// }
// inline V_RegisterId::V_RegisterId() : V_RegisterId(0) {}

inline RegisterId operator"" _reg(unsigned long long value) {
    return { static_cast<uint8_t>(value) };
}

// // 使用类似 10_vreg 的语法来创建 V_RegisterId 对象
// inline V_RegisterId operator"" _vreg(unsigned long long value) {
//     return { static_cast<uint8_t>(value) };
// }

/**
 * Register file
 */
class Registers : public QObject {
    Q_OBJECT
public:
    Registers();
    Registers(const Registers &);

    Address read_pc() const;        // Return current value of program counter
    void write_pc(Address address); // Absolute jump in program counter

    RegisterValue read_gp(RegisterId reg) const;        // Read general-purpose
                                                        // register
    void write_gp(RegisterId reg, RegisterValue value); // Write general-purpose
                                                        // register

    std::array<RegisterValue, 32> read_vr(RegisterId reg) const;        
    void write_vr(RegisterId reg, RegisterValue value, uint32_t offset);

    bool operator==(const Registers &c) const;
    bool operator!=(const Registers &c) const;

    void reset(); // Reset all values to zero (except pc)

signals:
    void pc_update(Address val);
    void gp_update(RegisterId reg, RegisterValue val);
    void gp_read(RegisterId reg, RegisterValue val) const;

private:
    /**
     * General purpose registers
     *
     * Zero register is always zero, but is allocated to avoid off-by-one.
     * Getters and setters will never try to read or write zero register.
     */
    std::array<RegisterValue, REGISTER_COUNT> gp {};
    Address pc {}; // program counter

    // 每个向量寄存器包含32个32位整数
    std::array<std::array<RegisterValue, 32>, REGISTER_COUNT> vr {};
};

// /**
//  * Vector Register file
//  */
// class V_Registers : public QObject {
//     Q_OBJECT
// public:
//     V_Registers();
//     V_Registers(const V_Registers &);

//     Address read_pc() const;        // Return current value of program counter
//     void write_pc(Address address); // Absolute jump in program counter



//     bool operator==(const V_Registers &c) const;
//     bool operator!=(const V_Registers &c) const;

//     void reset(); // Reset all values to zero (except pc)

// signals:
//     void pc_update(Address val);
//     void vr_update(V_RegisterId reg, RegisterValue value, uint8_t offset);
//     void vr_read(V_RegisterId reg, std::array<RegisterValue, 32> value_array) const;

// private:

//     Address pc {}; // program counter
// };

} // namespace machine

Q_DECLARE_METATYPE(machine::Registers)

#endif // REGISTERS_H
