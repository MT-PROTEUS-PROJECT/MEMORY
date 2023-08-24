#include "ram.hpp"

#include <fstream>
#include <bitset>

ram::ram()
{
    static_assert(sizeof(mem::value_type) * 8 >= CMD_SIZE);
    std::ranges::fill(_data, 0);
}

std::optional<ram::mem> ram::load_file()
{
    ram::mem data;
    static std::regex reg(R"(([0-9a-fA-F]+)\t([0-9a-fA-F]+))");

    std::ifstream file(FILE_NAME, std::ios::in | std::ios::binary);
    if (!file)
        return {};

    std::string line;
    std::smatch reg_match;
    size_t i = 0;
    for (; std::getline(file, line); )
    {
        if (!std::regex_match(line, reg_match, reg))
            continue;

        auto addr = std::stoi(reg_match[1], nullptr, 16);
        addr <<= (sizeof(addr) * 8 - ADDR_SIZE);
        addr >>= (sizeof(addr) * 8 - ADDR_SIZE);

        data[i] = addr;
        data[i] <<= 28;
        data[i] += std::stoi(reg_match[2], nullptr, 16);

        if (++i; i >= MEM_SIZE)
            break;
    }

    return data;
}

VOID ram::setup(IINSTANCE *instance, IDSIMCKT *dsim)
{
    _instance = instance;

    vsm::model::init_pins(_instance, _pins_D, "D");
    vsm::model::init_pins(_instance, _pins_A, "A");
    vsm::model::init_pins(_instance, _pins_C, "C");
    WR.init(_instance, "WR");

    auto file_data = load_file();
    if (file_data.has_value())
        std::swap(_data, file_data.value());
}

VOID ram::simulate(ABSTIME time, DSIMMODES mode)
{
    auto addr = vsm::model::make_number(_pins_A);
    if (WR->isposedge())
    {
        _data[addr] = vsm::model::make_number(_pins_D);
    }
    
    std::bitset<CMD_SIZE> bits(_data[addr]);
    for (size_t i = 0; i < _pins_C.size(); ++i)
    {
        _pins_C[i].set(time, 50000, (bits[i] == 1 ? SHI : SLO));
    }
}

extern "C"
{
    IDSIMMODEL __declspec(dllexport) *createdsimmodel(CHAR *device, ILICENCESERVER *license_server)
    {
        return license_server->authorize(ram::MODEL_KEY) ? new ram : nullptr;
    }

    VOID __declspec(dllexport) deletedsimmodel(IDSIMMODEL *model)
    {
        delete static_cast<ram *>(model);
    }
}
