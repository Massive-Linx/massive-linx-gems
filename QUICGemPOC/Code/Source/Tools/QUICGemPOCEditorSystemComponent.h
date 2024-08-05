
#pragma once

#include <AzToolsFramework/API/ToolsApplicationAPI.h>

#include <Clients/QUICGemPOCSystemComponent.h>

namespace QUICGemPOC
{
    /// System component for QUICGemPOC editor
    class QUICGemPOCEditorSystemComponent
        : public QUICGemPOCSystemComponent
        , protected AzToolsFramework::EditorEvents::Bus::Handler
    {
        using BaseSystemComponent = QUICGemPOCSystemComponent;
    public:
        AZ_COMPONENT_DECL(QUICGemPOCEditorSystemComponent);

        static void Reflect(AZ::ReflectContext* context);

        QUICGemPOCEditorSystemComponent();
        ~QUICGemPOCEditorSystemComponent();

    private:
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        // AZ::Component
        void Activate() override;
        void Deactivate() override;
    };
} // namespace QUICGemPOC
