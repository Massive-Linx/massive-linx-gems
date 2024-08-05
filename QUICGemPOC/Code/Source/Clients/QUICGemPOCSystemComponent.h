
#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <QUICGemPOC/QUICGemPOCBus.h>
#include <ISystem.h>

#include <msquic.h>

namespace QUICGemPOC
{
    class QUICGemPOCSystemComponent
        : public AZ::Component
        , protected QUICGemPOCRequestBus::Handler
        , public AZ::TickBus::Handler
        , public CrySystemEventBus::Handler
        , public ISystemEventListener
    {
    public:
        AZ_COMPONENT_DECL(QUICGemPOCSystemComponent);

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

		// ISystemEventListener
		void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;
        void OnCrySystemInitialized(ISystem& system, const SSystemInitParams& systemInitParams) override;
		// ~ISystemEventListener

        QUICGemPOCSystemComponent();
        ~QUICGemPOCSystemComponent();

        void ClientSend(HQUIC connection);
        void ConnectionClose(HQUIC connection);
        void StreamClose(HQUIC stream);

    protected:
        ////////////////////////////////////////////////////////////////////////
        // QUICGemPOCRequestBus interface implementation

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZTickBus interface implementation
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;
        ////////////////////////////////////////////////////////////////////////

        void InitializeMSQUIC();
        void Cleanup();
        void RunClient();
        bool LoadConfiguration();
        
        //
        // The QUIC API/function table returned from MsQuicOpen2. It contains all the
        // functions called by the app to interact with MsQuic.
        //
        const QUIC_API_TABLE* m_msQuic = nullptr;
        HQUIC m_registration;
        HQUIC m_configuration;
    };

} // namespace QUICGemPOC
