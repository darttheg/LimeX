#pragma once
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <vector>
#include <cstdint>
#include <atomic>
#include <functional>
#include <unordered_set>
#include <sol/forward.hpp>

class Application;
class Event;
class Packet;

#include <enet/enet.h>

template<typename T>
class ThreadQueue {
public:
	void push(T val) {
		std::lock_guard<std::mutex> lock(m);
		q.push(std::move(val));
	}

	bool pop(T& out) {
		std::lock_guard<std::mutex> lock(m);
		if (q.empty()) return false;
		out = std::move(q.front());
		q.pop();
		return true;
	}
private:
	std::queue<T> q;
	std::mutex m;
};

struct NetEvent {
	enum class Type { Connect, Disconnect, Receive };
	Type type;
	uint32_t peerID = 0;
	std::vector<uint8_t> data;
	uint8_t reason = 0;
};

struct NetSend {
	std::vector<uint8_t> data;
	uint8_t channel = 0;
	bool reliable = true;
	int peerID = -1; // broadcast from server or serverPeer
};

class NetworkManager {
public:
	NetworkManager(Application* owner);

	bool Init();
	void Update();
	void Shutdown();

	void host(int port, int maxPlayers = 32);
	void connect(const std::string& ip, int port);
	void disconnect(); // Stops hosting as server / disconnects as peer

	void sendPacket(const Packet& p, int peerID = -1, int channel = 0, bool reliable = true); // Broadcasts by default (ID = -1)

	// Bindables
	bool isHosting() const { return server != nullptr; }
	bool isConnected() const { return client != nullptr; }
	void setBandwidthLimits(int incoming, int outgoing); // bytes per second
	int getPeerState(int peerID) const; // Lime.Enum.PeerState
	int getPeerPing(int peerID) const;
	void disconnectPeer(int peerID, int reason = 0);
	int banPeer(int peerID, int reason = 0);
	int getPeerIP(int peerID) const;
	int getPeerCount() const;

	void sendPacketToPeer(const Packet& p, int peerID, int channel = 0, bool reliable = true); // Server
	void sendPacketToServer(const Packet& p, int channel = 0, bool reliable = true); // Peer
	void sendPacketToAll(const Packet& p, int channel = 0, bool reliable = true); // All

	void banIP(sol::variadic_args args);
	void unbanIP(sol::variadic_args args);
	void clearBannedIPs() { autoRejectIPs.clear(); }
	sol::table getBannedIPs();

	// Events
	std::shared_ptr<Event> LimeOnPeerConnect = nullptr;
	std::shared_ptr<Event> LimeOnPeerDisonnect = nullptr;
	std::shared_ptr<Event> LimeOnConnect = nullptr;
	std::shared_ptr<Event> LimeOnDisconnect = nullptr;
	std::shared_ptr<Event> LimeOnReceive = nullptr;
private:
	bool initialized = false;

	ENetHost* server = nullptr;
	ENetHost* client = nullptr;
	ENetPeer* serverPeer = nullptr;

	std::thread netThread;
	std::atomic<bool> running{ false };

	ThreadQueue<NetEvent> inbound;
	ThreadQueue<NetSend> outbound;

	void loop();
	void pollHost(ENetHost* host, bool isServer);
	void flushOutbound();

	std::unordered_set<uint32_t> autoRejectIPs;

	ENetPeer* getPeer(int id) const;
};