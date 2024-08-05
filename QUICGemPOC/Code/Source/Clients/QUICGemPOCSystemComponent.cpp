
#include "QUICGemPOCSystemComponent.h"

#include <QUICGemPOC/QUICGemPOCTypeIds.h>

#include <AzCore/Serialization/SerializeContext.h>

//#include <quiche.h>

namespace QUICGemPOC
{
	//
    // The (optional) registration configuration for the app. This sets a name for
    // the app (used for persistent storage and for debugging). It also configures
    // the execution profile, using the default "low latency" profile.
    //
	const QUIC_REGISTRATION_CONFIG RegConfig = { "quicsample", QUIC_EXECUTION_PROFILE_LOW_LATENCY };

	//
    // The default idle timeout period (1 second) used for the protocol.
    //
	const uint64_t IdleTimeoutMs = 1000;

	//
    // The protocol name used in the Application Layer Protocol Negotiation (ALPN).
    //
	const QUIC_BUFFER Alpn = { sizeof("sample") - 1, (uint8_t*)"sample" };

	//
    // The UDP port used by the server side of the protocol.
    //
	const uint16_t UdpPort = 4567;

	//
    // The length of buffer sent over the streams in the protocol.
    //
	const uint32_t SendBufferLength = 100;

    AZ_COMPONENT_IMPL(QUICGemPOCSystemComponent, "QUICGemPOCSystemComponent",
        QUICGemPOCSystemComponentTypeId);

    void QUICGemPOCSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<QUICGemPOCSystemComponent, AZ::Component>()
                ->Version(0)
                ;
        }
    }

    void QUICGemPOCSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("QUICGemPOCService"));
    }

    void QUICGemPOCSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("QUICGemPOCService"));
    }

    void QUICGemPOCSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void QUICGemPOCSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    void QUICGemPOCSystemComponent::OnSystemEvent(ESystemEvent event, UINT_PTR /*wparam*/, UINT_PTR /*lparam*/)
    {
		switch (event)
		{
		case ESYSTEM_EVENT_LEVEL_LOAD_END:
            RunClient();
			break;
		default:
			break;
		}
    }

    void QUICGemPOCSystemComponent::OnCrySystemInitialized(ISystem& system, const SSystemInitParams& systemInitParams)
    {
        (void)systemInitParams;
        system.GetISystemEventDispatcher()->RegisterListener(this);
    }

    QUICGemPOCSystemComponent::QUICGemPOCSystemComponent()
    {
        if (QUICGemPOCInterface::Get() == nullptr)
        {
            QUICGemPOCInterface::Register(this);
        }

        InitializeMSQUIC();
    }

    QUICGemPOCSystemComponent::~QUICGemPOCSystemComponent()
    {
        Cleanup();
        if (QUICGemPOCInterface::Get() == this)
        {
            QUICGemPOCInterface::Unregister(this);
        }
    }

    void QUICGemPOCSystemComponent::Init()
    {
    }

    void QUICGemPOCSystemComponent::Activate()
    {
        QUICGemPOCRequestBus::Handler::BusConnect();
        AZ::TickBus::Handler::BusConnect();
        CrySystemEventBus::Handler::BusConnect();
    }

    void QUICGemPOCSystemComponent::Deactivate()
    {
		if (GetISystem() && GetISystem()->GetISystemEventDispatcher())
		{
			GetISystem()->GetISystemEventDispatcher()->RemoveListener(this);
		}

        CrySystemEventBus::Handler::BusDisconnect();
        AZ::TickBus::Handler::BusDisconnect();
        QUICGemPOCRequestBus::Handler::BusDisconnect();
    }

    void QUICGemPOCSystemComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
    }

    void QUICGemPOCSystemComponent::InitializeMSQUIC()
    {
        QUIC_STATUS Status = QUIC_STATUS_SUCCESS;
        if (QUIC_FAILED(Status = MsQuicOpen2(&m_msQuic))) {
            AZ_Error("MSQUIC", false, "MsQuicOpen2 failed, 0x%x!\n", Status);
            Cleanup();
        }

		//
	    // Create a registration for the app's connections.
	    //
		if (QUIC_FAILED(Status = m_msQuic->RegistrationOpen(&RegConfig, &m_registration))) {
			AZ_Error("MSQUIC", false, "RegistrationOpen failed, 0x%x!\n", Status);
			Cleanup();
		}
    }

    void QUICGemPOCSystemComponent::Cleanup()
    {
        if (m_msQuic != NULL) {
            if (m_configuration != NULL) {
                m_msQuic->ConfigurationClose(m_configuration);
            }
            if (m_registration != NULL) {
                //
                // This will block until all outstanding child objects have been
                // closed.
                //
                m_msQuic->RegistrationClose(m_registration);
            }
            MsQuicClose(m_msQuic);
        }
    }

    _IRQL_requires_max_(DISPATCH_LEVEL)
        _Function_class_(QUIC_CONNECTION_CALLBACK)
        QUIC_STATUS
        QUIC_API
        ClientConnectionCallback(
            [[maybe_unused]] _In_ HQUIC Connection,
            [[maybe_unused]] _In_opt_ void* Context,
            [[maybe_unused]] _Inout_ QUIC_CONNECTION_EVENT* Event
        )
    {
        QUICGemPOCSystemComponent* systemComponent = static_cast<QUICGemPOCSystemComponent*>(Context);

        switch (Event->Type) {
        case QUIC_CONNECTION_EVENT_CONNECTED:
            //
            // The handshake has completed for the connection.
            //
            AZ_Info("MsQuic", "[conn][%p] Connected\n", Connection);
            systemComponent->ClientSend(Connection);
            break;
        case QUIC_CONNECTION_EVENT_SHUTDOWN_INITIATED_BY_TRANSPORT:
            //
            // The connection has been shut down by the transport. Generally, this
            // is the expected way for the connection to shut down with this
            // protocol, since we let idle timeout kill the connection.
            //
            if (Event->SHUTDOWN_INITIATED_BY_TRANSPORT.Status == QUIC_STATUS_CONNECTION_IDLE) {
                AZ_Info("MsQuic", "[conn][%p] Successfully shut down on idle.\n", Connection);
            }
            else {
                AZ_Info("MsQuic", "[conn][%p] Shut down by transport, 0x%x\n", Connection, Event->SHUTDOWN_INITIATED_BY_TRANSPORT.Status);
            }
            break;
        case QUIC_CONNECTION_EVENT_SHUTDOWN_INITIATED_BY_PEER:
            //
            // The connection was explicitly shut down by the peer.
            //
            AZ_Info("MsQuic", "[conn][%p] Shut down by peer, 0x%llu\n", Connection, (unsigned long long)Event->SHUTDOWN_INITIATED_BY_PEER.ErrorCode);
            break;
        case QUIC_CONNECTION_EVENT_SHUTDOWN_COMPLETE:
            //
            // The connection has completed the shutdown process and is ready to be
            // safely cleaned up.
            //
            AZ_Info("MsQuic", "[conn][%p] All done\n", Connection);
            if (!Event->SHUTDOWN_COMPLETE.AppCloseInProgress) {
                systemComponent->ConnectionClose(Connection);
            }
            break;
        case QUIC_CONNECTION_EVENT_RESUMPTION_TICKET_RECEIVED:
            //
            // A resumption ticket (also called New Session Ticket or NST) was
            // received from the server.
            //
            AZ_Info("MsQuic", "[conn][%p] Resumption ticket received (%u bytes):\n", Connection, Event->RESUMPTION_TICKET_RECEIVED.ResumptionTicketLength);
            for (uint32_t i = 0; i < Event->RESUMPTION_TICKET_RECEIVED.ResumptionTicketLength; i++) {
                AZ_Info("MsQuic", "%.2X", (uint8_t)Event->RESUMPTION_TICKET_RECEIVED.ResumptionTicket[i]);
            }
            AZ_Info("MsQuic", "\n");
            break;
        default:
            break;
        }
        return QUIC_STATUS_SUCCESS;
    }

	//
    // The clients's callback for stream events from MsQuic.
    //
	_IRQL_requires_max_(DISPATCH_LEVEL)
		_Function_class_(QUIC_STREAM_CALLBACK)
		QUIC_STATUS
		QUIC_API
		ClientStreamCallback(
			_In_ HQUIC Stream,
			_In_opt_ void* Context,
			_Inout_ QUIC_STREAM_EVENT* Event
		)
	{
        QUICGemPOCSystemComponent* systemComponent = static_cast<QUICGemPOCSystemComponent*>(Context);

		switch (Event->Type) {
		case QUIC_STREAM_EVENT_SEND_COMPLETE:
			//
			// A previous StreamSend call has completed, and the context is being
			// returned back to the app.
			//
			free(Event->SEND_COMPLETE.ClientContext);
            AZ_Info("MsQuic", "[strm][%p] Data sent\n", Stream);
			break;
		case QUIC_STREAM_EVENT_RECEIVE:
			//
			// Data was received from the peer on the stream.
			//
            AZ_Info("MsQuic", "[strm][%p] Data received\n", Stream);
			break;
		case QUIC_STREAM_EVENT_PEER_SEND_ABORTED:
			//
			// The peer gracefully shut down its send direction of the stream.
			//
            AZ_Info("MsQuic", "[strm][%p] Peer aborted\n", Stream);
			break;
		case QUIC_STREAM_EVENT_PEER_SEND_SHUTDOWN:
			//
			// The peer aborted its send direction of the stream.
			//
            AZ_Info("MsQuic", "[strm][%p] Peer shut down\n", Stream);
			break;
		case QUIC_STREAM_EVENT_SHUTDOWN_COMPLETE:
			//
			// Both directions of the stream have been shut down and MsQuic is done
			// with the stream. It can now be safely cleaned up.
			//
            AZ_Info("MsQuic", "[strm][%p] All done\n", Stream);
			if (!Event->SHUTDOWN_COMPLETE.AppCloseInProgress) {
                systemComponent->StreamClose(Stream);
			}
			break;
		default:
			break;
		}
		return QUIC_STATUS_SUCCESS;
	}

    void QUICGemPOCSystemComponent::RunClient()
    {
		//
	    // Load the client configuration based on the "unsecure" command line option.
	    //
		if (!LoadConfiguration()) {
			return;
		}

		QUIC_STATUS Status;
		HQUIC Connection = NULL;
		//
		// Get the target / server name or IP from the command line.
		//
		AZStd::string server = "127.0.0.1";

		//
		// Allocate a new connection object.
		//
		if (QUIC_FAILED(Status = m_msQuic->ConnectionOpen(m_registration, ClientConnectionCallback, this, &Connection))) {
			AZ_Error("MSQUIC", false, "ConnectionOpen failed, 0x%x!\n", Status);
			goto Error;
		}

		AZ_Info("MSQUIC", "[conn][%p] Connecting...\n", Connection)
		//
		// Start the connection to the server.
		//
		if (QUIC_FAILED(Status = m_msQuic->ConnectionStart(Connection, m_configuration, QUIC_ADDRESS_FAMILY_UNSPEC, server.c_str(), UdpPort))) {
			AZ_Error("MSQUIC", false, "ConnectionStart failed, 0x%x!\n", Status);
			goto Error;
		}

	Error:

		if (QUIC_FAILED(Status) && Connection != NULL) {
			m_msQuic->ConnectionClose(Connection);
		}
    }
    
    bool QUICGemPOCSystemComponent::LoadConfiguration()
    {
		QUIC_SETTINGS settings = { 0 };
		//
		// Configures the client's idle timeout.
		//
		settings.IdleTimeoutMs = IdleTimeoutMs;
		settings.IsSet.IdleTimeoutMs = true;

		//
		// Configures a default client configuration, optionally disabling
		// server certificate validation.
		//
		QUIC_CREDENTIAL_CONFIG credConfig;
		memset(&credConfig, 0, sizeof(credConfig));
		credConfig.Type = QUIC_CREDENTIAL_TYPE_NONE;
		credConfig.Flags = QUIC_CREDENTIAL_FLAG_CLIENT;
		if (/*Unsecure*/true) {
			credConfig.Flags |= QUIC_CREDENTIAL_FLAG_NO_CERTIFICATE_VALIDATION;
		}

		//
		// Allocate/initialize the configuration object, with the configured ALPN
		// and settings.
		//
		QUIC_STATUS Status = QUIC_STATUS_SUCCESS;
		if (QUIC_FAILED(Status = m_msQuic->ConfigurationOpen(m_registration, &Alpn, 1, &settings, sizeof(settings), NULL, &m_configuration))) {
            AZ_Error("MSQUIC", false, "ConfigurationOpen failed, 0x%x!\n", Status);
			return false;
		}

		//
		// Loads the TLS credential part of the configuration. This is required even
		// on client side, to indicate if a certificate is required or not.
		//
		if (QUIC_FAILED(Status = m_msQuic->ConfigurationLoadCredential(m_configuration, &credConfig))) {
            AZ_Error("MSQUIC", false, "ConfigurationLoadCredential failed, 0x%x!\n", Status);
			return false;
		}

		return true;
    }

    void QUICGemPOCSystemComponent::ClientSend(HQUIC connection)
    {
		QUIC_STATUS Status;
		HQUIC Stream = NULL;
		uint8_t* SendBufferRaw;
		QUIC_BUFFER* SendBuffer;

		//
		// Create/allocate a new bidirectional stream. The stream is just allocated
		// and no QUIC stream identifier is assigned until it's started.
		//
		if (QUIC_FAILED(Status = m_msQuic->StreamOpen(connection, QUIC_STREAM_OPEN_FLAG_NONE, ClientStreamCallback, this, &Stream))) {
            AZ_Error("MSQUIC", false, "StreamOpen failed, 0x%x!\n", Status);
			goto Error;
		}

        AZ_Info("MSQUIC", "[strm][%p] Starting...\n", Stream);

		//
		// Starts the bidirectional stream. By default, the peer is not notified of
		// the stream being started until data is sent on the stream.
		//
		if (QUIC_FAILED(Status = m_msQuic->StreamStart(Stream, QUIC_STREAM_START_FLAG_NONE))) {
            AZ_Error("MSQUIC", false, "StreamStart failed, 0x%x!\n", Status);
            m_msQuic->StreamClose(Stream);
			goto Error;
		}

		//
		// Allocates and builds the buffer to send over the stream.
		//
		SendBufferRaw = (uint8_t*)malloc(sizeof(QUIC_BUFFER) + SendBufferLength);
		if (SendBufferRaw == NULL) {
            AZ_Error("MSQUIC", false, "SendBuffer allocation failed!\n");
			Status = QUIC_STATUS_OUT_OF_MEMORY;
			goto Error;
		}
		SendBuffer = (QUIC_BUFFER*)SendBufferRaw;
		SendBuffer->Buffer = SendBufferRaw + sizeof(QUIC_BUFFER);
		SendBuffer->Length = SendBufferLength;

        AZ_Info("MSQUIC", "[strm][%p] Sending data...\n", Stream);

		//
		// Sends the buffer over the stream. Note the FIN flag is passed along with
		// the buffer. This indicates this is the last buffer on the stream and the
		// the stream is shut down (in the send direction) immediately after.
		//
		if (QUIC_FAILED(Status = m_msQuic->StreamSend(Stream, SendBuffer, 1, QUIC_SEND_FLAG_FIN, SendBuffer))) {
            AZ_Error("MSQUIC", false, "StreamSend failed, 0x%x!\n", Status);
			free(SendBufferRaw);
			goto Error;
		}

	Error:

		if (QUIC_FAILED(Status)) {
            m_msQuic->ConnectionShutdown(connection, QUIC_CONNECTION_SHUTDOWN_FLAG_NONE, 0);
		}
    }

    void QUICGemPOCSystemComponent::ConnectionClose(HQUIC connection)
    {
        if (m_msQuic) {
            m_msQuic->ConnectionClose(connection);
        }
    }

    void QUICGemPOCSystemComponent::StreamClose(HQUIC stream)
    {
		if (m_msQuic) {
			m_msQuic->StreamClose(stream);
		}
    }
} // namespace QUICGemPOC
