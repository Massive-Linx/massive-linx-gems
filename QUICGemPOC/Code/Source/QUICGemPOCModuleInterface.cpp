
#include "QUICGemPOCModuleInterface.h"
#include <AzCore/Memory/Memory.h>

#include <QUICGemPOC/QUICGemPOCTypeIds.h>

#include <Clients/QUICGemPOCSystemComponent.h>

namespace QUICGemPOC
{
    AZ_TYPE_INFO_WITH_NAME_IMPL(QUICGemPOCModuleInterface,
        "QUICGemPOCModuleInterface", QUICGemPOCModuleInterfaceTypeId);
    AZ_RTTI_NO_TYPE_INFO_IMPL(QUICGemPOCModuleInterface, AZ::Module);
    AZ_CLASS_ALLOCATOR_IMPL(QUICGemPOCModuleInterface, AZ::SystemAllocator);

    QUICGemPOCModuleInterface::QUICGemPOCModuleInterface()
    {
        // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
        // Add ALL components descriptors associated with this gem to m_descriptors.
        // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
        // This happens through the [MyComponent]::Reflect() function.
        m_descriptors.insert(m_descriptors.end(), {
            QUICGemPOCSystemComponent::CreateDescriptor(),
            });
    }

    AZ::ComponentTypeList QUICGemPOCModuleInterface::GetRequiredSystemComponents() const
    {
        return AZ::ComponentTypeList{
            azrtti_typeid<QUICGemPOCSystemComponent>(),
        };
    }
} // namespace QUICGemPOC
