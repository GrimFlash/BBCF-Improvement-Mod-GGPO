#include "SteamNetworkingWrapper.h"

#include "Core/interfaces.h"
#include "Core/logger.h"
#include "Core/utils.h"
#include "Game/gamestates.h"
#include "Overlay/Logger/ImGuiLogger.h"

SteamNetworkingWrapper::SteamNetworkingWrapper(ISteamNetworking** pSteamNetworking)
{
	LOG(2, "SteamNetworkingWrapper\n");
	LOG(2, "\t- before: *pSteamNetworking: 0x%p, thispointer: 0x%p\n", *pSteamNetworking, this);

	m_SteamNetworking = *pSteamNetworking;
	void* thisAddress = this;
	WriteToProtectedMemory((uintptr_t)pSteamNetworking, (char*)&thisAddress, 4); //basically *pSteamNetworking = this;

	LOG(2, "\t- after: *pSteamNetworking: 0x%p, m_SteamNetworking: 0x%p\n", *pSteamNetworking, m_SteamNetworking);
}

SteamNetworkingWrapper::~SteamNetworkingWrapper()
{
}

bool SteamNetworkingWrapper::SendP2PPacket(CSteamID steamIDRemote, const void *pubData, uint32 cubData, EP2PSend eP2PSendType, int nChannel)
{ 
	LOG(7, "SteamNetworkingWrapper SendP2PPacket\n");

	// Log outgoing data:
	// LOG(2, "\t- %s\n", RawMemoryArrayToString((unsigned char*)pubData, cubData));

	return m_SteamNetworking->SendP2PPacket(steamIDRemote, pubData, cubData, eP2PSendType, nChannel);
}

bool SteamNetworkingWrapper::IsP2PPacketAvailable(uint32 *pcubMsgSize, int nChannel)
{ 
	LOG(7, "SteamNetworkingWrapper IsP2PPacketAvailable\n"); 
	return m_SteamNetworking->IsP2PPacketAvailable(pcubMsgSize, nChannel);
}

bool SteamNetworkingWrapper::ReadP2PPacket(void *pubDest, uint32 cubDest, uint32 *pcubMsgSize, CSteamID *psteamIDRemote, int nChannel)
{ 
	LOG(7, "SteamNetworkingWrapper ReadP2PPacket\n");

	bool ret = m_SteamNetworking->ReadP2PPacket(pubDest, cubDest, pcubMsgSize, psteamIDRemote, nChannel);

	// Log incoming data:
	// LOG(2, "\t- %s\n", RawMemoryArrayToString((unsigned char*)pubDest, *pcubMsgSize));

	return ret;
}

bool SteamNetworkingWrapper::AcceptP2PSessionWithUser(CSteamID steamIDRemote)
{
	LOG(7, "SteamNetworkingWrapper AcceptP2PSessionWithUser\n"); 
	return m_SteamNetworking->AcceptP2PSessionWithUser(steamIDRemote);
}

bool SteamNetworkingWrapper::CloseP2PSessionWithUser(CSteamID steamIDRemote)
{
	LOG(7, "SteamNetworkingWrapper CloseP2PSessionWithUser\n");
	return m_SteamNetworking->CloseP2PSessionWithUser(steamIDRemote);
}

bool SteamNetworkingWrapper::CloseP2PChannelWithUser(CSteamID steamIDRemote, int nChannel)
{
	LOG(7, "SteamNetworkingWrapper CloseP2PChannelWithUser\n");
	return m_SteamNetworking->CloseP2PChannelWithUser(steamIDRemote, nChannel);
}

bool SteamNetworkingWrapper::GetP2PSessionState(CSteamID steamIDRemote, P2PSessionState_t *pConnectionState)
{
	LOG(7, "SteamNetworkingWrapper GetP2PSessionState\n");
	return m_SteamNetworking->GetP2PSessionState(steamIDRemote, pConnectionState);
}

bool SteamNetworkingWrapper::AllowP2PPacketRelay(bool bAllow)
{
	LOG(7, "SteamNetworkingWrapper AllowP2PPacketRelay\n");
	return m_SteamNetworking->AllowP2PPacketRelay(bAllow);
}

SNetListenSocket_t SteamNetworkingWrapper::CreateListenSocket(int nVirtualP2PPort, uint32 nIP, uint16 nPort, bool bAllowUseOfPacketRelay)
{
	LOG(7, "SteamNetworkingWrapper CreateListenSocket\n");
	return m_SteamNetworking->CreateListenSocket(nVirtualP2PPort, nIP, nPort, bAllowUseOfPacketRelay);
}

SNetSocket_t SteamNetworkingWrapper::CreateP2PConnectionSocket(CSteamID steamIDTarget, int nVirtualPort, int nTimeoutSec, bool bAllowUseOfPacketRelay)
{
	LOG(2, "SteamNetworkingWrapper CreateP2PConnectionSocket\n");
	return m_SteamNetworking->CreateP2PConnectionSocket(steamIDTarget, nVirtualPort, nTimeoutSec, bAllowUseOfPacketRelay);
}

SNetSocket_t SteamNetworkingWrapper::CreateConnectionSocket(uint32 nIP, uint16 nPort, int nTimeoutSec)
{
	LOG(7, "SteamNetworkingWrapper CreateConnectionSocket\n");
	return m_SteamNetworking->CreateConnectionSocket(nIP, nPort, nTimeoutSec);
}

bool SteamNetworkingWrapper::DestroySocket(SNetSocket_t hSocket, bool bNotifyRemoteEnd)
{
	LOG(7, "SteamNetworkingWrapper DestroySocket\n");
	return m_SteamNetworking->DestroySocket(hSocket, bNotifyRemoteEnd);
}

bool SteamNetworkingWrapper::DestroyListenSocket(SNetListenSocket_t hSocket, bool bNotifyRemoteEnd)
{
	LOG(7, "SteamNetworkingWrapper DestroyListenSocket\n");
	return m_SteamNetworking->DestroyListenSocket(hSocket, bNotifyRemoteEnd);
}

bool SteamNetworkingWrapper::SendDataOnSocket(SNetSocket_t hSocket, void *pubData, uint32 cubData, bool bReliable)
{
	LOG(7, "SteamNetworkingWrapper SendDataOnSocket\n");
	return m_SteamNetworking->SendDataOnSocket(hSocket, pubData, cubData, bReliable);
}

bool SteamNetworkingWrapper::IsDataAvailableOnSocket(SNetSocket_t hSocket, uint32 *pcubMsgSize)
{
	LOG(7, "SteamNetworkingWrapper IsDataAvailableOnSocket\n");
	return m_SteamNetworking->IsDataAvailableOnSocket(hSocket, pcubMsgSize);
}

bool SteamNetworkingWrapper::RetrieveDataFromSocket(SNetSocket_t hSocket, void *pubDest, uint32 cubDest, uint32 *pcubMsgSize)
{
	LOG(7, "SteamNetworkingWrapper RetrieveDataFromSocket\n");
	return m_SteamNetworking->RetrieveDataFromSocket(hSocket, pubDest, cubDest, pcubMsgSize);
}

bool SteamNetworkingWrapper::IsDataAvailable(SNetListenSocket_t hListenSocket, uint32 *pcubMsgSize, SNetSocket_t *phSocket)
{
	LOG(7, "SteamNetworkingWrapper IsDataAvailable\n");
	return m_SteamNetworking->IsDataAvailable(hListenSocket, pcubMsgSize, phSocket);
}

bool SteamNetworkingWrapper::RetrieveData(SNetListenSocket_t hListenSocket, void *pubDest, uint32 cubDest, uint32 *pcubMsgSize, SNetSocket_t *phSocket)
{
	LOG(7, "SteamNetworkingWrapper RetrieveData\n");
	return m_SteamNetworking->RetrieveData(hListenSocket, pubDest, cubDest, pcubMsgSize, phSocket);
}

bool SteamNetworkingWrapper::GetSocketInfo(SNetSocket_t hSocket, CSteamID *pSteamIDRemote, int *peSocketStatus, uint32 *punIPRemote, uint16 *punPortRemote)
{
	LOG(7, "SteamNetworkingWrapper GetSocketInfo\n");
	return m_SteamNetworking->GetSocketInfo(hSocket, pSteamIDRemote, peSocketStatus, punIPRemote, punPortRemote);
}

bool SteamNetworkingWrapper::GetListenSocketInfo(SNetListenSocket_t hListenSocket, uint32 *pnIP, uint16 *pnPort)
{
	LOG(7, "SteamNetworkingWrapper GetListenSocketInfo\n");
	return m_SteamNetworking->GetListenSocketInfo(hListenSocket, pnIP, pnPort);
}

ESNetSocketConnectionType SteamNetworkingWrapper::GetSocketConnectionType(SNetSocket_t hSocket)
{
	LOG(7, "SteamNetworkingWrapper GetSocketConnectionType\n");
	return m_SteamNetworking->GetSocketConnectionType(hSocket);
}

int SteamNetworkingWrapper::GetMaxPacketSize(SNetSocket_t hSocket)
{
	LOG(7, "SteamNetworkingWrapper GetMaxPacketSize\n");
	return m_SteamNetworking->GetMaxPacketSize(hSocket);
}
