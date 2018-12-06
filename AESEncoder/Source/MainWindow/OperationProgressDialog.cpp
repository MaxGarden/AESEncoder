#include "pch.h"
#include "OperationProgressDialog.h"
#include <QTimer>

COperationProgressDialog::COperationProgressDialog(const QString& labelText, GetProgressCallback&& progressCallback, unsigned int refreshTimeInMs, QWidget* parent /*= nullptr*/) :
    QProgressDialog(labelText, QString(), 0, 100, parent),
    m_ProgressCallback(std::move(progressCallback))
{
    Setup(refreshTimeInMs);
}

void COperationProgressDialog::Setup(unsigned int refreshTimeInMs)
{
    setValue(0);
    setWindowModality(Qt::WindowModal);

    EDITOR_ASSERT(m_ProgressCallback);
    if (!m_ProgressCallback)
        return;

    const auto timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(RefreshProgress()));

    timer->start(refreshTimeInMs);
}

void COperationProgressDialog::RefreshProgress()
{
    EDITOR_ASSERT(m_ProgressCallback);
    if (!m_ProgressCallback)
        return;

    const auto progress = std::min(100u, m_ProgressCallback());
    setValue(progress);
}
