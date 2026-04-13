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

	if (ok) {
		LimeOnPeerConnect = std::make_shared<Event>();
		LimeOnPeerDisonnect = std::make_shared<Event>();
		LimeOnConnect = std::make_shared<Event>();
		LimeOnDisconnect = std::make_shared<Event>();
		LimeOnReceive = std::make_shared<Event>();
	}

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
				// Lime.Network.onDisconnect
				LimeOnDisconnect.get()->engineRun([&](const std::string& msg) { d->PostError(msg); });
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

void NetworkManager::sendPacket(const Packet& p, int channel, bool reliable, int peerID) {
	NetSend out;
	out.data = p.getData();
	out.channel = channel;
	out.reliable = reliable;
	out.peerID = peerID;
	outbound.push(std::move(out));
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
				NetEvent ne;
				ne.type = NetEvent::Type::Connect;
				ne.peerID = isServer ? event.peer->incomingPeerID : 0;
				inbound.push(std::move(ne));
				break;
			}
		case ENET_EVENT_TYPE_DISCONNECT: {
			NetEvent ne;
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
