#include "Modules/ModuleNetwork.h"

#include "Application.h"
#include "NetworkManager.h"
#include "Objects/Event.h"
#include "Objects/Packet.h"

#include <sol/sol.hpp>

static Application* a;
static NetworkManager* n;

void Module::Network::bind(Application* app) {
	a = app;
	n = app->GetNetworkManager();
	sol::state& lua = app->GetLuaState();

	// Module Lime.Network
	sol::table module = lua["Lime"]["Network"].get_or_create<sol::table>();

	n->LimeOnConnect = std::make_shared<Event>();
	n->LimeOnDisconnect = std::make_shared<Event>();
	n->LimeOnPeerConnect = std::make_shared<Event>();
	n->LimeOnPeerDisonnect = std::make_shared<Event>();
	n->LimeOnReceive = std::make_shared<Event>();

	// Field Event onConnect, Event called by Lime as a **peer** when the client connects to a server.
	module["onConnect"] = n->LimeOnConnect;
	// Field Event onDisconnect, Event called by Lime as a **peer** when the client disconnects from a server.
	// Params number code
	module["onDisconnect"] = n->LimeOnDisconnect;
	// Field Event onPeerConnect, Event called by Lime as the **host** when a peer joins the server.
	// Params number peerID
	module["onPeerConnect"] = n->LimeOnPeerConnect;
	// Field Event onPeerDisconnect, Event called by Lime as the **host** when a peer disconnects from the server.
	// Params number peerID
	module["onPeerDisconnect"] = n->LimeOnPeerDisonnect;
	// Field Event onReceive, Event called by Lime when the application receives a `Packet`. If the application is a **peer**, `peerID` will be -1.
	// Params Packet received, number peerID
	module["onReceive"] = n->LimeOnReceive;

	// Hosts a server.
	// Params number port, number? maxPlayers
	// Returns void
	module.set_function("host", &Module::Network::Bind::Host);

	// Attempts to connect to a server.
	// Params string ip, int port
	// Returns void
	module.set_function("connect", &Module::Network::Bind::Connect);

	// Disconnects from a server. If this application is the host, it will close the server.
	// Returns void
	module.set_function("disconnect", &Module::Network::Bind::Disconnect);

	// Returns true if this application is hosting a server.
	// Returns boolean
	module.set_function("isHosting", &Module::Network::Bind::IsHosting);

	// Returns true if this application is connected to a server.
	// Returns boolean
	module.set_function("isConnected", &Module::Network::Bind::IsConnected);

	// [s] Sets the incoming and outgoing bandwidth limits in bytes per second.
	// Params number incoming, number outgoing
	// Returns void
	module.set_function("setBandwidthLimits", &Module::Network::Bind::SetBandwidthLimits);

	// [s] Returns the state of a peer.
	// Params number peerID
	// Returns Lime.Enum.PeerState
	module.set_function("getPeerState", &Module::Network::Bind::GetPeerState);

	// [s] Returns the ping of a peer in milliseconds.
	// Params number peerID
	// Returns number
	module.set_function("getPeerPing", &Module::Network::Bind::GetPeerPing);

	// [s] Forcefully disconnects a peer with an optional reason code.
	// Params number peerID, number? code
	// Returns void
	module.set_function("disconnectPeer", &Module::Network::Bind::DisconnectPeer);

	// [s] Forcefully disconnects a peer with an optional reason code, as well as appending the peer's IP address to the bans list. Returns the peer's IP address.
	// Params number peerID, number? code
	// Returns number
	module.set_function("banPeer", &Module::Network::Bind::BanPeer);

	// [s] Returns the IP address of a peer.
	// Params number peerID
	// Returns number
	module.set_function("getPeerIP", &Module::Network::Bind::GetPeerIP);

	// [s] Returns the number of connected peers.
	// Returns number
	module.set_function("getPeerCount", &Module::Network::Bind::GetPeerCount);

	// Bans peers under the provided IP address from connecting to the server. A peer attempting to connect under this IP will be immediately rejected.
	// Params number ip
	// Returns void
	module.set_function("banIP", &Module::Network::Bind::BanIP);

	// Unbans an IP address.
	// Params number ip
	// Returns void
	module.set_function("unbanIP", &Module::Network::Bind::UnbanIP);

	// Unbans all IP addresses.
	// Returns void
	module.set_function("clearBannedIPs", &Module::Network::Bind::ClearBannedIPs);

	// [s] Sends a packet to a peer.
	// Params Packet packet, number peerID, number? channel, boolean? reliable
	// Returns void
	module.set_function("sendPacketToPeer", &Module::Network::Bind::SendPacketToPeer);

	// [p] Sends a packet to the server.
	// Params Packet packet, number? channel, boolean? reliable
	// Returns void
	module.set_function("sendPacketToServer", &Module::Network::Bind::SendPacketToServer);

	// [s] Sends a packet to all connected peers.
	// Params Packet packet, number? channel, boolean? reliable
	// Returns void
	module.set_function("sendPacketToAll", &Module::Network::Bind::SendPacketToAll);

	// End Module
}

// Functions

void Module::Network::Bind::Host(int port, int maxPlayers) {
	n->host(port, maxPlayers);
}

void Module::Network::Bind::Connect(const std::string& ip, int port) {
	n->connect(ip, port);
}

void Module::Network::Bind::Disconnect() {
	n->disconnect();
}

bool Module::Network::Bind::IsHosting() {
	return n->isHosting();
}

bool Module::Network::Bind::IsConnected() {
	return n->isConnected();
}

void Module::Network::Bind::SetBandwidthLimits(int incoming, int outgoing) {
	n->setBandwidthLimits(incoming, outgoing);
}

int Module::Network::Bind::GetPeerState(int peerID) {
	return n->getPeerState(peerID);
}

int Module::Network::Bind::GetPeerPing(int peerID) {
	return n->getPeerPing(peerID);
}

void Module::Network::Bind::DisconnectPeer(int peerID, int reason) {
	n->disconnectPeer(peerID, reason);
}

int Module::Network::Bind::BanPeer(int peerID, int reason) {
	return n->banPeer(peerID, reason);
}

int Module::Network::Bind::GetPeerIP(int peerID) {
	return n->getPeerIP(peerID);
}

int Module::Network::Bind::GetPeerCount() {
	return n->getPeerCount();
}

void Module::Network::Bind::BanIP(sol::variadic_args args) {
	n->banIP(args);
}

void Module::Network::Bind::UnbanIP(sol::variadic_args args) {
	n->unbanIP(args);
}

void Module::Network::Bind::ClearBannedIPs() {
	n->clearBannedIPs();
}

sol::table Module::Network::Bind::GetBannedIPs() {
	return n->getBannedIPs();
}

void Module::Network::Bind::SendPacketToPeer(const Packet& p, int peerID, int channel, bool reliable) {
	n->sendPacketToPeer(p, peerID, channel, reliable);
}

void Module::Network::Bind::SendPacketToServer(const Packet& p, int channel, bool reliable) {
	n->sendPacketToServer(p, channel, reliable);
}

void Module::Network::Bind::SendPacketToAll(const Packet& p, int channel, bool reliable) {
	n->sendPacketToAll(p, channel, reliable);
}
