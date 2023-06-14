#ifndef UISMSVIEWER_H
#define UISMSVIEWER_H

#include "listener.h"
#include "sendertoinst.h"

//#include "controllers/MainController.h"
#include "models/ScannerBaseTableModel.h"
//#include "models/SmsModel.h"
#include <QDialog>
#include <QMessageBox>
#include <QString>
#include <QTimer>

Q_DECLARE_METATYPE(std::string);

namespace Ui {
class uiSMSViewer;
}

class uiSMSViewer : public QDialog {
    Q_OBJECT

public:
    explicit uiSMSViewer(QWidget* parent = nullptr);
    ~uiSMSViewer();
    void onNewSMS(const sms_model_t& sms);
    void editStatus();
    void CheckSmsStatus();

signals:
    void SendMes(const std::string& number, const std::string& text, const uint8_t reply_seq, const bool status_unicode);

private slots:
    void on_btnSendSMS_clicked();

private:
    Ui::uiSMSViewer*       ui;
    ScannerBaseTableModel* m_table_model { nullptr };
    ScannerBaseTableModel* m_table_outgoing_sms { nullptr };

    QTimer* m_timer { nullptr };

    int m_pred_last_send_row { 0 };
};

#endif // UISMSVIEWER_H
