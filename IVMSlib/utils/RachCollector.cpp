#include "RachCollector.h"

RachCollector::RachCollector(const mobile_setting_t& config, const elementSync& sync, const uniTypes& pch)
    : m_sync(sync)
    , m_pch(pch)
    , m_mobile_config(config)
{
}

/**
 * @brief Get registration message rach
 * @param[in] model_lac - lac settings fieds
 * @param[in] model - layer3 settings field
 *
 * @return Registation message(data, size)
 */
std::pair<QList<uint8_t>, uint32_t> RachCollector::GetRegistrationMes(const lac_collector_model_t& model_lac,
    const registration_collector_model_t&                                                          model)
{
    QList<uint8_t> out_data;
    uint32_t       size { 0 };
    auto           lvl_3 = FormLvl3Registration(model);  //form lvl3
    auto           lac   = FormLacLvl(model_lac);        //form lac
    lac.InsertBits(lvl_3.GetList(), lvl_3.GetSizeBit()); //compound

    int sf = lac.GetSizeBit() % 8;
    if (sf > 2) {
        lac.InsertBits(0x00, 10 - sf);
    } else if (sf < 2) {
        lac.InsertBits(0x00, 2 - sf);
    }

    out_data = lac.GetList();
    size     = lac.GetSizeBit();

    return std::pair<QList<uint8_t>, uint32_t> { out_data, size };
}

/**
 * @brief Get Extended status message rach
 * @param[in] model_lac - lac settings fieds
 * @param[in] model - layer3 settings field
 *
 * @return Extended status message(data, size)
 */
std::pair<QList<uint8_t>, uint32_t> RachCollector::GetExtendedStatusMes(const lac_collector_model_t& model_lac, const extended_status_response_model_t& status)
{
    QList<uint8_t> out_data;
    uint32_t       size { 0 };
    auto           lvl_3 = FormLvl3ExtendedStatus(status); //form lvl3
    auto           lac   = FormLacLvl(model_lac);          //form lac
    lac.InsertBits(lvl_3.GetList(), lvl_3.GetSizeBit());   //compound

    int sf = lac.GetSizeBit() % 8;
    if (sf > 2) {
        lac.InsertBits(0x00, 10 - sf);
    } else if (sf < 2) {
        lac.InsertBits(0x00, 2 - sf);
    }

    out_data = lac.GetList();
    size     = lac.GetSizeBit();

    return std::pair<QList<uint8_t>, uint32_t> { out_data, size };
}

/**
 * @brief Get Data burs message rach
 * @param[in] model_lac - lac settings fieds
 * @param[in] model - layer3 settings field
 *
 * @return Data burst message(data, size)
 */
std::pair<QList<uint8_t>, uint32_t> RachCollector::GetDataBurstMes(const lac_collector_model_t& model_lac, const sms_model_t& sms)
{
    QList<uint8_t> out_data;
    uint32_t       size { 0 };
    auto           lvl_3 = FormLvl3SMS(sms);
    auto           lac   = FormLacLvl(model_lac);
    lac.InsertBits(lvl_3.GetList(), lvl_3.GetSizeBit());
    out_data = lac.GetList();
    size     = lac.GetSizeBit();
    return std::pair<QList<uint8_t>, uint32_t> { out_data, size };
}

/**
 * @brief Former lac filds for mes
 * @param[in] model - lac settings
 *
 * @return lac fieds
 */
FormerMes RachCollector::FormLacLvl(const lac_collector_model_t& model)
{
    FormerMes out;
    uint16_t  imsi_s2 = (m_mobile_config.IMSI >> 24) & 0x3FF;
    uint32_t  imsi_s1 = m_mobile_config.IMSI & 0xFFFFFF;
    uint32_t  esn     = m_mobile_config.ESN;

    out.InsertBits(0x01, 2); //pd
    out.InsertBits(model.msg_id, 6);

    out.InsertBits(0x0F, 5); //lac len
    //arq
    out.InsertBits(model.ack_seq, 3);
    out.InsertBits(model.msg_seq, 3);
    out.InsertBits(model.ack_req, 1);
    out.InsertBits(model.valid_ack, 1);
    out.InsertBits(0x00, 3);
    //addressing
    out.InsertBits(0x03, 3); //msid_type
    out.InsertBits(0x0B, 4); //msid_len

    out.InsertBits(esn, 32);                              //esn
    out.InsertBits(0x00, 1);                              //imsi_type(0 1)
    out.InsertBits(0x03, 2);                              //imsi_class
    out.InsertBits(0x00, 2);                              //reserved
    out.InsertBits(m_pch.extendedSystemParamMes.mcc, 10); //code country
    out.InsertBits(m_pch.extendedSystemParamMes.imsi_11_12, 7);
    out.InsertBits(imsi_s2, 10);
    out.InsertBits(imsi_s1, 24);

    out.InsertBits(0x00, 2); //auth mode(off)

    out.InsertBits(0x00, 7); //pading bits

    out.InsertBits(0x0F, 6); //pilot strength
    out.InsertBits(0x01, 1); //first is pta
    out.InsertBits(0x00, 1); //fist is
    out.InsertBits(0x00, 3); //num add pilots
    return out;
}

/**
 * @brief Former lvl3 filds for sms
 * @param[in] model - lvl3 settings
 *
 * @return lvl3 fieds
 */
FormerMes RachCollector::FormLvl3SMS(const sms_model_t& sms_model)
{
    FormerMes out;
    out.InsertBits(0x01, 8); //msg_number
    out.InsertBits(0x03, 6); //burst type
    out.InsertBits(0x01, 8); //num_msgs
    auto sms = SMSgenerate(sms_model);
    out.InsertBits((sms.GetSizeBit() + 7) / 8, 8); //num_fields
    out.InsertBits(sms.GetList(), sms.GetSizeBit());
    out.InsertBits(0x00, 1);
    return out;
}

/**
 * @brief Former lvl3 filds for registration
 * @param[in] model - lvl3 settings
 *
 * @return lvl3 fieds
 */
FormerMes RachCollector::FormLvl3Registration(const registration_collector_model_t& model)
{
    auto slotted_mode = m_mobile_config.support_slot;
    auto meid         = m_mobile_config.MEID;

    FormerMes out;
    if (model.reg_type == 1)
        out.InsertBits(0x01, 4); //power up
    else
        out.InsertBits(0x00, 4);

    out.InsertBits(0x00, 3); //slot cycle

    out.InsertBits(0x06, 8); //mob_p_rev(6 revision)

    if (meid) { //scm mask
        if (slotted_mode) {
            out.InsertBits(0x3A, 8);
        } else {
            out.InsertBits(0x1A, 8);
        }
    } else {
        if (slotted_mode) {
            out.InsertBits(0x2A, 8);
        } else {
            out.InsertBits(0x0A, 8);
        }
    }

    out.InsertBits(0x01, 1); //mob_term
    out.InsertBits(0x00, 4); //return cause
    out.InsertBits(0x01, 1); //qpch sup
    out.InsertBits(0x01, 1); //enhanced_Rc
    out.InsertBits(0x00, 1);

    return out;
}

/**
 * @brief Former lvl3 filds for extended status mes
 * @param[in] model - lvl3 settings
 *
 * @return lvl3 fieds
 */
FormerMes RachCollector::FormLvl3ExtendedStatus(const extended_status_response_model_t& model)
{
    FormerMes out;

    out.InsertBits(model.info_type, 8);
    if (model.info_type == BAND_CLASS_OP_MODE) {
        out.InsertBits(0x02, 3); //qual info len
        out.InsertBits(model.band_class, 5);
        out.InsertBits(model.op_mode, 8);
        out.InsertBits(0x00, 3); //reserved
    } else if (model.info_type == BAND_CLASS) {
        out.InsertBits(0x01, 3); //qual info len
        out.InsertBits(model.band_class, 5);
        out.InsertBits(0x00, 3); //reserved
    } else {
        out.InsertBits(0x00, 3); //qual info len
    }

    out.InsertBits(model.requests.size(), 4);

    for (auto i : model.requests) {
        out.InsertBits(i, 8);
        if (i == BAND_CLASS_INFORMATION) {
            out.InsertBits(0x01, 8); //len
            switch (model.band_class_info) {
            case 0:
                out.InsertBits(128, 8);
                break;
            case 1:
                out.InsertBits(64, 8);
                break;
            case 2:
                out.InsertBits(32, 8);
                break;
            case 3:
                out.InsertBits(16, 8);
                break;
            case 4:
                out.InsertBits(8, 8);
                break;
            case 5:
                out.InsertBits(4, 8);
                break;
            case 6:
                out.InsertBits(2, 8);
                break;
            case 7:
                out.InsertBits(1, 8);
                break;
            }
        } else if (i == OPERATING_MODE) {
            out.InsertBits(0x01, 8); //len
            out.InsertBits(model.op_mode, 5);
            out.InsertBits(0x00, 3); //reserved
        } else if (i == POWER_CLASS) {
            out.InsertBits(0x01, 8); //len
            out.InsertBits(model.max_eirp, 8);
        } else if (i == MEID) {
            out.InsertBits(0x08, 8); //len
            out.InsertBits(0x07, 4); //meid len
            out.InsertBits(m_mobile_config.MEID, 7 * 8);
            out.InsertBits(0x00, 4); //reserved
        } else if (i == POWER_CONTROL) {
            out.InsertBits(0x01, 8); //len
            out.InsertBits(0x02, 3); //min pwr cntl step
        }
    }

    //out.InsertBits(0x00, 5);

    return out;
}

/**
 * @brief Sms generator message
 * @param[in] sms - settings mes
 *
 * @return lvl3 fieds
 */
FormerMes RachCollector::SMSgenerate(const sms_model_t& sms)
{
    FormerMes former;
    auto      text = QString::fromStdString(sms.data);

    if (sms.sms_type_ack)
        former.InsertBits(0x02, 8); //sms msg type
    else {
        former.InsertBits(0x00, 8);  //sms msg type
        former.InsertBits(0x00, 8);  //parameter id(Teleservice Identifier)
        former.InsertBits(0x02, 8);  //parameter len
        former.InsertBits(4098, 16); //Wireless Messaging Teleservice
    }

    former.InsertBits(0x04, 8);                                 //parameter id
    former.InsertBits(((4 * sms.from.size() + 10) + 7) / 8, 8); //parameter len(+7 for rounding)
    former.InsertBits(0x00, 1);                                 //digit mode
    former.InsertBits(0x00, 1);                                 //number mode
    former.InsertBits(sms.from.size(), 8);
    for (auto i : sms.from) { //address
        if (i == '0')
            former.InsertBits(0x0A, 4);
        else if (i == '*')
            former.InsertBits(0x0B, 4);
        else if (i == '#')
            former.InsertBits(0x0C, 4);
        else
            (former.InsertBits(i, 4));
    }

    former.InsertBits(0x00, 8 - ((4 * sms.from.size() + 10) % 8)); //reserved
    //add cause code and ....там другой sms msg_type, требуется другая структура
    //TODO: ADD

    if (sms.sms_type_ack) {
        former.InsertBits(0x07, 8);          //parameter id(Bearer reply options)Cause code
        former.InsertBits(0x01, 8);          //parameter len
        former.InsertBits(sms.reply_seq, 6); //reply_seq
        former.InsertBits(0x00, 2);          //error_class

    } else {

        former.InsertBits(0x06, 8);          //parameter id(Bearer reply options)
        former.InsertBits(0x01, 8);          //parameter len
        former.InsertBits(sms.reply_seq, 6); //reply seq
        former.InsertBits(0x00, 2);          //reserved

        former.InsertBits(0x08, 8); //parameter id(Bearer data)

        auto skip = (3 + text.size()) % 8;
        if (sms.coding == 4) {
            former.InsertBits(12 + text.size() * 2, 8); //parameter len
        } else {
            former.InsertBits(8 + (text.size() * 7 + 36) / 8, 8); //parameter len
        }

        former.InsertBits(0x00, 8);  //subparameter id (Message identifier)
        former.InsertBits(0x03, 8);  //subparameter len
        former.InsertBits(0x02, 4);  //message type(submit)
        former.InsertBits(0x02, 16); //message id(2) TODO:fix
        former.InsertBits(0x00, 4);  //reserved

        former.InsertBits(0x01, 8); //subparameter id (user data)

        if (sms.coding == 4)
            former.InsertBits(text.size() * 2 + 2, 8); //subparameter len
        else
            former.InsertBits((text.size() * 7 + 21) / 8, 8); //subparameter len

        former.InsertBits(sms.coding, 5);  //msg encoding(used only unicode)
        former.InsertBits(text.size(), 8); //num_fields
        if (sms.coding == 4) {
            for (auto i : text) {
                former.InsertBits(i.unicode(), 16); //unicode message
            }
        } else
            for (auto i : text) {
                former.InsertBits(i.toLatin1(), 7); //unicode message
            }
        if (sms.coding == 4) {
            former.InsertBits(0x00, 3); //reserved
        } else {
            if (skip != 0)
                former.InsertBits(0x00, ((3 + text.size()) % 8)); //reserved
            //else
            //former.InsertBits(0x00, 8); //reserved
        }

        former.InsertBits(0x0A, 8); //reply option
        former.InsertBits(0x01, 8); //len
        former.InsertBits(0x00, 1); //user_ack_req
        former.InsertBits(0x01, 1); //user_ack_req
        former.InsertBits(0x00, 6); //reserved
    }

    return former;
}
