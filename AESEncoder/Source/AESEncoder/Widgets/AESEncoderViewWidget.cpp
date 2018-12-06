#include "pch.h"
#include "AESEncoderViewWidget.h"
#include <QMessageBox>

using namespace AES;

CAESEncoderViewWidget::CAESEncoderViewWidget(QWidget* parent) : QWidget(parent)
{
    setupUi(this);
    ResetView();
}

void CAESEncoderViewWidget::SetController(IAESEncoderController* controller)
{
    if (m_Controller = controller)
    {
        if (!TryApplySettingsToController())
        {
            QMessageBox::critical(this, tr("Error"), tr("Cannot apply settings for selected controller. Reseting view!"));
            ResetView();
        }
    }
}

void CAESEncoderViewWidget::ResetView()
{
    if (m_KeyTypeComboBox)
    {
        m_KeyTypeComboBox->clear();
        m_KeyTypeComboBox->addItems(QStringList{ tr("128 bits"), tr("192 bits"), tr("256 bits") });
    }

    if (m_KeyLineEdit)
        m_KeyLineEdit->setText(m_Controller ? m_Controller->GetKey().data() : QString{});

    if (m_MaxThreadsNumberComboBox)
    {
        m_MaxThreadsNumberComboBox->clear();
        m_MaxThreadsNumberComboBox->addItems(QStringList{ tr("1"), tr("2"), tr("4"), tr("8"), tr("16") });
    }
}

bool CAESEncoderViewWidget::TryApplySettingsToController()
{
    EDITOR_ASSERT(m_Controller);
    if (!m_Controller)
        return false;

    auto result = true;

    result &= m_KeyTypeComboBox && m_Controller->SetKeyType(static_cast<EKeyType>(m_KeyTypeComboBox->currentIndex()));
    result &= m_KeyLineEdit && m_Controller->SetKey(m_KeyLineEdit->text().toStdString());

    if (m_MaxThreadsNumberComboBox)
    {
        const auto threadsNumber = m_MaxThreadsNumberComboBox->currentText().toInt(&result);
        result &= m_Controller->SetMaximumThreadsWorkers(threadsNumber);
    }

    return result;
}

void CAESEncoderViewWidget::OnKeyTypeChanged(int index)
{
    if (index < 0)
        return;

    if (!m_Controller)
        return;

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
        return;

    if (!m_Controller->SetKey(key.toStdString()))
    {
        QMessageBox::critical(this, tr("Error"), tr("Cannot set key: invalid key!"));
        if (m_KeyLineEdit)
            m_KeyLineEdit->setText(QString{});
    }
}

void CAESEncoderViewWidget::OnMaxThreadsNumberChanged(const QString& number)
{
    if (!m_Controller)
        return;

    auto parseResult = true;
    const auto threadsNumber = number.toInt(&parseResult);

    const auto rollback = [this]()
    {
        if (m_MaxThreadsNumberComboBox)
            m_MaxThreadsNumberComboBox->setCurrentText(QString("%1").arg(m_Controller->GetMaximumThreadsWorkers()));
    };

    if (!parseResult || threadsNumber < 1)
    {
        QMessageBox::critical(this, tr("Error"), QString("\"%1\" is not valid threads number!").arg(number));
        return rollback();
    }

    if (!m_Controller->SetMaximumThreadsWorkers(threadsNumber))
    {
        QMessageBox::critical(this, tr("Error"), tr("Cannot set maximum threads: invalid number!"));
        return rollback();
    }
}