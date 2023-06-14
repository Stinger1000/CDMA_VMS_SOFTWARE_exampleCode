#include "SmsController.h"

SmsController::SmsController(QObject* parent)
    : QObject(parent)
{
}

/**
 * @brief Clear class for next mes
 * @param none
 *
 * @return none
 */
void SmsController::Clear()
{
    m_sms.char_i.clear();
    m_sms.priority.clear();
    m_sms.num_field.clear();
    m_sms.reply_seq.clear();
    m_sms.digit_mode.clear();
    m_sms.identifier.clear();
    m_sms.message_id.clear();
    m_sms.sub_char_i.clear();
    m_sms.number_mode.clear();
    m_sms.message_type.clear();
    m_sms.msg_encoding.clear();
    m_sms.parameter_id.clear();
    m_sms.parameter_len.clear();
    m_sms.alert_priority.clear();
    m_sms.subparameter_id.clear();
    m_sms.subparameter_len.clear();
    m_sms.error_class.clear();
}

/**
 * @brief Data structure handler for sms from base station
 * @param[in] sid - the SID number of BS
 * @param[in] elem - struct sms
 *
 * @return none
 */
void SmsController::NewSms(int sid, int nid,
    dataBurstMesModel elem)
{
    Clear();
    if (elem.sms.empty()) {
        return;
    }
    m_sms.sms_msg_type = elem.sms.at(0) & 0xFF;

    auto num_of_bits = (elem.num_fields - 1) * 8;

    if (m_sms.sms_msg_type == 0 || m_sms.sms_msg_type == 2) {
        while (num_of_bits > 16) {
            if (!m_toEnd) {
                m_sms.parameter_id.push_back(elem.sms.at(1) & 0xFF);
                m_sms.parameter_len.push_back(elem.sms.at(2) & 0xFF);
            }

            switch (m_sms.parameter_id.last()) {
            case 0:
                m_sms.identifier.push_back(
                    ((elem.sms.at(3) << 8) & 0xFF00) | ((elem.sms.at(4) & 0xFF)));
                shift_left(elem.sms, elem.sms.size(), 32);
                num_of_bits = num_of_bits - 32;
                break;

            case 2: {
                m_sms.digit_mode.push_back((elem.sms.at(3) >> 7) & 0x1);
                m_sms.number_mode.push_back((elem.sms.at(3) >> 6) & 0x1);
                m_sms.num_field.push_back(((elem.sms.at(3) << 2) & 0xFC) | ((elem.sms.at(4) >> 6) & 0x3));
                shift_left(elem.sms, elem.sms.size(), 10);
                QVector<uint8_t> word;

                for (auto i = 0, g = 0; i < m_sms.num_field.last();
                     i++, g++) {
                    if (uint8_t((elem.sms.at(g + 3) >> 4) & 0xF) != 0x0A) {
                        word.push_back((elem.sms.at(g + 3) >> 4) & 0xF);
                    } else {
                        word.push_back(0x00);
                    }
                    i++;

                    if (i == m_sms.num_field.last()) {
                        break;
                    }

                    if (uint8_t((elem.sms.at(g + 3)) & 0xF) != 0x0A) {
                        word.push_back((elem.sms.at(g + 3)) & 0xF);
                    } else {
                        word.push_back(0x00);
                    }
                }

                for (auto num : qAsConst(word))
                    m_sms.char_i.append(QString::number(num));

                if (m_sms.num_field.last() & 0x1) {
                    num_of_bits = num_of_bits - (28 + (m_sms.num_field.last() * 4));
                    shift_left(elem.sms, elem.sms.size(),
                        18 + (m_sms.num_field.last() * 4));
                } else {
                    num_of_bits = num_of_bits - (32 + (m_sms.num_field.last() * 4));
                    shift_left(elem.sms, elem.sms.size(),
                        22 + (m_sms.num_field.last() * 4));
                }

                break;
            }

            case 4: {
                m_sms.digit_mode.push_back((elem.sms.at(3) >> 7) & 0x1);
                m_sms.number_mode.push_back((elem.sms.at(3) >> 6) & 0x1);
                m_sms.num_field.push_back(((elem.sms.at(3) << 2) & 0xFC) | ((elem.sms.at(4) >> 6) & 0x3));
                shift_left(elem.sms, elem.sms.size(), 10);
                QVector<uint8_t> word;

                for (auto i = 0, g = 0; i < m_sms.num_field.last();
                     i++, g++) {
                    if (uint8_t((elem.sms.at(g + 3) >> 4) & 0xF) != 0x0A) {
                        word.push_back((elem.sms.at(g + 3) >> 4) & 0xF);
                    } else {
                        word.push_back(0x00);
                    }
                    i++;

                    if (i == m_sms.num_field.last()) {
                        break;
                    }

                    if (uint8_t((elem.sms.at(g + 3)) & 0xF) != 0x0A) {
                        word.push_back((elem.sms.at(g + 3)) & 0xF);
                    } else {
                        word.push_back(0x00);
                    }
                }

                for (auto num : qAsConst(word))
                    m_sms.char_i.append(QString::number(num));

                if (m_sms.num_field.last() & 0x1) {
                    num_of_bits = num_of_bits - (28 + (m_sms.num_field.last() * 4));
                    shift_left(elem.sms, elem.sms.size(),
                        18 + (m_sms.num_field.last() * 4));
                } else {
                    num_of_bits = num_of_bits - (32 + (m_sms.num_field.last() * 4));
                    shift_left(elem.sms, elem.sms.size(),
                        22 + (m_sms.num_field.last() * 4));
                }

                break;
            }

            case 6:
                m_sms.reply_seq.push_back((elem.sms.at(3) >> 2) & 0x3F);
                num_of_bits = num_of_bits - 24;
                shift_left(elem.sms, elem.sms.size(), 24);
                break;

            case 7:
                m_sms.reply_seq.push_back((elem.sms.at(3) >> 2) & 0x3F);
                m_sms.error_class.push_back((elem.sms.at(3) & 0x3));
                num_of_bits = num_of_bits - 26;
                shift_left(elem.sms, elem.sms.size(), 26);

                if (m_sms.error_class.last() == 0) {
                    emit SMSacknowledge();
                }

                break;

            case 8: {
                if (!m_toEnd) {
                    remove_list(elem.sms, 0, 3);
                }
                m_toEnd = 1;

                m_sms.subparameter_id.push_back((elem.sms.at(0) & 0xFF));
                m_sms.subparameter_len.push_back((elem.sms.at(1) & 0xFF));

                switch (m_sms.subparameter_id.last()) {
                case 0:

                    m_sms.message_type.push_back((elem.sms.at(2) >> 4) & 0xF);
                    m_sms.message_id.push_back(
                        ((elem.sms.at(2) << 12) & 0xF000) | ((elem.sms.at(3) << 4) & 0xFF0) | ((elem.sms.at(4) >> 4) & 0xF));

                    num_of_bits = num_of_bits - 40;
                    shift_left(elem.sms, elem.sms.size(), 40);
                    break;

                case 1: {
                    m_sms.msg_encoding.push_back((elem.sms.at(2) >> 3) & 0x1F);
                    m_sms.num_field.push_back(
                        ((elem.sms.at(2) << 5) & 0xE0) | ((elem.sms.at(3) >> 3) & 0x1F));

                    QString sub_word;
                    shift_left(elem.sms, elem.sms.size(), 29);

                    auto rep_push = m_sms.num_field.last();

                    if (m_sms.msg_encoding.last() != 4)
                        while (rep_push) {
                            if (rep_push >= 8) {
                                sub_word.push_back((elem.sms.at(0) >> 1) & 0x7F);
                                sub_word.push_back(
                                    ((elem.sms.at(0) << 6) & 0x40) | ((elem.sms.at(1) >> 2) & 0x3F));
                                sub_word.push_back(
                                    ((elem.sms.at(1) << 5) & 0x60) | ((elem.sms.at(2) >> 3) & 0x1F));
                                sub_word.push_back(
                                    ((elem.sms.at(2) << 4) & 0x70) | ((elem.sms.at(3) >> 4) & 0xF));
                                sub_word.push_back(
                                    ((elem.sms.at(3) << 3) & 0x78) | ((elem.sms.at(4) >> 5) & 0x7));
                                sub_word.push_back(
                                    ((elem.sms.at(4) << 2) & 0x7C) | ((elem.sms.at(5) >> 6) & 0x3));
                                sub_word.push_back(
                                    ((elem.sms.at(5) << 1) & 0x7E) | ((elem.sms.at(6) >> 7) & 0x1));
                                sub_word.push_back(elem.sms.at(6) & 0x7F);

                                rep_push = rep_push - 8;
                                remove_list(elem.sms, 0, 7);
                            } else {
                                sub_word.push_back((elem.sms.at(0) >> 1) & 0x7F);

                                rep_push = rep_push - 1;
                                shift_left(elem.sms, elem.sms.size(), 7);
                            }
                        }

                    if (m_sms.msg_encoding.last() == 4)
                        for (auto i = 0; i < (rep_push / 2); i++) {
                            sub_word.push_back((elem.sms.at(0) << 8) | elem.sms.at(1));
                            shift_left(elem.sms, elem.sms.size(), 16);
                        }

                    int del = 0;
                    if (m_sms.msg_encoding.last() != 4)
                        del = ((m_sms.subparameter_len.last() * 8) - 13) - (m_sms.num_field.last() * 7);

                    if (m_sms.msg_encoding.last() == 4)
                        del = ((m_sms.subparameter_len.last() * 8) - 13) - (m_sms.num_field.last() * 8);

                    shift_left(elem.sms, elem.sms.size(), del);
                    m_sms.sub_char_i = sub_word;

                    num_of_bits = num_of_bits - ((m_sms.subparameter_len.last() * 8) + 16);

                    break;
                }

                case 2:
                    shift_left(elem.sms, elem.sms.size(), 24);
                    num_of_bits -= 24;
                    break;

                case 8:
                    m_sms.priority.push_back((elem.sms.at(2) >> 6) & 0x3);
                    num_of_bits = num_of_bits - 24;
                    shift_left(elem.sms, elem.sms.size(), 24);
                    break;

                case 9:
                    m_sms.privacy = (elem.sms.at(2) >> 6) & 0x3;
                    num_of_bits   = num_of_bits - 24;

                    shift_left(elem.sms, elem.sms.size(), 24);
                    break;

                case 12:
                    m_sms.alert_priority.push_back(
                        (elem.sms.at(2) >> 6) & 0x3);

                    num_of_bits = num_of_bits - 24;

                    shift_left(elem.sms, elem.sms.size(), 24);
                    break;

                default:
                    num_of_bits = 0;
                    break;
                }
                break;
            }
            default:
                num_of_bits = 0;
                break;
            }
        }

        QDateTime time = QDateTime::currentDateTime();
        bool      status_ack { false };

        if (!m_sms.error_class.isEmpty()) {
            if (!m_sms.error_class.last())
                status_ack = true;
        }

        if (m_sms.msg_encoding.isEmpty())
            m_sms.msg_encoding.push_back(0);
        emit SMSreceived(
            { sid, nid, uint64_t(elem.addr_fields.imsi_s2 * 10000000 + elem.addr_fields.imsi_s1),
                time.toString().toStdString(), m_sms.char_i.toStdString(), m_sms.sub_char_i.toStdString(), m_sms.msg_encoding.last(), status_ack, m_sms.reply_seq.last() });

        m_sms.char_i.clear();
        m_toEnd = 0;
    }
}

/**d
 * @brief Data structure handler for sms from mobile station
 * @param[in] sid - the SID number of BS
 * @param[in] elem - struct sms
 *
 * @return none
 */
void SmsController::NewSmsRach(int sid, int nid, uint64_t imsi,
    RAchDataBurstMesModel elem)
{
    Clear();
    m_sms.sms_msg_type = elem.sms.at(0) & 0xFF;

    auto num_of_bits = (elem.num_fields - 1) * 8;

    if (m_sms.sms_msg_type == 0 || m_sms.sms_msg_type == 2) {
        while (num_of_bits > 16) {
            if (!m_toEnd) {
                m_sms.parameter_id.push_back(elem.sms.at(1) & 0xFF);
                m_sms.parameter_len.push_back(elem.sms.at(2) & 0xFF);
            }

            switch (m_sms.parameter_id.last()) {
            case 0:
                m_sms.identifier.push_back(
                    ((elem.sms.at(3) << 8) & 0xFF00) | ((elem.sms.at(4) & 0xFF)));
                shift_left(elem.sms, elem.sms.size(), 32);
                num_of_bits = num_of_bits - 32;
                break;

            case 2: {
                m_sms.digit_mode.push_back((elem.sms.at(3) >> 7) & 0x1);
                m_sms.number_mode.push_back((elem.sms.at(3) >> 6) & 0x1);
                m_sms.num_field.push_back(((elem.sms.at(3) << 2) & 0xFC) | ((elem.sms.at(4) >> 6) & 0x3));
                shift_left(elem.sms, elem.sms.size(), 10);
                QVector<uint8_t> word;

                for (auto i = 0, g = 0; i < m_sms.num_field.last();
                     i++, g++) {
                    if (uint8_t((elem.sms.at(g + 3) >> 4) & 0xF) != 0x0A) {
                        word.push_back((elem.sms.at(g + 3) >> 4) & 0xF);
                    } else {
                        word.push_back(0x00);
                    }
                    i++;

                    if (i == m_sms.num_field.last()) {
                        break;
                    }

                    if (uint8_t((elem.sms.at(g + 3)) & 0xF) != 0x0A) {
                        word.push_back((elem.sms.at(g + 3)) & 0xF);
                    } else {
                        word.push_back(0x00);
                    }
                }

                for (auto num : qAsConst(word))
                    m_sms.char_i.append(QString::number(num));

                if (m_sms.num_field.last() & 0x1) {
                    num_of_bits = num_of_bits - (28 + (m_sms.num_field.last() * 4));
                    shift_left(elem.sms, elem.sms.size(),
                        18 + (m_sms.num_field.last() * 4));
                } else {
                    num_of_bits = num_of_bits - (32 + (m_sms.num_field.last() * 4));
                    shift_left(elem.sms, elem.sms.size(),
                        22 + (m_sms.num_field.last() * 4));
                }

                break;
            }

            case 4: {
                m_sms.digit_mode.push_back((elem.sms.at(3) >> 7) & 0x1);
                m_sms.number_mode.push_back((elem.sms.at(3) >> 6) & 0x1);
                m_sms.num_field.push_back(((elem.sms.at(3) << 2) & 0xFC) | ((elem.sms.at(4) >> 6) & 0x3));
                shift_left(elem.sms, elem.sms.size(), 10);
                QVector<uint8_t> word;

                for (auto i = 0, g = 0; i < m_sms.num_field.last();
                     i++, g++) {
                    if (uint8_t((elem.sms.at(g + 3) >> 4) & 0xF) != 0x0A) {
                        word.push_back((elem.sms.at(g + 3) >> 4) & 0xF);
                    } else {
                        word.push_back(0x00);
                    }
                    i++;

                    if (i == m_sms.num_field.last()) {
                        break;
                    }

                    if (uint8_t((elem.sms.at(g + 3)) & 0xF) != 0x0A) {
                        word.push_back((elem.sms.at(g + 3)) & 0xF);
                    } else {
                        word.push_back(0x00);
                    }
                }

                for (auto num : qAsConst(word))
                    m_sms.char_i.append(QString::number(num));

                if (m_sms.num_field.last() & 0x1) {
                    num_of_bits = num_of_bits - (28 + (m_sms.num_field.last() * 4));
                    shift_left(elem.sms, elem.sms.size(),
                        18 + (m_sms.num_field.last() * 4));
                } else {
                    num_of_bits = num_of_bits - (32 + (m_sms.num_field.last() * 4));
                    shift_left(elem.sms, elem.sms.size(),
                        22 + (m_sms.num_field.last() * 4));
                }

                break;
            }

            case 6:
                m_sms.reply_seq.push_back((elem.sms.at(3) >> 2) & 0x3F);
                num_of_bits = num_of_bits - 24;
                shift_left(elem.sms, elem.sms.size(), 24);
                break;

            case 7:
                m_sms.reply_seq.push_back((elem.sms.at(3) >> 2) & 0x3F);
                m_sms.error_class.push_back((elem.sms.at(3) & 0x3));
                num_of_bits = num_of_bits - 26;
                shift_left(elem.sms, elem.sms.size(), 26);
                break;

            case 8: {
                if (!m_toEnd) {
                    remove_list(elem.sms, 0, 3);
                }
                m_toEnd = 1;

                m_sms.subparameter_id.push_back((elem.sms.at(0) & 0xFF));
                m_sms.subparameter_len.push_back((elem.sms.at(1) & 0xFF));

                switch (m_sms.subparameter_id.last()) {
                case 0:

                    m_sms.message_type.push_back((elem.sms.at(2) >> 4) & 0xF);
                    m_sms.message_id.push_back(
                        ((elem.sms.at(2) << 12) & 0xF000) | ((elem.sms.at(3) << 4) & 0xFF0) | ((elem.sms.at(4) >> 4) & 0xF));

                    num_of_bits = num_of_bits - 40;
                    shift_left(elem.sms, elem.sms.size(), 40);
                    break;

                case 1: {
                    m_sms.msg_encoding.push_back((elem.sms.at(2) >> 3) & 0x1F);
                    m_sms.num_field.push_back(
                        ((elem.sms.at(2) << 5) & 0xE0) | ((elem.sms.at(3) >> 3) & 0x1F));

                    QString sub_word;
                    shift_left(elem.sms, elem.sms.size(), 29);

                    auto rep_push = m_sms.num_field.last();

                    QByteArray uni_mes;

                    if (m_sms.msg_encoding.last() == 4) {
                        for (auto i = 0; i < (rep_push * 2); i += 2) {
                            uni_mes.push_back(
                                uint8_t(elem.sms.at(i + 1)));
                            uni_mes.push_back(uint8_t(elem.sms.at(i)));
                        }
                        sub_word = QString::fromUtf16(
                            reinterpret_cast<const char16_t*>(
                                uni_mes.constData()),
                            rep_push);

                    } else {
                        while (rep_push) {
                            if (rep_push >= 8) {
                                sub_word.push_back(
                                    (elem.sms.at(0) >> 1) & 0x7F);
                                sub_word.push_back(
                                    ((elem.sms.at(0) << 6) & 0x40) | ((elem.sms.at(1) >> 2) & 0x3F));
                                sub_word.push_back(
                                    ((elem.sms.at(1) << 5) & 0x60) | ((elem.sms.at(2) >> 3) & 0x1F));
                                sub_word.push_back(
                                    ((elem.sms.at(2) << 4) & 0x70) | ((elem.sms.at(3) >> 4) & 0xF));
                                sub_word.push_back(
                                    ((elem.sms.at(3) << 3) & 0x78) | ((elem.sms.at(4) >> 5) & 0x7));
                                sub_word.push_back(
                                    ((elem.sms.at(4) << 2) & 0x7C) | ((elem.sms.at(5) >> 6) & 0x3));
                                sub_word.push_back(
                                    ((elem.sms.at(5) << 1) & 0x7E) | ((elem.sms.at(6) >> 7) & 0x1));
                                sub_word.push_back(elem.sms.at(6) & 0x7F);

                                rep_push = rep_push - 8;
                                remove_list(elem.sms, 0, 7);
                            } else {
                                sub_word.push_back(
                                    (elem.sms.at(0) >> 1) & 0x7F);

                                rep_push = rep_push - 1;
                                shift_left(elem.sms, elem.sms.size(),
                                    7);
                            }
                        }
                    }

                    auto del { 0 };

                    if (m_sms.msg_encoding.last() == 4) {
                        del = ((m_sms.subparameter_len.last() * 8) - 13);
                    } else {
                        del = ((m_sms.subparameter_len.last() * 8) - 13) - (m_sms.num_field.last() * 7);
                    }

                    shift_left(elem.sms, elem.sms.size(), del);
                    m_sms.sub_char_i = sub_word;

                    num_of_bits = num_of_bits - ((m_sms.subparameter_len.last() * 8) + 16);

                    break;
                }

                case 2:
                    shift_left(elem.sms, elem.sms.size(), 24);
                    num_of_bits -= 24;
                    break;

                case 8:
                    m_sms.priority.push_back((elem.sms.at(2) >> 6) & 0x3);
                    num_of_bits = num_of_bits - 24;
                    shift_left(elem.sms, elem.sms.size(), 24);
                    break;

                case 9:
                    m_sms.privacy = (elem.sms.at(2) >> 6) & 0x3;
                    num_of_bits   = num_of_bits - 24;

                    shift_left(elem.sms, elem.sms.size(), 24);
                    break;

                case 12:
                    m_sms.alert_priority.push_back(
                        (elem.sms.at(2) >> 6) & 0x3);

                    num_of_bits = num_of_bits - 24;

                    shift_left(elem.sms, elem.sms.size(), 24);
                    break;

                default:
                    num_of_bits = 0;
                    break;
                }
                break;
            }
            default:
                num_of_bits = 0;
                break;
            }
        }
        QDateTime time = QDateTime::currentDateTime();
        if (m_sms.sub_char_i.size()) //send if sms is have

            emit SMSreceived(
                { sid, nid, imsi, time.toString().toStdString(), m_sms.char_i.toStdString(), m_sms.sub_char_i.toStdString(), m_sms.msg_encoding.last() });
        m_sms.char_i.clear();
        m_toEnd = 0;
    }
}
