
#include <QUICGemPOC/QUICGemPOCTypeIds.h>
#include <QUICGemPOCModuleInterface.h>
#include "QUICGemPOCEditorSystemComponent.h"

namespace QUICGemPOC
{
    class QUICGemPOCEditorModule
        : public QUICGemPOCModuleInterface
    {
    public:
        AZ_RTTI(QUICGemPOCEditorModule, QUICGemPOCEditorModuleTypeId, QUICGemPOCModuleInterface);
        AZ_CLASS_ALLOCATOR(QUICGemPOCEditorModule, AZ::SystemAllocator);

        QUICGemPOCEditorModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            // Add ALL components descriptors associated with this gem to m_descriptors.
            // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
            // This happens through the [MyComponent]::Reflect() function.
            m_descriptors.insert(m_descriptors.end(), {
                QUICGemPOCEditorSystemComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         * Non-SystemComponents should not be added here
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList {
                azrtti_typeid<QUICGemPOCEditorSystemComponent>(),
            };
        }
    };
}// namespace QUICGemPOC

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME, _Editor), QUICGemPOC::QUICGemPOCEditorModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_QUICGemPOC_Editor, QUICGemPOC::QUICGemPOCEditorModule)
#endif
