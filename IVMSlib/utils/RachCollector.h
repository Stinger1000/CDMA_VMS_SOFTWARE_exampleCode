#ifndef RACHCOLLECTOR_H
#define RACHCOLLECTOR_H

#include "FormerMes.h"
#include "controllers/SmsController.h"
#include "models/CollectorModel.h"
#include "models/ParsModels.h"

class RachCollector {
public:
    RachCollector(const mobile_setting_t& config, const elementSync& sync, const uniTypes& pch);
    ~RachCollector() = default;
    std::pair<QList<uint8_t>, uint32_t> GetRegistrationMes(const lac_collector_model_t& model_lac,
        const registration_collector_model_t&                                           model_lvl3);

    std::pair<QList<uint8_t>, uint32_t> GetExtendedStatusMes(const lac_collector_model_t& model_lac,
        const extended_status_response_model_t&                                           status);
    std::pair<QList<uint8_t>, uint32_t> GetDataBurstMes(const lac_collector_model_t& model_lac, const sms_model_t& sms);

private:
    FormerMes SMSgenerate(const sms_model_t& sms);

    FormerMes FormLacLvl(const lac_collector_model_t& model);

    FormerMes FormLvl3Registration(const registration_collector_model_t& model);
    FormerMes FormLvl3ExtendedStatus(const extended_status_response_model_t& model);
    FormerMes FormLvl3SMS(const sms_model_t& sms);

    elementSync      m_sync;
    uniTypes         m_pch;
    mobile_setting_t m_mobile_config;
};

#endif // RACHCOLLECTOR_H
