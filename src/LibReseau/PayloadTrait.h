#pragma once
#include "Payloads.h"

template<typename T>
struct PayloadTraits;

template<>
struct PayloadTraits<PayloadConnect>
{
    static constexpr PayloadType type = PayloadType::CONNECT;
};

template<>
struct PayloadTraits<PayloadACK>
{
    static constexpr PayloadType type = PayloadType::ACK;
};

template<>
struct PayloadTraits<PayloadPing>
{
    static constexpr PayloadType type = PayloadType::PING;
};

template<>
struct PayloadTraits<PayloadName>
{
    static constexpr PayloadType type = PayloadType::NAME;
};

template<>
struct PayloadTraits<PayloadComfirmName>
{
    static constexpr PayloadType type = PayloadType::COMFIRM_NAME;
};

template<>
struct PayloadTraits<PayloadEntityInfo>
{
    static constexpr PayloadType type = PayloadType::ENTITY_INFO;
};

template<>
struct PayloadTraits<PayloadGiveEntityNetworkId>
{
    static constexpr PayloadType type = PayloadType::GIVE_ENTITY_NETWORK_ID;
};

template<>
struct PayloadTraits<PayloadCollide>
{
    static constexpr PayloadType type = PayloadType::COLLIDE;
};

template<>
struct PayloadTraits<PayloadChat>
{
    static constexpr PayloadType type = PayloadType::CHAT;
};

template<>
struct PayloadTraits<PayloadPlayerDeath>
{
    static constexpr PayloadType type = PayloadType::PLAYER_DEATH;
};

template<>
struct PayloadTraits<PayloadGameTimer>
{
    static constexpr PayloadType type = PayloadType::GAME_TIMER;
};
