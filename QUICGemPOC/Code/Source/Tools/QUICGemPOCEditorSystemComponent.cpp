
#include <AzCore/Serialization/SerializeContext.h>
#include "QUICGemPOCEditorSystemComponent.h"

#include <QUICGemPOC/QUICGemPOCTypeIds.h>

namespace QUICGemPOC
{
    AZ_COMPONENT_IMPL(QUICGemPOCEditorSystemComponent, "QUICGemPOCEditorSystemComponent",
        QUICGemPOCEditorSystemComponentTypeId, BaseSystemComponent);

    void QUICGemPOCEditorSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<QUICGemPOCEditorSystemComponent, QUICGemPOCSystemComponent>()
                ->Version(0);
        }
    }

    QUICGemPOCEditorSystemComponent::QUICGemPOCEditorSystemComponent() = default;

    QUICGemPOCEditorSystemComponent::~QUICGemPOCEditorSystemComponent() = default;

    void QUICGemPOCEditorSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        BaseSystemComponent::GetProvidedServices(provided);
        provided.push_back(AZ_CRC_CE("QUICGemPOCEditorService"));
    }

    void QUICGemPOCEditorSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        BaseSystemComponent::GetIncompatibleServices(incompatible);
        incompatible.push_back(AZ_CRC_CE("QUICGemPOCEditorService"));
    }

    void QUICGemPOCEditorSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        BaseSystemComponent::GetRequiredServices(required);
    }

    void QUICGemPOCEditorSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        BaseSystemComponent::GetDependentServices(dependent);
    }

    void QUICGemPOCEditorSystemComponent::Activate()
    {
        QUICGemPOCSystemComponent::Activate();
        AzToolsFramework::EditorEvents::Bus::Handler::BusConnect();
    }

    void QUICGemPOCEditorSystemComponent::Deactivate()
    {
        AzToolsFramework::EditorEvents::Bus::Handler::BusDisconnect();
        QUICGemPOCSystemComponent::Deactivate();
    }

} // namespace QUICGemPOC
