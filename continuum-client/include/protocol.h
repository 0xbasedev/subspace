/*
 * Continuum 0.40 Encryption Client
 * Protocol definitions
 *
 * SubSpace/Continuum protocol packet types and structures.
 */

#ifndef CONTINUUM_PROTOCOL_H
#define CONTINUUM_PROTOCOL_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Core Protocol Packets (0x00 0xXX)
 * These are always sent unencrypted.
 */
#define CORE_PACKET_MARKER      0x00

/* Connection packets */
#define CORE_CONNECTION_INIT    0x01  /* Client -> Server: Connection request */
#define CORE_CONNECTION_RESP    0x02  /* Server -> Client: Connection response */
#define CORE_RELIABLE           0x03  /* Reliable packet wrapper */
#define CORE_RELIABLE_ACK       0x04  /* Reliable packet acknowledgment */
#define CORE_SYNC_REQUEST       0x05  /* Time sync request */
#define CORE_SYNC_RESPONSE      0x06  /* Time sync response */
#define CORE_DISCONNECT         0x07  /* Disconnect notification */
#define CORE_CHUNK              0x08  /* Large packet chunk */
#define CORE_CHUNK_TAIL         0x09  /* Large packet chunk (final) */
#define CORE_STREAM             0x0A  /* File stream */
#define CORE_STREAM_CANCEL      0x0B  /* Cancel file stream */
#define CORE_CLUSTER            0x0E  /* Packet cluster (multiple packets) */

/* Encryption key exchange packets */
#define CORE_KEY_REQUEST        0x10  /* Client -> Server: Key exchange request */
#define CORE_KEY_RESPONSE       0x11  /* Client -> Server: Key confirmation */

/*
 * Connection Init Packet (0x00 0x01)
 * Sent by client to initiate connection.
 *
 * VIE format:  00 01 <client_key:4> 01 00
 * Continuum:   00 01 <legacy:4> 11 00
 */
#pragma pack(push, 1)
typedef struct {
    u8  type1;          /* 0x00 */
    u8  type2;          /* 0x01 */
    u32 client_key;     /* Client-generated key (VIE) or legacy field */
    u16 version;        /* 0x0001 (VIE) or 0x0011 (Continuum) */
} ConnectionInitPacket;

/*
 * Connection Response Packet (0x00 0x02)
 * Sent by server in response to connection init.
 */
typedef struct {
    u8  type1;          /* 0x00 */
    u8  type2;          /* 0x02 */
    u32 server_key;     /* Server's key response */
} ConnectionResponsePacket;

/*
 * Key Exchange Request (0x00 0x10)
 * Sent by Continuum client to request encryption keys.
 *
 * Format: 00 10 <key1:4> <key2:4>
 */
typedef struct {
    u8  type1;          /* 0x00 */
    u8  type2;          /* 0x10 */
    u32 key1;           /* Client's key (used in step 3) */
    u32 key2;           /* Server should generate partial expansion for this */
} KeyExchangeRequestPacket;

/*
 * Key Exchange Response (0x00 0x10 from server)
 * Server responds with the same packet type.
 */
typedef struct {
    u8  type1;          /* 0x00 */
    u8  type2;          /* 0x10 */
    u32 key1;           /* Echo of client's key1 */
    u32 key2;           /* Server's key2 */
    u16 trailer;        /* 0x0001 */
} KeyExchangeResponsePacket;

/*
 * Key Confirmation (0x00 0x11)
 * Client confirms key exchange, encryption now active.
 */
typedef struct {
    u8  type1;          /* 0x00 */
    u8  type2;          /* 0x11 */
    u32 key1;           /* Echo key1 */
    u16 trailer;        /* 0x0001 */
} KeyConfirmationPacket;

/*
 * Reliable Packet Wrapper (0x00 0x03)
 * Wraps a packet for reliable delivery.
 */
typedef struct {
    u8  type1;          /* 0x00 */
    u8  type2;          /* 0x03 */
    u32 sequence;       /* Sequence number */
    /* Followed by actual packet data */
} ReliablePacketHeader;

/*
 * Reliable ACK (0x00 0x04)
 */
typedef struct {
    u8  type1;          /* 0x00 */
    u8  type2;          /* 0x04 */
    u32 sequence;       /* Sequence number being acknowledged */
} ReliableAckPacket;

/*
 * Time Sync Request (0x00 0x05)
 */
typedef struct {
    u8  type1;          /* 0x00 */
    u8  type2;          /* 0x05 */
    u32 timestamp;      /* Client's timestamp */
    u32 total_sent;     /* Total packets sent */
    u32 total_recv;     /* Total packets received */
} TimeSyncRequestPacket;

/*
 * Time Sync Response (0x00 0x06)
 */
typedef struct {
    u8  type1;          /* 0x00 */
    u8  type2;          /* 0x06 */
    u32 client_time;    /* Echo of client's timestamp */
    u32 server_time;    /* Server's timestamp */
} TimeSyncResponsePacket;

/*
 * Disconnect (0x00 0x07)
 */
typedef struct {
    u8  type1;          /* 0x00 */
    u8  type2;          /* 0x07 */
} DisconnectPacket;

#pragma pack(pop)

/*
 * Game Protocol Packets
 * These are sent encrypted after key exchange.
 */

/* Client -> Server */
#define GAME_POSITION           0x03  /* Position update */
#define GAME_CHAT               0x06  /* Chat message */
#define GAME_LOGIN              0x09  /* Login request */
#define GAME_ARENA_LOGIN        0x0A  /* Arena login */
#define GAME_LEAVE_ARENA        0x0C  /* Leave arena */
#define GAME_SECURITY_RESPONSE  0x1A  /* Security checksum response */
#define GAME_SETTINGS_REQUEST   0x18  /* Request arena settings */

/* Server -> Client */
#define GAME_PLAYER_ID          0x01  /* Assigned player ID */
#define GAME_PLAYER_ENTER       0x03  /* Player entered arena */
#define GAME_PLAYER_LEAVE       0x04  /* Player left arena */
#define GAME_POSITION_UPDATE    0x05  /* Large position update */
#define GAME_CHAT_MESSAGE       0x07  /* Chat message */
#define GAME_ARENA_SETTINGS     0x0F  /* Arena settings */
#define GAME_SECURITY_REQUEST   0x18  /* Security checksum request */
#define GAME_MAP_INFO           0x29  /* Map information */

/*
 * Login Packet (0x09)
 */
#pragma pack(push, 1)
typedef struct {
    u8  type;           /* 0x09 */
    u8  new_user;       /* 0 = existing, 1 = new user */
    char name[20];      /* Player name (null-padded) */
    char password[20];  /* Password (null-padded) */
    u32 machine_id;     /* Machine identifier */
    u8  connection_type;/* Connection type */
    u16 timezone;       /* Timezone offset */
    u16 unused1;
    u16 client_version; /* Client version */
    u32 reg_form_req;   /* Registration form request */
    u32 unused2;
    u32 unused3;
    u16 unused4;
} LoginPacket;

/*
 * Security Request Packet (0x18 from server)
 */
typedef struct {
    u8  type;           /* 0x18 */
    u32 green_seed;     /* Seed for green checksum */
    u32 door_seed;      /* Seed for door checksum */
    u32 timestamp;      /* Server timestamp */
    u32 checksum_key;   /* Key for executable checksum */
} SecurityRequestPacket;

/*
 * Security Response Packet (0x1A from client)
 */
typedef struct {
    u8  type;           /* 0x1A */
    u32 weapon_count;   /* Number of weapons fired */
    u32 settings_checksum;  /* Arena settings checksum */
    u32 exe_checksum;   /* Executable checksum */
    u32 map_checksum;   /* Map checksum */
    u32 s2c_slow_total; /* S2C slow total */
    u32 s2c_fast_total; /* S2C fast total */
    u16 s2c_slow_current;
    u16 s2c_fast_current;
    u16 s2c_rel_out;
    u16 ping;
    u16 ping_avg;
    u16 ping_low;
    u16 ping_high;
    u16 unused;
} SecurityResponsePacket;
#pragma pack(pop)

/*
 * Encryption type identifiers
 */
typedef enum {
    ENCRYPT_NONE,       /* No encryption */
    ENCRYPT_VIE,        /* Original SubSpace VIE encryption */
    ENCRYPT_CONTINUUM   /* Continuum 0.40 encryption */
} EncryptionType;

#ifdef __cplusplus
}
#endif

#endif /* CONTINUUM_PROTOCOL_H */
