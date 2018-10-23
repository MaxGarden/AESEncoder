#include "pch.h"
#include "EncoderContext.h"
#include "Encoder.h"
#include "EncoderController.h"
#include "EncoderView.h"

class CEncoderContext final : public IEncoderContext
{
public:
    CEncoderContext(QString&& name);
    virtual ~CEncoderContext() override final = default;

    virtual bool SetEncoder(IEncoderUniquePtr&& encoder) override final;
    virtual const IEncoderSharedPtr& GetEncoder() const noexcept override final;

    virtual bool SetController(IEncoderControllerUniquePtr&& controller) override final;
    virtual const IEncoderControllerSharedPtr& GetController() const noexcept override final;

    virtual bool SetView(IEncoderViewUniquePtr&& view) override final;
    virtual const IEncoderViewSharedPtr& GetView() const noexcept override final;

    virtual const QString& GetName() const noexcept override final;

private:
    const QString m_Name;
    IEncoderSharedPtr m_Encoder;
    IEncoderControllerSharedPtr m_Controller;
    IEncoderViewSharedPtr m_View;
};

CEncoderContext::CEncoderContext(QString&& name) :
    m_Name(std::move(name))
{
}

bool CEncoderContext::SetEncoder(IEncoderUniquePtr&& encoder)
{
    m_Encoder = std::move(encoder);

    if (const auto& controller = GetController())
    {
        if (!controller->SetEncoder(m_Encoder))
        {
            EDITOR_ASSERT(false);
            SetController(nullptr);
        }
    }

    return true;
}

const IEncoderSharedPtr& CEncoderContext::GetEncoder() const noexcept
{
    return m_Encoder;
}

bool CEncoderContext::SetController(IEncoderControllerUniquePtr&& controller)
{
    if (!controller)
    {
        m_Controller.reset();
        return true;
    }

    if (const auto& encoder = GetEncoder())
    {
        const auto result = controller->SetEncoder(encoder);
        if (!result)
            return false;
    }

    m_Controller = std::move(controller);

    if (const auto& view = GetView())
    {
        if (!view->SetController(m_Controller))
        {
            EDITOR_ASSERT(false);
            SetView(nullptr);
        }
    }

    return true;
}

const IEncoderControllerSharedPtr& CEncoderContext::GetController() const noexcept
{
    return m_Controller;
}

bool CEncoderContext::SetView(IEncoderViewUniquePtr&& view)
{
    if (!view)
    {
        m_View.reset();
        return true;
    }


    if (const auto& controller = GetController())
    {
        const auto result = view->SetController(controller);
        if (!result)
            return false;
    }

    m_View = std::move(view);
    return true;
}

const IEncoderViewSharedPtr& CEncoderContext::GetView() const noexcept
{
    return m_View;
}

const QString& CEncoderContext::GetName() const noexcept
{
    return m_Name;
}

IEncoderContextUniquePtr IEncoderContext::Create(QString&& name)
{
    return std::make_unique<CEncoderContext>(std::move(name));
}