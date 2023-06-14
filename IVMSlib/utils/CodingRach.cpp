#include "CodingRach.h"
#include <QFile>
#include <QTextStream>

CodingRach::CodingRach()
{
}

CodingRach::~CodingRach()
{
    delete m_mas_location;
}

/**
 * @brief Calculation crc for mes
 * @param[in] mes - message
 * @param[in] size - message size
 *
 * @return CRC
 */
uint32_t CodingRach::CrcCalc(const QList<uint8_t>& mes, const uint16_t size)
{

    bool flag { 0 };

    uint8_t        num_byte { 0 };
    uint8_t        num_bit { 8 };
    uint32_t       crc     = 0x3FFFFFFF;
    const uint32_t polinom = 0x38E74301;

    for (auto i = 0; i < size; i++) {
        num_bit--;

        flag = bool(crc & 0x1) ^ bool((mes.value(num_byte) >> (num_bit)) & 0x1);

        crc = static_cast<uint32_t>((crc >> 1) & 0x1FFFFFFF);
        crc = static_cast<uint32_t>((crc ^ (flag * polinom)) & 0x3FFFFFFF);

        if (num_bit == 0) {
            num_byte++;
            num_bit = 8;
        }
    }
    crc = crc ^ 0x3FFFFFFF;
    uint32_t crc_rev { 0 };
    for (auto i = 1; i <= 30; i++) {
        crc_rev = crc_rev | ((crc & 0x1) << (30 - i));
        crc     = crc >> 1;
    }

    return crc_rev;
}

/**
 * @brief Add null bits in mes
 * @param[in] data - message
 *
 * @return message with null bits
 */
const QByteArray CodingRach::AddNull(const QByteArray data)
{
    QByteArray out_data(m_add_null, false);
    int        size_sum = data.size() + m_add_null;
    out_data.resize(size_sum);
    for (int i = out_data.size(), j = 0; i < size_sum; i++, j++) {
        out_data[i] = data[j];
    }
    return out_data;
}

/**
 * @brief Add null tail bits in mes
 * @param[in] data - message
 *
 * @return message with tail bits
 */
const QByteArray CodingRach::AddTailBits(const QByteArray data)
{
    QByteArray out_data = data;
    out_data.resize(out_data.size() + 8);
    for (auto i = data.size(); i < out_data.size(); i++) {
        out_data[i] = false;
    }
    return out_data;
}

/**
 * @brief Encoder for message
 * @param[in] data - message
 *
 * @return encoding message
 */
const QByteArray CodingRach::EncodingConvolutional(const QByteArray data)
{
    m_memory = { 0, 0, 0, 0, 0, 0, 0, 0 };

    QByteArray out;
    int        size_out = data.size(); //data.size() + m_memory.size()
    out.resize(size_out * 3);

    QByteArray data_in = data;
    data_in.resize(size_out);

    auto j = 0;
    for (auto i = 0; i < size_out; i++) {

        out[j] = data_in[i] ^ m_memory[7] ^ m_memory[6] ^ m_memory[5] ^ m_memory[4] ^ m_memory[2] ^ m_memory[1];
        j++;
        out[j] = data_in[i] ^ m_memory[7] ^ m_memory[6] ^ m_memory[3] ^ m_memory[2] ^ m_memory[0];
        j++;
        out[j] = data_in[i] ^ m_memory[7] ^ m_memory[4] ^ m_memory[1] ^ m_memory[0];
        j++;

        m_memory[7] = m_memory[6];
        m_memory[6] = m_memory[5];
        m_memory[5] = m_memory[4];
        m_memory[4] = m_memory[3];
        m_memory[3] = m_memory[2];
        m_memory[2] = m_memory[1];
        m_memory[1] = m_memory[0];
        m_memory[0] = data_in[i];
    }
    return out;
}

/**
 * @brief Decimation sym in message
 * @param[in] data - mes
 *
 * @return mes with dublication sym
 */
const QByteArray CodingRach::DublicateSymbol(const QByteArray data)
{
    QByteArray data_out;
    data_out.resize(data.size() * 2);
    for (auto i = 0; i < data.size(); i++) {
        data_out[i * 2]     = data.at(i);
        data_out[i * 2 + 1] = data.at(i);
    }
    return data_out;
}

/**
 * @brief Interleaver message
 * @param[in] data - message
 *
 * @return interliving message
 */
const QByteArray CodingRach::Interleaver(const QByteArray data)
{
    QByteArray out_data;
    out_data.resize(data.size());
    auto num_block = data.size() / m_interleaver_block_size;
    for (auto i = 0; i < num_block; i++) {
        for (auto j = 0; j < m_interleaver_block_size; j++) {
            out_data[j + (i * m_interleaver_block_size)] = data[m_mas_location[j]];
        }
    }
    return out_data;
}

/**
 * @brief Convert List ti vector<bool>
 * @param[in] data - message
 * @param[in] size - message size
 *
 * @return message : vector<bool>
 */
const QByteArray CodingRach::QListToVectorBool(const QList<uint8_t> data, const uint16_t size)
{
    QByteArray out_data;
    out_data.resize(size);
    for (auto i = 0; i < out_data.size(); i++) {
        out_data[i] = (data.at(i / 8) >> (7 - (i % 8))) & 0x01;
    }
    return out_data;
}

/**
 * @brief Convert vector<bool> ti vector<uint8>
 * @param[in] data - message
 * @param[in] size - message size
 *
 * @return message : vector<uint8>
 */
const QByteArray CodingRach::VectorBoolToVectorUint(const QByteArray data)
{
    QByteArray out_data;
    if (data.size() % 8)
        return out_data;

    out_data.resize(data.size() / 8);
    auto FormUint8 = [&](const uint32_t begin) {
        uint8_t out_byte { 0 };
        uint8_t sdv { 7 };
        for (auto i = begin; i < begin + 8; i++) {
            out_byte = out_byte | ((static_cast<uint8_t>(data[i]) << sdv) & MASKS[sdv]);
            sdv--;
        }
        return out_byte;
    };

    uint32_t bit_data = 0;
    for (auto i = 0; i < out_data.size(); i++) {
        out_data[i] = FormUint8(bit_data);
        bit_data += 8;
    }
    return out_data;
}

/**
 * @brief Parser for message rach
 * @param[in] data - message
 * @param[in] size - message size
 * @return none
 */
const std::vector<QByteArray> CodingRach::ParsPacket(const QList<uint8_t>& data, const uint16_t size)
{
    std::vector<QByteArray> out_data;
    QList<uint8_t>          data_crc = data;
    data_crc.push_front(static_cast<uint8_t>(data_crc.size() + 4)); //length
    uint32_t crc    = CrcCalc(data_crc, size + 8);
    data_crc.last() = static_cast<uint8_t>(data.last() | ((crc >> 24) & 0x3F));
    data_crc.push_back(static_cast<uint8_t>((crc >> 16) & 0xFF));
    data_crc.push_back(static_cast<uint8_t>((crc >> 8) & 0xFF));
    data_crc.push_back(static_cast<uint8_t>(crc & 0xFF));

    std::vector<QList<uint8_t>> frames;
    uint32_t                    num_byte { 0 };
    while (data_crc.size() % 11)
        data_crc.push_back(0x00);
    for (auto i = 0; i < data_crc.size() / 11; i++) {
        QList<uint8_t> frame;
        for (auto num = num_byte; num < num_byte + 11; num++)
            frame.push_back(data_crc.at(num));
        num_byte += 11;
        frames.push_back(frame);
    }

    for (auto i = 0; i < int(frames.size()); i++) {
        auto vec_bits = QListToVectorBool(frames.at(i), 88);
        vec_bits      = AddTailBits(vec_bits);

        vec_bits = EncodingConvolutional(vec_bits);

        vec_bits = DublicateSymbol(vec_bits);
        vec_bits = Interleaver(vec_bits);

        out_data.push_back(VectorBoolToVectorUint(vec_bits));
    }
    return out_data;
}
