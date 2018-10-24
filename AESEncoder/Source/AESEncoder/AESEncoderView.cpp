#include "pch.h"
#include "AESEncoderView.h"
#include "AESEncoderController.h"
#include "ui_AESEncoderView.h"

using namespace AES;

class CAESEncoderViewWidget final : public QWidget, public Ui::AESEncoderView
{
public:
    CAESEncoderViewWidget(QWidget* parent);
    virtual ~CAESEncoderViewWidget() override final = default;

    void SetController(IAESEncoderController* controller);

private:
    IAESEncoderController* m_Controller = nullptr;
};

void CAESEncoderViewWidget::SetController(IAESEncoderController* controller)
{
    m_Controller = controller;
}

CAESEncoderViewWidget::CAESEncoderViewWidget(QWidget* parent) : QWidget(parent)
{
    setupUi(this);
}

class CAESEncoderView final : public IAESEncoderView
{
public:
    CAESEncoderView() = default;
    virtual ~CAESEncoderView() override final = default;

    virtual bool SetController(const IEncoderControllerSharedPtr& controller) override final;

    virtual QWidget* GetViewWidget() const noexcept override final;

private:
    void OnControllerChanged();

private:
    std::unique_ptr<CAESEncoderViewWidget> m_ViewWidget = std::make_unique<CAESEncoderViewWidget>(nullptr);
    IAESEncoderControllerSharedPtr m_Controller;
};

bool CAESEncoderView::SetController(const IEncoderControllerSharedPtr& controller)
{
    if (!controller)
    {
        m_Controller.reset();
        OnControllerChanged();
        return true;
    }

    const auto aesEncoderController = std::dynamic_pointer_cast<IAESEncoderController>(controller);
    if (!aesEncoderController)
        return false;

    m_Controller = aesEncoderController;
    OnControllerChanged();

    return true;
}

QWidget* CAESEncoderView::GetViewWidget() const noexcept
{
    return m_ViewWidget.get();
}

void CAESEncoderView::OnControllerChanged()
{
    EDITOR_ASSERT(m_ViewWidget);
    if (m_ViewWidget)
        m_ViewWidget->SetController(m_Controller.get());
}

IAESEncoderViewUniquePtr IAESEncoderView::Create()
{
    return std::make_unique<CAESEncoderView>();
}