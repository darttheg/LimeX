#include "NetworkManager.h"
#include "Application.h"
#include "DebugConsole.h"

#include "Objects/Event.h"
#include "Objects/Packet.h"

static Application* a = nullptr;
static DebugConsole* d = nullptr;

NetworkManager::NetworkManager(Application* owner) {
	a = owner;
	d = owner->GetDebugConsole();
}

bool NetworkManager::Init() {
	bool ok = enet_initialize() == 0;
	initialized = ok;
	return ok;
}

void NetworkManager::Update() {
	NetEvent event;
	while (inbound.pop(event)) {
		switch (event.type) {
		case NetEvent::Type::Connect:
			if (event.peerID == 0) {
				// Lime.Network.onConnect
				LimeOnConnect.get()->engineRun([&](const std::string& msg) { d->PostError(msg); });
			} else {
				// Lime.Network.onPeerConnected
				LimeOnPeerConnect.get()->engineRun([&](const std::string& msg) { d->PostError(msg); }, event.peerID);
			}
			break;
		case NetEvent::Type::Disconnect:
			if (event.peerID == 0) {
				int reason = event.reason;
				// Lime.Network.onDisconnect
				LimeOnDisconnect.get()->engineRun([&](const std::string& msg) { d->PostError(msg); }, reason);
			} else {
				// Lime.Network.onPeerDisconnected
				LimeOnPeerDisonnect.get()->engineRun([&](const std::string& msg) { d->PostError(msg); }, event.peerID);
			}
			break;
		case NetEvent::Type::Receive:
			Packet p(event.data);
			// Lime.Network.onPacketReceived
			LimeOnReceive.get()->engineRun([&](const std::string& msg) { d->PostError(msg); }, p, event.peerID); // peerID is only used by server
			break;
		}
	}
}

void NetworkManager::Shutdown() {
	disconnect();
	enet_deinitialize();
}

void NetworkManager::host(int port, int maxPlayers) {
	if (!initialized) {
		d->PostError("Cannot host as networking is not initialized");
		return;
	}

	if (server || client) {
		d->PostError("Cannot host a server while already connected to one");
		return;
	}

	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = port;
	server = enet_host_create(&address, maxPlayers, 2, 0, 0);

	if (!server) {
		d->PostError("Failed to host server");
		return;
	}

	running = true;
	netThread = std::thread(&NetworkManager::loop, this);
}

void NetworkManager::connect(const std::string& ip, int port) {
	if (!initialized) {
		d->PostError("Cannot connect as networking is not initialized");
		return;
	}

	if (server || client) {
		d->PostError("Cannot connect to another server while connected to an existing one");
		return;
	}

	client = enet_host_create(nullptr, 1, 2, 0, 0);
	if (!client) {
		d->PostError("Failed to create client");
		return;
	}

	ENetAddress address;
	enet_address_set_host(&address, ip.c_str());
	address.port = port;
	serverPeer = enet_host_connect(client, &address, 2, 0);
	if (!serverPeer) {
		d->PostError("Failed to create client");
		enet_host_destroy(client);
		client = nullptr;
		return;
	}

	running = true;
	netThread = std::thread(&NetworkManager::loop, this);
}

void NetworkManager::disconnect() {
	running = false;
	if (netThread.joinable()) netThread.join();

	if (serverPeer) {
		enet_peer_disconnect_now(serverPeer, 0);
		serverPeer = nullptr;
	}

	if (client) {
		enet_host_destroy(client);
		client = nullptr;
	}

	if (server) {
		enet_host_destroy(server);
		server = nullptr;
	}
}

void NetworkManager::sendPacket(const Packet& p, int peerID, int channel, bool reliable) {
	NetSend out;
	out.data = p.getData();
	out.channel = channel;
	out.reliable = reliable;
	out.peerID = peerID;
	outbound.push(std::move(out));
}

void NetworkManager::setBandwidthLimits(int incoming, int outgoing) {
	if (!server) {
		d->Warn("Cannot set bandwidth limits without being the host");
		return;
	}

	if (incoming < 0) incoming = 0;
	if (outgoing < 0) outgoing = 0;

	enet_host_bandwidth_limit(server, incoming, outgoing);
}

int NetworkManager::getPeerState(int peerID) const {
	if (!server) {
		d->Warn("Cannot get peer state without being the host");
		return -1;
	}

	auto* out = getPeer(peerID);
	if (!out) d->Warn("Invalid peer ID " + std::to_string(peerID));
	return out ? static_cast<int>(out->state) : -1;
}

int NetworkManager::getPeerPing(int peerID) const {
	if (!server) {
		d->Warn("Cannot get peer ping without being the host");
		return -1;
	}

	auto* out = getPeer(peerID);
	if (!out) d->Warn("Invalid peer ID " + std::to_string(peerID));
	return out ? static_cast<int>(out->roundTripTime) : -1;
}

void NetworkManager::disconnectPeer(int peerID, int reason) {
	if (!server) {
		d->Warn("Cannot disconnect a peer without being the host");
		return;
	}

	auto* out = getPeer(peerID);
	if (!out) {
		d->Warn("Invalid peer ID " + std::to_string(peerID));
		return;
	}

	enet_peer_disconnect(out, reason);
}

int NetworkManager::banPeer(int peerID, int reason) {
	if (!server) {
		d->Warn("Cannot ban a peer without being the host");
		return -1;
	}

	auto* out = getPeer(peerID);
	if (!out) {
		d->Warn("Invalid peer ID " + std::to_string(peerID));
		return -1;
	}

	int ip = out->address.host;
	banIP(ip);
	disconnectPeer(peerID, reason);
	return ip;
}

int NetworkManager::getPeerIP(int peerID) const {
	if (!server) {
		d->Warn("Cannot get a peer's IP without being the host");
		return 0;
	}

	auto* out = getPeer(peerID);
	if (!out) {
		d->Warn("Invalid peer ID " + std::to_string(peerID));
		return 0;
	}

	return out ? out->address.host : 0;
}

int NetworkManager::getPeerCount() const {
	if (!server) {
		d->Warn("Cannot get number of peers without being the host");
		return 0;
	}

	return server->connectedPeers;
}

void NetworkManager::sendPacketToPeer(const Packet& p, int peerID, int channel, bool reliable) {
	if (!server) {
		d->Warn("Cannot send a Packet to a peer without being the host");
		return;
	}

	auto* peer = getPeer(peerID);
	if (peerID < 0 || !peer) {
		d->Warn("Invalid peer ID " + std::to_string(peerID));
		return;
	}

	sendPacket(p, peerID, channel, reliable);
}

void NetworkManager::sendPacketToServer(const Packet& p, int channel, bool reliable) {
	if (server) {
		d->Warn("Cannot send a Packet to self");
		return;
	}

	sendPacket(p, -1, channel, reliable); // -1 -> server
}

void NetworkManager::sendPacketToAll(const Packet& p, int channel, bool reliable) {
	if (!server) {
		d->Warn("Cannot send a Packet to all peers without being the host");
		return;
	}

	sendPacket(p, -1, channel, reliable);
}

void NetworkManager::banIP(int IP) {
	autoRejectIPs.insert(IP);
}

void NetworkManager::unbanIP(int IP) {
	autoRejectIPs.erase(IP);
}

void NetworkManager::loop() {
	while (running) {
		flushOutbound();
		if (server) pollHost(server, true);
		if (client) pollHost(client, false);
	}
}

void NetworkManager::pollHost(ENetHost* host, bool isServer) {
	ENetEvent event;
	while (enet_host_service(host, &event, 1) > 0) {
		switch (event.type) {
		case ENET_EVENT_TYPE_CONNECT: {
			if (isServer && autoRejectIPs.find(event.peer->address.host) != autoRejectIPs.end()) {
				enet_peer_reset(event.peer);
				break;
			}

			NetEvent ne;
			ne.type = NetEvent::Type::Connect;
			ne.peerID = isServer ? event.peer->incomingPeerID : 0;
			inbound.push(std::move(ne));
			break;
			}
		case ENET_EVENT_TYPE_DISCONNECT: {
			NetEvent ne;
			ne.reason = event.data;
			ne.type = NetEvent::Type::Disconnect;
			ne.peerID = isServer ? event.peer->incomingPeerID : 0;
			inbound.push(std::move(ne));
			break;
			}
		case ENET_EVENT_TYPE_RECEIVE: {
				NetEvent ne;
				ne.type = NetEvent::Type::Receive;
				ne.peerID = isServer ? event.peer->incomingPeerID : 0;
				ne.data.assign(event.packet->data, event.packet->data + event.packet->dataLength);
				enet_packet_destroy(event.packet);
				inbound.push(std::move(ne));
				break;
			}
		}
	}
}

void NetworkManager::flushOutbound() {
	NetSend s;
	while (outbound.pop(s)) {
		uint32_t flags = s.reliable ? ENET_PACKET_FLAG_RELIABLE : 0;
		ENetPacket* packet = enet_packet_create(s.data.data(), s.data.size(), flags);

		if (server) {
			if (s.peerID == -1) enet_host_broadcast(server, s.channel, packet);
			else {
				ENetPeer* peer = &server->peers[s.peerID];
				if (peer) enet_peer_send(peer, s.channel, packet);
			}
		} else if (client && serverPeer)
			enet_peer_send(serverPeer, s.channel, packet);
	}
}

ENetPeer* NetworkManager::getPeer(int id) const {
	if (!server) return nullptr;
	if (id < 0 || id > server->peerCount) return nullptr;
	return &server->peers[id];
}
