#ifndef UISTATUSCONNECT_H
#define UISTATUSCONNECT_H

#include <IVMSlib.h>

//#include "models/BaseModel.h"
//#include "models/MobileModel.h"
#include <QWidget>

namespace Ui {
class uiStatusConnect;
}

class uiStatusConnect : public QWidget {
    Q_OBJECT

public:
    explicit uiStatusConnect(QWidget* parent = nullptr);
    ~uiStatusConnect();

public slots:
    void SetStatusInfo(const base_model_t& base_model, const mobile_status_t status);

private:
    Ui::uiStatusConnect* ui;
};

#endif // UISTATUSCONNECT_H
