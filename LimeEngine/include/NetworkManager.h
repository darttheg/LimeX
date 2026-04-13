#pragma once
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <vector>
#include <cstdint>
#include <atomic>
#include <functional>
#include <sol/forward.hpp>

class Application;
class Event;

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
	void disconnect();

	bool isHost() const { return server != nullptr; }
	bool isConnected() const { return client != nullptr; }
private:
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

	std::shared_ptr<Event> LimeOnPeerConnect = nullptr;
	std::shared_ptr<Event> LimeOnPeerDisonnect = nullptr;
	std::shared_ptr<Event> LimeOnConnect = nullptr;
	std::shared_ptr<Event> LimeOnDisconnect = nullptr;
	std::shared_ptr<Event> LimeOnReceive = nullptr;
};