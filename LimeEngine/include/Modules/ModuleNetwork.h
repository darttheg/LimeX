#pragma once

#include <sol/forward.hpp>
#include <string>

class Application;
class Packet;

namespace Module {
	namespace Network {
		void bind(Application* app);

		namespace Bind {
			void Host(int port, int maxPlayers = 32);
			void Connect(const std::string& ip, int port);
			void Disconnect();
			bool IsHosting();
			bool IsConnected();
			void SetBandwidthLimits(int incoming, int outgoing);
			int GetPeerState(int peerID);
			int GetPeerPing(int peerID);
			void DisconnectPeer(int peerID, int reason = 0);
			int BanPeer(int peerID, int reason = 0);
			int GetPeerIP(int peerID);
			int GetPeerCount();
			void BanIP(int IP);
			void UnbanIP(int IP);
			void ClearBannedIPs();

			void SendPacketToPeer(const Packet& p, int peerID, int channel = 0, bool reliable = true); // Server
			void SendPacketToServer(const Packet& p, int channel = 0, bool reliable = true); // Peer
			void SendPacketToAll(const Packet& p, int channel = 0, bool reliable = true); // All
		}
	}
}