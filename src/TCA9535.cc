#include "TCA9535.hpp"

TCA9535::TCA9535(I2C_HandleTypeDef* i2c_handle, uint8_t address_7bit)
    : i2c(i2c_handle), address(address_7bit << 1) {}

std::expected<TCA9535, HAL_StatusTypeDef>
TCA9535::configure(I2C_HandleTypeDef* i2c_handle, uint8_t address_7bit) {
    TCA9535 device(i2c_handle, address_7bit);

    auto s0 = device.write_reg(REG_CONFIG_0, 0xFF); // all input
    if (!s0) return std::unexpected(s0.error());

    auto s1 = device.write_reg(REG_CONFIG_1, 0xFF);
    if (!s1) return std::unexpected(s1.error());

    return device;
}

std::expected<uint8_t, HAL_StatusTypeDef> TCA9535::read_reg(uint8_t reg) {
    uint8_t val = 0;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(i2c, address, reg,
                                                I2C_MEMADD_SIZE_8BIT, &val, 1, HAL_MAX_DELAY);
    if (status != HAL_OK)
        return std::unexpected(status);
    return val;
}

std::expected<void, HAL_StatusTypeDef> TCA9535::write_reg(uint8_t reg, uint8_t value) {
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(i2c, address, reg,
                                                 I2C_MEMADD_SIZE_8BIT, &value, 1, HAL_MAX_DELAY);
    if (status != HAL_OK)
        return std::unexpected(status);
    return {};
}

std::expected<void, HAL_StatusTypeDef> TCA9535::config_pin(Port port, Pin pin, Mode mode) {
    uint8_t reg = (port == Port::PORT_0) ? REG_CONFIG_0 : REG_CONFIG_1;
    return transact(reg, [=](uint8_t cfg) {
        return (mode == Mode::INPUT)
            ? (cfg | static_cast<uint8_t>(pin))
            : (cfg & ~static_cast<uint8_t>(pin));
    });
}

std::expected<void, HAL_StatusTypeDef> TCA9535::write_pin(Port port, Pin pin, GPIO_PinState state) {
    uint8_t reg = (port == Port::PORT_0) ? REG_OUTPUT_0 : REG_OUTPUT_1;
    return transact(reg, [=](uint8_t val) {
        return (state == GPIO_PIN_SET)
            ? (val | static_cast<uint8_t>(pin))
            : (val & ~static_cast<uint8_t>(pin));
    });
}

std::expected<void, HAL_StatusTypeDef> TCA9535::toggle_pin(Port port, Pin pin) {
    uint8_t reg = (port == Port::PORT_0) ? REG_OUTPUT_0 : REG_OUTPUT_1;
    return transact(reg, [=](uint8_t val) {
        return val ^ static_cast<uint8_t>(pin);
    });
}

std::expected<void, HAL_StatusTypeDef> TCA9535::write_port(Port port, uint8_t value) {
    uint8_t reg = (port == Port::PORT_0) ? REG_OUTPUT_0 : REG_OUTPUT_1;
    return write_reg(reg, value);
}

std::expected<uint8_t, HAL_StatusTypeDef> TCA9535::read_port(Port port) {
    uint8_t reg = (port == Port::PORT_0) ? REG_INPUT_0 : REG_INPUT_1;
    return read_reg(reg);
}

std::expected<GPIO_PinState, HAL_StatusTypeDef> TCA9535::read_pin(Port port, Pin pin) {
    auto val = read_port(port);
    if (!val)
        return std::unexpected(val.error());

    return ((*val & static_cast<uint8_t>(pin)) != 0) ? GPIO_PIN_SET : GPIO_PIN_RESET;
}

