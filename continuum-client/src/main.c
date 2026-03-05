/*
 * Continuum 0.40 Encryption Client
 * Example usage and demonstration
 *
 * This demonstrates how to use the Continuum encryption library.
 *
 * IMPORTANT: This implementation includes everything EXCEPT steps 1-2 of
 * key expansion, which must be obtained from:
 * 1. An oracle service that has access to Continuum.exe
 * 2. The scrty1 file from a SubSpace server
 * 3. Reverse engineering the algorithm from the Continuum binary
 *
 * The actual key expansion steps 1-2 are proprietary and intentionally
 * not publicly documented to maintain game integrity.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "types.h"
#include "checksum.h"
#include "vie_crypt.h"
#include "continuum_crypt.h"
#include "protocol.h"

/* Print a hex dump of data */
static void hex_dump(const char* label, const u8* data, size_t len) {
    printf("%s (%zu bytes): ", label, len);
    for (size_t i = 0; i < len && i < 32; i++) {
        printf("%02x ", data[i]);
    }
    if (len > 32) printf("...");
    printf("\n");
}

/* Demonstrate VIE encryption (SubSpace legacy) */
static void demo_vie_encryption(void) {
    printf("\n=== VIE Encryption Demo ===\n");

    VieEncrypt vie;
    vie_init(&vie);

    /* Generate client key */
    vie.client_key = vie_generate_key();
    printf("Client key: 0x%08x\n", vie.client_key);

    /* Simulate server response (negated key enables encryption) */
    u32 server_key = (~vie.client_key) + 1;
    printf("Server key: 0x%08x\n", server_key);

    /* Initialize with server key */
    if (vie_initialize(&vie, server_key)) {
        printf("VIE encryption initialized successfully\n");
    } else {
        printf("VIE encryption initialization failed!\n");
        return;
    }

    /* Test encryption/decryption */
    u8 original[] = {0x03, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0x11, 0x22, 0x33};
    u8 encrypted[sizeof(original)];
    u8 decrypted[sizeof(original)];

    hex_dump("Original", original, sizeof(original));

    /* Encrypt */
    vie_encrypt(&vie, original, encrypted, sizeof(original));
    hex_dump("Encrypted", encrypted, sizeof(encrypted));

    /* Decrypt (in-place) */
    memcpy(decrypted, encrypted, sizeof(encrypted));
    vie_decrypt(&vie, decrypted, sizeof(decrypted));
    hex_dump("Decrypted", decrypted, sizeof(decrypted));

    /* Verify */
    if (memcmp(original, decrypted, sizeof(original)) == 0) {
        printf("VIE encryption test PASSED\n");
    } else {
        printf("VIE encryption test FAILED\n");
    }
}

/* Demonstrate Continuum encryption with mock key expansion */
static void demo_continuum_encryption(void) {
    printf("\n=== Continuum Encryption Demo ===\n");

    ContinuumEncrypt cont;
    continuum_init(&cont);

    /*
     * IMPORTANT: In a real implementation, you would obtain the partial
     * expanded key (steps 1-2 done) from an oracle service.
     *
     * The oracle would:
     * 1. Receive key2 from the client
     * 2. Execute Continuum.exe code at RVA 0x057D60 (step 1)
     * 3. Execute Continuum.exe code at RVA 0x058BE0 (step 2)
     * 4. Return the 80-byte partially expanded key
     *
     * Here we use mock data for demonstration purposes.
     */
    printf("NOTE: Using mock expanded key (real implementation needs oracle)\n");

    /* Mock partial expanded key (would come from oracle in real use) */
    u32 mock_partial_key[20] = {
        0x12345678, 0x9abcdef0, 0x11223344, 0x55667788,
        0x99aabbcc, 0xddeeff00, 0x12121212, 0x34343434,
        0x56565656, 0x78787878, 0x9a9a9a9a, 0xbcbcbcbc,
        0xdededede, 0xf0f0f0f0, 0xabababab, 0xcdcdcdcd,
        0xefefefef, 0x01010101, 0x23232323, 0x45454545
    };

    cont.key2 = 0xdeadbeef;  /* Server's key2 */
    continuum_set_expanded_key(&cont, mock_partial_key);

    /* Finalize expansion with our key1 (step 3 - modified MD5) */
    u32 key1 = 0xcafebabe;
    continuum_finalize_expansion(&cont, key1);
    printf("Key expansion finalized (step 3 done)\n");

    /* Mark as initialized (normally done after receiving 0x00 0x02) */
    cont.state = CONT_STATE_INITIALIZED;

    /* Test encryption/decryption */
    u8 original[] = {0x06, 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
    u8 encrypted[sizeof(original) + 2];  /* +2 for CRC and possible escape */
    u8 decrypted[sizeof(original)];

    hex_dump("Original", original, sizeof(original));

    /* Encrypt (adds CRC8 prefix) */
    size_t enc_len = continuum_encrypt(&cont, original, encrypted, sizeof(original));
    hex_dump("Encrypted", encrypted, enc_len);

    /* Decrypt (removes CRC8, verifies it) */
    memcpy(decrypted, encrypted, enc_len);
    size_t dec_len = continuum_decrypt(&cont, decrypted, enc_len);

    if (dec_len > 0) {
        hex_dump("Decrypted", decrypted, dec_len);

        if (dec_len == sizeof(original) && memcmp(original, decrypted, sizeof(original)) == 0) {
            printf("Continuum encryption test PASSED\n");
        } else {
            printf("Continuum encryption test FAILED (data mismatch)\n");
        }
    } else {
        printf("Continuum decryption failed (CRC check failed)\n");
    }
}

/* Demonstrate checksum functions */
static void demo_checksums(void) {
    printf("\n=== Checksum Demo ===\n");

    u8 data[] = "Hello, SubSpace!";
    size_t len = sizeof(data) - 1;

    /* CRC8 */
    u8 c8 = crc8(data, len);
    printf("CRC8:  0x%02x\n", c8);

    /* CRC32 */
    u32 c32 = crc32(data, len);
    printf("CRC32: 0x%08x\n", c32);

    /* VIE checksum */
    u32 key = 0x12345678;
    u32 vie = vie_checksum(key);
    printf("VIE checksum (key=0x%08x): 0x%08x\n", key, vie);

    /* Weapon checksum */
    u8 weapon_data[] = {0x05, 0x12, 0x34, 0x56, 0x78};
    u8 wc = weapon_checksum(weapon_data, sizeof(weapon_data));
    printf("Weapon checksum: 0x%02x\n", wc);
}

/* Demonstrate protocol structures */
static void demo_protocol(void) {
    printf("\n=== Protocol Structures Demo ===\n");

    /* Connection init packet */
    ConnectionInitPacket init;
    init.type1 = CORE_PACKET_MARKER;
    init.type2 = CORE_CONNECTION_INIT;
    init.client_key = 0;  /* Continuum doesn't use VIE key here */
    init.version = 0x0011;  /* Continuum version marker */

    printf("Connection init packet size: %zu bytes\n", sizeof(init));
    hex_dump("Packet", (u8*)&init, sizeof(init));

    /* Key exchange request */
    KeyExchangeRequestPacket key_req;
    key_req.type1 = CORE_PACKET_MARKER;
    key_req.type2 = CORE_KEY_REQUEST;
    key_req.key1 = 0xcafebabe;
    key_req.key2 = 0xdeadbeef;

    printf("Key exchange request size: %zu bytes\n", sizeof(key_req));
    hex_dump("Packet", (u8*)&key_req, sizeof(key_req));

    /* Login packet */
    LoginPacket login;
    memset(&login, 0, sizeof(login));
    login.type = GAME_LOGIN;
    login.new_user = 0;
    strncpy(login.name, "TestPlayer", sizeof(login.name) - 1);
    strncpy(login.password, "secret", sizeof(login.password) - 1);
    login.client_version = 40;  /* Continuum 0.40 */

    printf("Login packet size: %zu bytes\n", sizeof(login));
}

int main(int argc, char* argv[]) {
    printf("Continuum 0.40 Encryption Client\n");
    printf("================================\n");
    printf("This library implements Continuum's encryption scheme.\n");
    printf("\n");
    printf("IMPORTANT: Key expansion steps 1-2 are NOT included.\n");
    printf("You must obtain the partial expanded key from:\n");
    printf("  - An oracle service with Continuum.exe access\n");
    printf("  - The scrty1 file from a SubSpace server\n");
    printf("\n");

    (void)argc;
    (void)argv;

    srand((unsigned int)time(NULL));

    demo_vie_encryption();
    demo_continuum_encryption();
    demo_checksums();
    demo_protocol();

    printf("\n=== Done ===\n");
    return 0;
}
