#ifndef TCA9535_HPP
#define TCA9535_HPP

#include "stm32f1xx_hal.h"
#include <cstdint>
#include <expected>
#include <type_traits>

class TCA9535 {
public:
    enum class Port : uint8_t {
        PORT_0 = 0,
        PORT_1 = 1
    };

    enum class Pin : uint8_t {
        PIN_0 = 0x01,
        PIN_1 = 0x02,
        PIN_2 = 0x04,
        PIN_3 = 0x08,
        PIN_4 = 0x10,
        PIN_5 = 0x20,
        PIN_6 = 0x40,
        PIN_7 = 0x80
    };

    enum class Mode : uint8_t {
        INPUT  = 1,
        OUTPUT = 0
    };

    static constexpr uint8_t REG_INPUT_0    = 0x00;
    static constexpr uint8_t REG_INPUT_1    = 0x01;
    static constexpr uint8_t REG_OUTPUT_0   = 0x02;
    static constexpr uint8_t REG_OUTPUT_1   = 0x03;
    static constexpr uint8_t REG_POLARITY_0 = 0x04;
    static constexpr uint8_t REG_POLARITY_1 = 0x05;
    static constexpr uint8_t REG_CONFIG_0   = 0x06;
    static constexpr uint8_t REG_CONFIG_1   = 0x07;

    TCA9535(I2C_HandleTypeDef* i2c_handle, uint8_t address_7bit);

    std::expected<void, HAL_StatusTypeDef> config_pin(Port port, Pin pin, Mode mode);
    std::expected<void, HAL_StatusTypeDef> write_pin(Port port, Pin pin, GPIO_PinState state);
    std::expected<void, HAL_StatusTypeDef> toggle_pin(Port port, Pin pin);
    std::expected<void, HAL_StatusTypeDef> write_port(Port port, uint8_t value);
    std::expected<GPIO_PinState, HAL_StatusTypeDef> read_pin(Port port, Pin pin);
    std::expected<uint8_t, HAL_StatusTypeDef> read_port(Port port);

    template<uint8_t REG, typename Func>
    std::expected<void, HAL_StatusTypeDef> transact(Func&& func);

private:
    I2C_HandleTypeDef* i2c;
    uint8_t address;

    std::expected<uint8_t, HAL_StatusTypeDef> read_reg(uint8_t reg);
    std::expected<void, HAL_StatusTypeDef> write_reg(uint8_t reg, uint8_t value);
};

// Inline template implementation
template<uint8_t REG, typename Func>
std::expected<void, HAL_StatusTypeDef> TCA9535::transact(Func&& func) {
    static_assert(std::is_invocable_r_v<uint8_t, Func, uint8_t>,
        "Func must be callable with uint8_t and return uint8_t");

    auto result = read_reg(REG);
    if (!result)
        return std::unexpected(result.error());

    uint8_t updated = func(*result);
    return write_reg(REG, updated);
}

#endif // TCA9535_HPP

