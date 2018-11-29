#include "pch.h"
#include "AESEncoderViewWidget.h"
#include <QMessageBox>

using namespace AES;

void CAESEncoderViewWidget::SetController(IAESEncoderController* controller)
{
    m_Controller = controller;
    Setup();
}

CAESEncoderViewWidget::CAESEncoderViewWidget(QWidget* parent) : QWidget(parent)
{
    setupUi(this);
}

void CAESEncoderViewWidget::Setup()
{
    if (m_KeyTypeComboBox)
    {
        m_KeyTypeComboBox->clear();

        if(m_Controller)
            m_KeyTypeComboBox->addItems(QStringList{ tr("128 bits"), tr("192 bits"), tr("256 bits") });
    }

    if (m_KeyLineEdit)
        m_KeyLineEdit->setText(m_Controller ? m_Controller->GetKey().data() : QString{});
}

void CAESEncoderViewWidget::OnKeyTypeChanged(int index)
{
    if (index < 0)
        return;

    if (!m_Controller)
    {
        QMessageBox::critical(this, tr("Error"), tr("Cannot set key type: controller is null!"));
        return;
    }

    if (!m_Controller->SetKeyType(static_cast<EKeyType>(index)))
    {
        QMessageBox::critical(this, tr("Error"), tr("Cannot set key type: invalid key type!"));
        return;
    }

    if (m_KeyLineEdit)
        m_KeyLineEdit->setText(QString{});
}

void CAESEncoderViewWidget::OnKeyChanged(const QString& key)
{
    if (!m_Controller)
    {
        QMessageBox::critical(this, tr("Error"), tr("Cannot set key: controller is null!"));
        return;
    }

    if (!m_Controller->SetKey(key.toStdString()))
    {
        QMessageBox::critical(this, tr("Error"), tr("Cannot set key: invalid key!"));
        if (m_KeyLineEdit)
            m_KeyLineEdit->setText(QString{});
    }
}