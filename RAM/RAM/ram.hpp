#pragma once

#include <model.hpp>
#include <pin.hpp>

#include <array>
#include <optional>
#include <string>
#include <ranges>
#include <regex>

class RAM final : public vsm::model
{
public:
    static constexpr DWORD MODEL_KEY = 0x00000001;

private:
    static constexpr uint8_t CMD_SIZE = 42;
    static constexpr uint16_t MEM_SIZE = 1024;

    static constexpr uint8_t ADDR_BC1_SIZE = 4;
    static constexpr uint8_t ADDR_RAM_SIZE = 10;
    static constexpr uint8_t WORD_SIZE = 8;
    static constexpr uint8_t CONTROL_SIZE = 9;
    static constexpr uint8_t CA_SIZE = 4;
    static constexpr uint8_t SHIFT_SIZE = 2;
    static constexpr const char *FILE_NAME = "out.bin";

    using mem = std::array<uint64_t, MEM_SIZE>;
    using word_pins = std::array<vsm::pin, WORD_SIZE>;
    using addr_pins = std::array<vsm::pin, ADDR_BC1_SIZE>;
    using control_pins = std::array<vsm::pin, CONTROL_SIZE>;
    using ca_pins = std::array<vsm::pin, CA_SIZE>;
    using shift = std::array<vsm::pin, SHIFT_SIZE>;
    using ram_pins = std::array<vsm::pin, ADDR_RAM_SIZE>;

private:
    //INPUT
    addr_pins _pins_IA;
    addr_pins _pins_IB;
    word_pins _pins_ID;
    control_pins _pins_II;
    shift _pins_IM;
    ca_pins _pins_ICA;
    ram_pins _pins_IAR;
    ram_pins _pins_IT;
    
    vsm::pin IC0;
    vsm::pin WR;

    //OUTPUT
    addr_pins _pins_A;
    addr_pins _pins_B;
    word_pins _pins_D;
    control_pins _pins_I;
    shift _pins_M;
    ca_pins _pins_CA;
    ram_pins _pins_AR;

    vsm::pin C0;

    mem _data;

public:
    std::optional<mem> load_file();

public:
    RAM();

    VOID setup(IINSTANCE *instance, IDSIMCKT *dsim) override;

    VOID simulate(ABSTIME time, DSIMMODES mode) override;

    ~RAM() = default;
};
