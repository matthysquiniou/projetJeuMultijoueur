#pragma once
#include "PayloadTrait.h"
#include "PacketData.h"
#include "PacketToSend.h"
#include "PacketManager.h"

class PacketFactory
{
public:
    template<typename PayloadData>
    static Packet CreatePacket(const PacketData<PayloadData>& packetData, const sockaddr_in& to)
    {
        Packet packet;
        packet.from = to;

        const size_t size = sizeof(PacketHeader) + sizeof(PayloadData);
        packet.data.resize(size);

        std::memcpy(packet.data.data(), &packetData.header, sizeof(PacketHeader));
        std::memcpy(packet.data.data() + sizeof(PacketHeader), &packetData.data, sizeof(PayloadData));

        return packet;
    }

    template<typename PayloadData>
    static Packet CreatePacket(const PacketData<PayloadData>& packetData)
    {
        Packet packet;

        const size_t size = sizeof(PacketHeader) + sizeof(PayloadData);
        packet.data.resize(size);

        std::memcpy(packet.data.data(), &packetData.header, sizeof(PacketHeader));
        std::memcpy(packet.data.data() + sizeof(PacketHeader), &packetData.data, sizeof(PayloadData));

        return packet;
    }

    template<typename PayloadData>
    static PacketToSend CreatePacketToSend(
        const PacketData<PayloadData>& packetData,
        const bool replicate = false,
        const uint8_t maxReplicateCount = 0,
        std::function<void(PacketToSend&, PacketStatus, PacketManagerData&)> onComplete = nullptr
    )
    {
        Packet packet = CreatePacket<PayloadData>(packetData);
        PacketToSend packetToSend;
        packetToSend.data = packet.data;
        packetToSend.from = packet.from;
        packetToSend.packetID = packetData.header.id;
        packetToSend.replicate = replicate;
        packetToSend.maxReplicateCount = maxReplicateCount;
        packetToSend.onComplete = onComplete;
        return packetToSend;
    }

    template<typename PayloadData>
    static bool TryParsePaket(const Packet& packet, PacketData<PayloadData>& out)
    {
        if (packet.data.size() != sizeof(PacketHeader) + sizeof(PayloadData))
            return false;

        auto* header = reinterpret_cast<const PacketHeader*>(packet.data.data());

        if (header->payloadType != static_cast<uint8_t>(PayloadTraits<PayloadData>::type))
            return false;

        std::memcpy(&out.header, packet.data.data(), sizeof(PacketHeader));
        std::memcpy(&out.data, packet.data.data() + sizeof(PacketHeader), sizeof(PayloadData));

        return true;
    }

    template<typename PayloadData>
    static PacketData<PayloadData> MakePacketData(uint32_t id, uint32_t frameId, HeaderFlags flags, bool needACK, const PayloadData& payload)
    {
        PacketData<PayloadData> packetData;
        packetData.header.id = id;
        packetData.header.frameId = frameId;
        packetData.header.flag = static_cast<uint8_t>(flags);
        packetData.header.needACK = needACK;
        packetData.header.payloadType = static_cast<uint8_t>(PayloadTraits<PayloadData>::type);
        packetData.data = payload;
        return packetData;
    }

};
