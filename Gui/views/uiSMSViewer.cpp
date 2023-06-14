#include "uiSMSViewer.h"
#include "ui_uiSMSViewer.h"

uiSMSViewer::uiSMSViewer(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::uiSMSViewer)
    , m_table_model(new ScannerBaseTableModel(QStringList({ "Отправитель", "Сообщение" }), this))
    , m_table_outgoing_sms(new ScannerBaseTableModel(QStringList({ "Получатель", "Сообщение", "Статус" }), this))
    , m_timer(new QTimer(this))
{
    ui->setupUi(this);

    qRegisterMetaType<std::string>("string");
    qRegisterMetaType<std::basic_string<char>>("std::basic_string<char>");

    QRegExp setEx("^-?([0-9]{0,20})$");

    auto* setValid = new QRegExpValidator(setEx, this);
    ui->linMobileNumber->setValidator(setValid);

    ui->tblIncomingSMS->setModel(m_table_model);
    ui->tblOutgoingSMS->setModel(m_table_outgoing_sms);
    ui->tblIncomingSMS->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblOutgoingSMS->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    connect(m_timer, &QTimer::timeout, this, &uiSMSViewer::CheckSmsStatus);
}

uiSMSViewer::~uiSMSViewer()
{
    delete ui;
}

/**
 * @brief Handler new SMS
 * @param[in] sms - SMS from BS
 *
 * @return none
 */
void uiSMSViewer::onNewSMS(const sms_model_t& sms)
{
    auto current_row = m_table_model->rowCount();
    m_table_model->insertRows(current_row, 1);
    m_table_model->setData(m_table_model->index(current_row, 0), QString::fromStdString(sms.from));

    if (sms.coding != 4)
        m_table_model->setData(m_table_model->index(current_row, 1), QString::fromStdString(sms.data));
    else
        m_table_model->setData(m_table_model->index(current_row, 1), QString::fromStdString(sms.data));

    if (current_row > 10000)
        m_table_model->removeRows(0, 10000);

    ui->tblIncomingSMS->resizeRowsToContents();
}

/**
 * @brief Editor status SMS(from Mobile)
 *
 * @return none
 */
void uiSMSViewer::editStatus()
{
    if (m_table_outgoing_sms->rowCount() < m_pred_last_send_row + 1) {
        return;
    }

    m_table_outgoing_sms->setData(m_table_outgoing_sms->index(m_pred_last_send_row, 2), "Доставлено");

    m_pred_last_send_row++;

    if (m_pred_last_send_row > 10000) {
        m_table_outgoing_sms->removeRows(0, 10000);
        m_pred_last_send_row = 0;
    }

    m_timer->stop();
}

/**
 * @brief Check row count in table
 *
 * @return none
 */
void uiSMSViewer::CheckSmsStatus()
{
    m_pred_last_send_row = m_table_outgoing_sms->rowCount();
    m_timer->stop();
}

/**
 * @brief Handler for Send SMS to BS
 *
 * @return none
 */
void uiSMSViewer::on_btnSendSMS_clicked()
{
    if (m_timer->isActive()) {
        QMessageBox::warning(this, "Внимание", "Подождите пожалуйста");
        return;
    }

    auto phone_number = ui->linMobileNumber->text();
    auto sms_text     = ui->linSMSBody->text();

    bool status_unicode { false };

    for (auto i : sms_text)
        if (i < 31 || i > 126)
            status_unicode = true;

    if (status_unicode && (sms_text.size() > 28)) {
        QMessageBox::warning(this, "Ошибка", "Ограничение 28 символов для Unicode");
        return;
    }

    if (!status_unicode && (sms_text.size() > 64)) {
        QMessageBox::warning(this, "Ошибка", "Ограничение 64 символа для 7-Ascii");
        return;
    }

    auto current_row = m_table_outgoing_sms->rowCount();
    m_table_outgoing_sms->insertRows(current_row, 1);
    m_table_outgoing_sms->setData(m_table_outgoing_sms->index(current_row, 0), phone_number);
    m_table_outgoing_sms->setData(m_table_outgoing_sms->index(current_row, 1), sms_text);
    m_table_outgoing_sms->setData(m_table_outgoing_sms->index(current_row, 2), "Не доставлено");
    m_timer->start(18000);
    if (status_unicode)
        emit SendMes(phone_number.toStdString(), sms_text.toStdString(), 0, status_unicode);
    else
        emit SendMes(phone_number.toStdString(), sms_text.toLatin1().toStdString(), 0, status_unicode);

    ui->tblOutgoingSMS->resizeRowsToContents();
}
