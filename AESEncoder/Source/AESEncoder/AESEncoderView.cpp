#include "pch.h"
#include "AESEncoderView.h"
#include "AESEncoderController.h"

using namespace AES;

class CAESEncoderView final : public IAESEncoderView
{
public:
    CAESEncoderView() = default;
    virtual ~CAESEncoderView() override final = default;

    virtual bool SetController(const IEncoderControllerSharedPtr& controller) override final;

    virtual QWidget* GetViewWidget() const noexcept override final;

private:
    IEncoderControllerSharedPtr m_Controller;
};

bool CAESEncoderView::SetController(const IEncoderControllerSharedPtr& controller)
{
    if (!controller)
    {
        m_Controller.reset();
        return true;
    }

    const auto aesEncoderController = std::dynamic_pointer_cast<IAESEncoderController>(controller);
    EDITOR_ASSERT(aesEncoderController);
    if (!aesEncoderController)
        return false;

    m_Controller = aesEncoderController;
    return true;
}

QWidget* CAESEncoderView::GetViewWidget() const noexcept
{
    return new QWidget();
}

IAESEncoderViewUniquePtr IAESEncoderView::Create()
{
    return std::make_unique<CAESEncoderView>();
}