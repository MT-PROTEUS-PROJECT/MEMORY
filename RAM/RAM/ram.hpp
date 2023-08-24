#pragma once

#include <model.hpp>
#include <pin.hpp>

#include <array>
#include <optional>
#include <string>
#include <ranges>
#include <regex>

class ram final : public vsm::model
{
public:
    static constexpr DWORD MODEL_KEY = 0x00000001;

private:
    static constexpr uint8_t ADDR_SIZE = 4;
    static constexpr uint8_t MEM_SIZE = 16;

    static constexpr const char *FILE_NAME = "out.bin";

    using mem = std::array<uint32_t, MEM_SIZE>;
    using in_pins = std::array<vsm::pin, ADDR_SIZE>;
    using out_pins = std::array<vsm::pin, ADDR_SIZE + 28>;

private:
    in_pins _pins_D;
    in_pins _pins_A;
    out_pins _pins_C;

    mem _data;

public:
    std::optional<mem> load_file();

public:
    ram();

    VOID setup(IINSTANCE *instance, IDSIMCKT *dsim) override;

    VOID simulate(ABSTIME time, DSIMMODES mode) override;

    ~ram() = default;
};
