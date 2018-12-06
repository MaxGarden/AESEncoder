#include "pch.h"
#include "MainWindow.h"
#include "Encoder/Encoder.h"
#include "Encoder/EncoderController.h"
#include "Encoder/EncoderView.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QElapsedTimer>
#include <QDateTime>
#include <fstream>

CMainWindow::CMainWindow() : QMainWindow(nullptr)
{
    setupUi(this);
}

bool CMainWindow::RegisterEncoder(const QString& name, IEncoderUniquePtr&& encoder)
{
    if (!encoder)
        return false;

    if (m_Encoders.find(name) != m_Encoders.cend())
    {
        EDITOR_ASSERT(false);
        return false;
    }

    auto& encoderReference = *encoder;

    m_Encoders.emplace(name, std::move(encoder));
    OnEncoderRegistered(name, encoderReference);

    return true;
}

bool CMainWindow::UnregisterEncoder(const QString& name)
{
    auto iterator = m_Encoders.find(name);
    EDITOR_ASSERT(iterator != m_Encoders.cend());
    if (iterator == m_Encoders.cend())
        return false;

    const auto encoder = std::move(iterator->second);
    m_Encoders.erase(iterator);

    EDITOR_ASSERT(encoder);
    if(encoder)
        OnEncoderUnregistered(name, *encoder);

    return true;
}

bool CMainWindow::RegisterEncoderController(IEncoderControllerUniquePtr&& controller)
{
    if (!controller)
        return false;

    m_ControllersPool.emplace_back(std::move(controller));
    return true;
}

bool CMainWindow::RegisterEncoderView(IEncoderViewUniquePtr&& view)
{
    if (!view)
        return false;

    m_ViewsPool.emplace_back(std::move(view));
    return true;
}

bool CMainWindow::Initialize()
{
    EDITOR_ASSERT(!m_Initialized);
    if (m_Initialized)
        return false;

    m_Initialized = true;

    EDITOR_ASSERT(m_CurrentEncoder);

    EDITOR_ASSERT(m_Initialized);
    return m_Initialized;
}

void CMainWindow::OnEncoderChanged(const QString& name)
{
    const auto iterator = m_Encoders.find(name);
    const auto isIteratorValid = iterator != m_Encoders.cend();

    EDITOR_ASSERT(isIteratorValid);
    SelectEncoder(isIteratorValid ? iterator->second : nullptr);
}

void CMainWindow::OnEncodeButtonClicked()
{
    EncodeFile(QString::null);
}

void CMainWindow::OnEncodeAsButtonClicked()
{
    const auto saveFileName = QFileDialog::getSaveFileName(this, tr("Save file"));

    if (saveFileName.isNull() || saveFileName.isEmpty())
        return;

    EncodeFile(saveFileName);
}

void CMainWindow::OnBrowseFileToEncodeButtonClicked()
{
    const auto fileToEncodeName = QFileDialog::getOpenFileName(this, tr("File to encode"));

    if (fileToEncodeName.isNull() || fileToEncodeName.isEmpty())
        return;

    if (m_FileToEncodeLineEdit)
        m_FileToEncodeLineEdit->setText(fileToEncodeName);
}

void CMainWindow::SelectEncoder(IEncoderSharedPtr encoder)
{
    ResetControllers();
    ResetViews();

    if (!encoder)
    {
        m_CurrentEncoder.reset();
        SetEncoderViewWidget(nullptr);
        return;
    }

    const auto encoderController = FindMatchingController(encoder);
    EDITOR_ASSERT(encoderController);
    if (!encoderController)
        return SelectEncoder(nullptr);

    const auto& encoderView = FindMatchingView(encoderController);
    EDITOR_ASSERT(encoderView);
    if(!encoderView)
        return SelectEncoder(nullptr);

    SetEncoderViewWidget(encoderView->GetViewWidget());
    m_CurrentEncoder = std::move(encoder);
}

template<typename ContainerType, typename FunctionType>
void ResetContainer(const ContainerType& container, FunctionType functionToReset)
{
    for (const auto& element : container)
    {
        if(!element)
            continue;

        const auto result = (element.get()->*functionToReset)(nullptr);
        EDITOR_ASSERT(result);
    }
}

void CMainWindow::ResetControllers()
{
    ResetContainer(m_ControllersPool, &IEncoderController::SetEncoder);
}

void CMainWindow::ResetViews()
{
    ResetContainer(m_ViewsPool, &IEncoderView::SetController);
}

IEncoderControllerSharedPtr CMainWindow::FindMatchingController(const IEncoderSharedPtr& encoder) const noexcept
{
    if (!encoder)
        return nullptr;

    const auto controllerIterator = std::find_if(m_ControllersPool.cbegin(), m_ControllersPool.cend(), [&encoder](const auto& controller)
    {
        return controller && controller->SetEncoder(encoder);
    });

    if (controllerIterator == m_ControllersPool.cend())
        return nullptr;

    return *controllerIterator;
}

IEncoderView* CMainWindow::FindMatchingView(const IEncoderControllerSharedPtr& controller) const noexcept
{
    if (!controller)
        return nullptr;

    const auto viewIterator = std::find_if(m_ViewsPool.cbegin(), m_ViewsPool.cend(), [&controller](const auto& view)
    {
        return view && view->SetController(controller);
    });

    if (viewIterator == m_ViewsPool.cend())
        return nullptr;

    return viewIterator->get();
}

void CMainWindow::SetEncoderViewWidget(QWidget* viewWidget)
{
    EDITOR_ASSERT(m_SelectedEncoderGroupBox);
    if (!m_SelectedEncoderGroupBox)
        return;

    const auto layout = m_SelectedEncoderGroupBox->layout();
    EDITOR_ASSERT(layout);
    if (!layout)
        return;

    if (m_CurrentViewWidget)
        layout->removeWidget(m_CurrentViewWidget);

    m_CurrentViewWidget = viewWidget;

    if (viewWidget)
        layout->addWidget(viewWidget);
}

void CMainWindow::OnEncoderRegistered(const QString& name, IEncoder&)
{
    EDITOR_ASSERT(m_EncoderSelectComboBox);
    if (m_EncoderSelectComboBox)
        m_EncoderSelectComboBox->addItem(name);
}

void CMainWindow::OnEncoderUnregistered(const QString& name, IEncoder& encoder)
{
    EDITOR_ASSERT(m_EncoderSelectComboBox);
    if (m_EncoderSelectComboBox)
    {
        const auto itemsCount = m_EncoderSelectComboBox->count();

        const auto result = [&name, &comboBox = *m_EncoderSelectComboBox, itemsCount = m_EncoderSelectComboBox->count()]()
        {
            for (auto i = 0; i < itemsCount; ++i)
            {
                if (comboBox.itemText(i) != name)
                    continue;

                comboBox.removeItem(i);
                return true;
            }

            return false;
        }();

        EDITOR_ASSERT(result);
    }

    if (m_CurrentEncoder.get() == &encoder)
        SelectEncoder(nullptr);
}

void CMainWindow::EncodeFile(const QString& savePath) const
{
    const auto fileToEncodeName = m_FileToEncodeLineEdit ? m_FileToEncodeLineEdit->text() : QString{};

    EncoderData dataToEncode;
    if (!OpenFileToEncode(fileToEncodeName, dataToEncode))
        return;

    QElapsedTimer timer;
    timer.start();

    const auto encodedData = m_CurrentEncoder->Encode(dataToEncode);

    const auto encodingTimeInNanoSeconds = timer.nsecsElapsed();

    if (encodedData.size() < dataToEncode.size())
    {
        QMessageBox::critical(nullptr, tr("Error"), tr("Encoding failed!"));
        return;
    }

    const auto fileToSaveName = savePath.isNull() ? fileToEncodeName : savePath;
    if (!SaveEncodedDataToFile(fileToSaveName, encodedData))
        return;

    QMessageBox::information(nullptr, tr("Success"), tr("Done, see logs for details."));

    if (m_LogTextEdit)
    {
        const auto currentTime = QDateTime::currentDateTime().time().toString();
        m_LogTextEdit->append(QString{ "%1: Encoding file %2 to file %3 by using encoder %4 (max threads number - %5) took %6 ns." }
            .arg(currentTime)
            .arg(fileToEncodeName, fileToSaveName)
            .arg(m_EncoderSelectComboBox ? m_EncoderSelectComboBox->currentText() : tr("UNKNOWN"))
            .arg(m_CurrentEncoder->GetMaximumThreadsWorkers())
            .arg(encodingTimeInNanoSeconds));
    }
}

bool CMainWindow::OpenFileToEncode(const QString& filename, EncoderData& dataToEncode) const
{
    static const auto maxEncodeFileSize = 1024 * 1024 * 1024; //1GiB

    if (!m_CurrentEncoder)
    {
        QMessageBox::critical(nullptr, tr("Error"), tr("Internal error: current encoder is null!"));
        return false;
    }

    std::ifstream fileToEncode{ filename.toStdString(), std::ios::binary | std::ios::ate };
    if (!fileToEncode.good())
    {
        QMessageBox::critical(nullptr, tr("Error"), QString{ "Cannot open file \"%1\" to read!" }.arg(filename));
        return false;
    }

    const auto fileToEncodeSize = fileToEncode.tellg();
    if (fileToEncodeSize > maxEncodeFileSize)
    {
        QMessageBox::critical(nullptr, tr("Error"), QString{ "File size %1 is greater than maximum size %2!" }.arg(fileToEncodeSize, maxEncodeFileSize));
        return false;
    }

    fileToEncode.seekg(0, std::ios::beg);;

    dataToEncode.resize(fileToEncodeSize);
    fileToEncode.read(reinterpret_cast<char*>(dataToEncode.data()), dataToEncode.size());

    fileToEncode.close();

    return true;
}

bool CMainWindow::SaveEncodedDataToFile(const QString& filename, const EncoderData& encodedData) const
{
    std::ofstream fileToSave{ filename.toStdString(), std::ios::binary };
    if (!fileToSave.good())
    {
        QMessageBox::critical(nullptr, tr("Error"), QString{ "Cannot open file \"%1\" to write!" }.arg(filename));
        return false;
    }

    fileToSave.write(reinterpret_cast<const char*>(encodedData.data()), encodedData.size());
    fileToSave.close();

    return true;
}