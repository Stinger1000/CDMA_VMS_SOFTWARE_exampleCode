#ifndef CODINGRACH_H
#define CODINGRACH_H

#include <QByteArray>
#include <QList>
#include <cmath>
#include <memory>
#include <vector>

class CodingRach {
public:
    CodingRach();
    ~CodingRach();

    uint32_t         CrcCalc(const QList<uint8_t>& mes, const uint16_t size);
    const QByteArray AddNull(const QByteArray data);
    const QByteArray AddTailBits(const QByteArray data);
    const QByteArray EncodingConvolutional(const QByteArray data);
    const QByteArray DublicateSymbol(const QByteArray data);
    const QByteArray Interleaver(const QByteArray data);

    const QByteArray QListToVectorBool(const QList<uint8_t> data, const uint16_t size);
    const QByteArray VectorBoolToVectorUint(const QByteArray data);

    const std::vector<QByteArray> ParsPacket(const QList<uint8_t>& data, const uint16_t size);

private:
    std::vector<uint8_t> m_memory { 0, 0, 0, 0, 0, 0, 0, 0 };

    constexpr static int m_interleaver_block_size { 576 }; //384
    constexpr static int m_interleaver_m { 5 };            //6
    constexpr static int m_interleaver_J { 18 };           //6
    constexpr static int m_add_null { 96 };                //96

    constexpr int reverse(int value)
    {
        int ans { 0 };
        for (auto i = 4; i >= 0; i--) {
            ans |= (value & 1) << i;
            value >>= 1;
        }
        return ans;
    }

    constexpr int location(int i)
    {
        int loc = 2 * 2 * 2 * 2 * 2 * (i % m_interleaver_J) + reverse(i / m_interleaver_J);
        return loc;
    }

    int* mas_location()
    {
        int* mas = new int[m_interleaver_block_size];
        for (auto i = 0; i < m_interleaver_block_size; i++) {
            mas[i] = location(i);
        }
        return mas;
    }

    const int* m_mas_location = mas_location();

    const std::vector<uint8_t> MASKS {
        0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
    };
};

#endif // CODINGRACH_H
