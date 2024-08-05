
#pragma once

#include <QUICGemPOC/QUICGemPOCTypeIds.h>

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace QUICGemPOC
{
    class QUICGemPOCRequests
    {
    public:
        AZ_RTTI(QUICGemPOCRequests, QUICGemPOCRequestsTypeId);
        virtual ~QUICGemPOCRequests() = default;
        // Put your public methods here
    };

    class QUICGemPOCBusTraits
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////
    };

    using QUICGemPOCRequestBus = AZ::EBus<QUICGemPOCRequests, QUICGemPOCBusTraits>;
    using QUICGemPOCInterface = AZ::Interface<QUICGemPOCRequests>;

} // namespace QUICGemPOC
