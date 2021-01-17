#pragma once
#include <isteamclient.h>
#include <isteamnetworking.h>

#include <Windows.h>

interface SteamNetworkingWrapper : public ISteamNetworking
{
public:
	SteamNetworkingWrapper(ISteamNetworking**);
	~SteamNetworkingWrapper();

	ISteamNetworking* m_SteamNetworking;

	bool SendP2PPacket(CSteamID steamIDRemote, const void *pubData, uint32 cubData, EP2PSend eP2PSendType, int nChannel = 0);
	bool IsP2PPacketAvailable(uint32 *pcubMsgSize, int nChannel = 0);
	bool ReadP2PPacket(void *pubDest, uint32 cubDest, uint32 *pcubMsgSize, CSteamID *psteamIDRemote, int nChannel = 0);
	bool AcceptP2PSessionWithUser(CSteamID steamIDRemote);
	bool CloseP2PSessionWithUser(CSteamID steamIDRemote);
	bool CloseP2PChannelWithUser(CSteamID steamIDRemote, int nChannel);
	bool GetP2PSessionState(CSteamID steamIDRemote, P2PSessionState_t *pConnectionState);
	bool AllowP2PPacketRelay(bool bAllow);
	SNetListenSocket_t CreateListenSocket(int nVirtualP2PPort, uint32 nIP, uint16 nPort, bool bAllowUseOfPacketRelay);
	SNetSocket_t CreateP2PConnectionSocket(CSteamID steamIDTarget, int nVirtualPort, int nTimeoutSec, bool bAllowUseOfPacketRelay);
	SNetSocket_t CreateConnectionSocket(uint32 nIP, uint16 nPort, int nTimeoutSec);
	bool DestroySocket(SNetSocket_t hSocket, bool bNotifyRemoteEnd);
	bool DestroyListenSocket(SNetListenSocket_t hSocket, bool bNotifyRemoteEnd);
	bool SendDataOnSocket(SNetSocket_t hSocket, void *pubData, uint32 cubData, bool bReliable);
	bool IsDataAvailableOnSocket(SNetSocket_t hSocket, uint32 *pcubMsgSize);
	bool RetrieveDataFromSocket(SNetSocket_t hSocket, void *pubDest, uint32 cubDest, uint32 *pcubMsgSize);
	bool IsDataAvailable(SNetListenSocket_t hListenSocket, uint32 *pcubMsgSize, SNetSocket_t *phSocket);
	bool RetrieveData(SNetListenSocket_t hListenSocket, void *pubDest, uint32 cubDest, uint32 *pcubMsgSize, SNetSocket_t *phSocket);
	bool GetSocketInfo(SNetSocket_t hSocket, CSteamID *pSteamIDRemote, int *peSocketStatus, uint32 *punIPRemote, uint16 *punPortRemote);
	bool GetListenSocketInfo(SNetListenSocket_t hListenSocket, uint32 *pnIP, uint16 *pnPort);
	ESNetSocketConnectionType GetSocketConnectionType(SNetSocket_t hSocket);
	int GetMaxPacketSize(SNetSocket_t hSocket);
};