#ifndef SMSCONTROLLER_H
#define SMSCONTROLLER_H

#include <QDebug>
#include <QObject>
#include <QTime>
#include <vector>

#include "models/SmsModel.h"
#include "utils/FormerMes.h"
#include "utils/ParserMes.h"

#include "IVMSlib.h"

class SmsController : public QObject, public ParserMes {
    Q_OBJECT
public:
    explicit SmsController(QObject* parent = nullptr);
    ~SmsController() = default;
    void Clear();

public slots:
    void NewSms(int, int, dataBurstMesModel);
    void NewSmsRach(int, int, uint64_t, RAchDataBurstMesModel);

signals:
    /**
     * @brief SMSreceived, new sms came
     * @param[in] sid - the SID number of base
     * @param[in] sysTime - system time for sms
     * @param[in] addr - sms address
     * @param[in] data - sms user data
     *
     * @return none
     */
    void SMSreceived(const sms_model_t& sms);

    void SMSacknowledge();

private:
    smsModel m_sms;
    bool     m_toEnd { false };
};
#endif // SMSCONTROLLER_H
