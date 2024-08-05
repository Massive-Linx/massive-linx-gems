
#include <QUICGemPOC/QUICGemPOCTypeIds.h>
#include <QUICGemPOCModuleInterface.h>
#include "QUICGemPOCSystemComponent.h"

namespace QUICGemPOC
{
    class QUICGemPOCModule
        : public QUICGemPOCModuleInterface
    {
    public:
        AZ_RTTI(QUICGemPOCModule, QUICGemPOCModuleTypeId, QUICGemPOCModuleInterface);
        AZ_CLASS_ALLOCATOR(QUICGemPOCModule, AZ::SystemAllocator);
    };
}// namespace QUICGemPOC

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME), QUICGemPOC::QUICGemPOCModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_QUICGemPOC, QUICGemPOC::QUICGemPOCModule)
#endif
