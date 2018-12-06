#if !defined(__OPERATION_PROGRESS_DIALOG_H__)
#define __OPERATION_PROGRESS_DIALOG_H__
#pragma once

#include <QProgressDialog>

using GetProgressCallback = std::function<unsigned int()>; //progress in range 0..100

class COperationProgressDialog final : public QProgressDialog
{
    Q_OBJECT

public:
    COperationProgressDialog(const QString& labelText, GetProgressCallback&& progressCallback, unsigned int refreshTimeInMs, QWidget* parent = nullptr);
    virtual ~COperationProgressDialog() override final = default;

private:
    void Setup(unsigned int refreshTimeInMs);

private slots:
    void RefreshProgress();

private:
    const GetProgressCallback m_ProgressCallback;
};

#endif //__OPERATION_PROGRESS_DIALOG_H__