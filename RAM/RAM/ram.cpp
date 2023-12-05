#include "ram.hpp"

#include <fstream>
#include <bitset>
#include <algorithm>
#include <filesystem>

RAM::RAM()
{
    static_assert(sizeof(mem::value_type) * 8 >= CMD_SIZE);
    std::fill(_data.begin(), _data.end(), 0);
}

std::optional<RAM::mem> RAM::load_file()
{
    mem data;
    std::fill(data.begin(), data.end(), 0);

    static std::regex reg(R"(([0-9a-fA-F]+)\t([0-9a-fA-F]+)\t([0-9a-fA-F]+)[\r]*)");
    std::filesystem::directory_iterator it;
    std::string path = std::filesystem::current_path().string();
    try
    {
        it = std::filesystem::directory_iterator(path);
    }
    catch (const std::exception &)
    {
        _instance->log((CHAR *)("MEMORY: failed to open dir: " + path).data());
        return {};
    }

    for (const auto& dirEntry : it)
    {
        if (dirEntry.path().extension() != FILE_NAME_EXT)
            continue;

        std::ifstream file(dirEntry.path().string(), std::ios::in | std::ios::binary);
        if (!file)
            continue;

        std::string line;
        std::smatch reg_match;
        size_t i = 0;
        for (; std::getline(file, line); )
        {
            if (!std::regex_match(line, reg_match, reg))
                continue;

            auto addr = std::stoul(reg_match[2]);

            auto index = std::stoul(reg_match[1]);
            data[index] = addr;
            data[index] <<= 32;
            data[index] += std::stoul(reg_match[3]);

            if (++i; i >= MEM_SIZE)
                break;
        }

        _instance->log((CHAR *)("MEMORY: Loaded file - " + dirEntry.path().string()).data());
        return data;
    }
    return {};
}

VOID RAM::setup(IINSTANCE* instance, IDSIMCKT* dsim)
{
    _instance = instance;

    // INPUT
    vsm::model::init_pins(_instance, _pins_ID, "ID");
    vsm::model::init_pins(_instance, _pins_IB, "IB");
    vsm::model::init_pins(_instance, _pins_IA, "IA");
    vsm::model::init_pins(_instance, _pins_II, "II");
    vsm::model::init_pins(_instance, _pins_ICA, "ICA");
    vsm::model::init_pins(_instance, _pins_IAR, "IAR");
    vsm::model::init_pins(_instance, _pins_IT, "IT");
    vsm::model::init_pins(_instance, _pins_IM, "IM");

    IC0.init(_instance, "IC0");
    WR.init(_instance, "WR");

    // OUTPUT
    vsm::model::init_pins(_instance, _pins_D, "D");
    vsm::model::init_pins(_instance, _pins_B, "B");
    vsm::model::init_pins(_instance, _pins_A, "A");
    vsm::model::init_pins(_instance, _pins_I, "I");
    vsm::model::init_pins(_instance, _pins_CA, "CA");
    vsm::model::init_pins(_instance, _pins_AR, "AR");
    vsm::model::init_pins(_instance, _pins_M, "M");

    C0.init(_instance, "C0");

    auto file_data = load_file();
    if (file_data.has_value())
        std::swap(_data, file_data.value());
}

VOID RAM::simulate(ABSTIME time, DSIMMODES mode)
{
    auto addr = vsm::model::make_number(_pins_IT);
    if (WR->isposedge())
    {
        _data[addr] = vsm::model::make_number(_pins_IAR);
        _data[addr] <<= 4;
        _data[addr] += vsm::model::make_number(_pins_ICA);
        _data[addr] <<= 1;
        _data[addr] += _pins_M[1]->isactive();
        _data[addr] <<= 3;
        _data[addr] += vsm::model::make_number(std::views::counted(_pins_II.begin() + 6, 3));
        _data[addr] <<= 1;
        _data[addr] += _pins_M[0]->isactive();
        _data[addr] <<= 3;
        _data[addr] += vsm::model::make_number(std::views::counted(_pins_II.begin(), 3));
        _data[addr] <<= 1;
        _data[addr] += C0->isactive();
        _data[addr] <<= 3;
        _data[addr] += vsm::model::make_number(std::views::counted(_pins_II.begin() + 3, 3));
        _data[addr] <<= ADDR_BC1_SIZE;
        _data[addr] += vsm::model::make_number(_pins_IA);
        _data[addr] <<= ADDR_BC1_SIZE;
        _data[addr] += vsm::model::make_number(_pins_IB);
        _data[addr] <<= WORD_SIZE;
        _data[addr] += vsm::model::make_number(_pins_ID);
    }

    std::bitset<CMD_SIZE> bits(_data[addr]);
    size_t i = 0;
    size_t j = 0;
    for (j = 0; j < _pins_D.size(); ++j, ++i)
    {
        _pins_D[j].set(time, 500, bits.test(i) ? SHI : SLO);
    }
    for (j = 0; j < _pins_B.size(); ++j, ++i)
    {
        _pins_B[j].set(time, 500, bits.test(i) ? SHI : SLO);
    }
    for (j = 0; j < _pins_A.size(); ++j, ++i)
    {
        _pins_A[j].set(time, 500, bits.test(i) ? SHI : SLO);
    }
    for (j = 0; j < 3; ++j, ++i)
    {
        _pins_I[3 + j].set(time, 500, bits.test(i) ? SHI : SLO);
    }
    C0.set(time, 500, bits.test(i++) ? SHI : SLO);
    for (j = 0; j < 3; ++j, ++i)
    {
        _pins_I[j].set(time, 500, bits.test(i) ? SHI : SLO);
    }
    _pins_M[0].set(time, 500, bits.test(i++) ? SHI : SLO);
    for (j = 0; j < 3; ++j, ++i)
    {
        _pins_I[6 + j].set(time, 500, bits.test(i) ? SHI : SLO);
    }
    _pins_M[1].set(time, 500, bits.test(i++) ? SHI : SLO);
    for (j = 0; j < _pins_CA.size(); ++j, ++i)
    {
        _pins_CA[j].set(time, 500, bits.test(i) ? SHI : SLO);
    }
    for (j = 0; j < _pins_AR.size(); ++j, ++i)
    {
        _pins_AR[j].set(time, 500, bits.test(i) ? SHI : SLO);
    }
}

extern "C"
{
    IDSIMMODEL __declspec(dllexport)* createdsimmodel(CHAR* device, ILICENCESERVER* license_server)
    {
        return license_server->authorize(RAM::MODEL_KEY) ? new RAM : nullptr;
    }

    VOID __declspec(dllexport) deletedsimmodel(IDSIMMODEL* model)
    {
        delete static_cast<RAM*>(model);
    }
}