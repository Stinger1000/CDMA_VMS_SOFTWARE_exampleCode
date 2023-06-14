#include "uiStatusConnect.h"
#include "ui_uiStatusConnect.h"

uiStatusConnect::uiStatusConnect(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::uiStatusConnect)
{
    ui->setupUi(this);
}

uiStatusConnect::~uiStatusConnect()
{
    delete ui;
}

/**
 * @brief Display statuns MS
 * @param[in] base_model - status BS
 * @param[in] status - status MS
 *
 * @return none
 */
void uiStatusConnect::SetStatusInfo(const base_model_t& base_model, const mobile_status_t status)
{
    if (base_model.sync_state == fsync_state_t::FOUND) {
        switch (status) {
        case IDLE:
            ui->lblStatus->setText("Не подключено");
            break;
        case CONNECTED_TO_BASE:
            ui->lblStatus->setText("Подключено к БС");
            break;
        case REGISTRATION_COMPELED:
            ui->lblStatus->setText("Регистрация пройдена успешно");
            break;
        case REGISTRATION_FAILED:
            ui->lblStatus->setText("Ошибка регистрации");
            break;
        case REGISTRATION_:
            ui->lblStatus->setText("Регистрация....");
            break;
        }
        ui->lblBER->setNum(base_model.ber);
        ui->lblBLER->setNum(base_model.bler);
        ui->lblSID->setNum(base_model.sid);
        ui->lblNID->setNum(base_model.nid);
        ui->lblPower->setNum(static_cast<double>(base_model.power));
    } else {
        ui->lblStatus->setText("Не подключено к БС");
        ui->lblBER->setText("");
        ui->lblBLER->setText("");
        ui->lblSID->setText("");
        ui->lblNID->setText("");
        ui->lblPower->setText("");
    }
}
