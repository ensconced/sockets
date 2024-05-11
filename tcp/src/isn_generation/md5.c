#include "./md5.h"
#include <arpa/inet.h>
#include <openssl/evp.h>
#include <stdint.h>

md5_result md5(EVP_MD *md5_algorithm, uint32_t local_ipv4_addr,
               uint16_t local_port, uint32_t remote_ipv4_addr,
               uint16_t remote_port, uint8_t *secret_key,
               uint32_t secret_key_len) {
  uint32_t big_endian_local_ipv4_addr = htonl(local_ipv4_addr);
  uint16_t big_endian_local_port = htons(local_port);
  uint32_t big_endian_remote_ipv4_addr = htonl(remote_ipv4_addr);
  uint16_t big_endian_remote_port = htons(remote_port);

  EVP_MD_CTX *hash_ctx = EVP_MD_CTX_new();
  if (hash_ctx == NULL) {
    fprintf(stderr, "Failed to create hash ctx\n");
    exit(1);
  }

  if (!EVP_DigestInit(hash_ctx, md5_algorithm)) {
    fprintf(stderr, "Failed to init MD5 digest\n");
    exit(1);
  };

  if (!EVP_DigestUpdate(hash_ctx, &big_endian_local_ipv4_addr,
                        sizeof(big_endian_local_ipv4_addr))) {
    fprintf(stderr, "Failed to digest local IPv4 address\n");
    exit(1);
  };
  if (!EVP_DigestUpdate(hash_ctx, &big_endian_local_port,
                        sizeof(big_endian_local_port))) {
    fprintf(stderr, "Failed to digest local port\n");
    exit(1);
  };
  if (!EVP_DigestUpdate(hash_ctx, &big_endian_remote_ipv4_addr,
                        sizeof(big_endian_remote_ipv4_addr))) {
    fprintf(stderr, "Failed to digest remote IPv4 address\n");
    exit(1);
  };
  if (!EVP_DigestUpdate(hash_ctx, &big_endian_remote_port,
                        sizeof(big_endian_remote_port))) {
    fprintf(stderr, "Failed to digest remote port\n");
    exit(1);
  };
  if (!EVP_DigestUpdate(hash_ctx, secret_key,
                        secret_key_len * sizeof(unsigned char))) {
    fprintf(stderr, "Failed to digest secret key\n");
    exit(1);
  };

  md5_result result = {0};
  if (!EVP_DigestFinal(hash_ctx, (unsigned char *)&result.hash, NULL)) {
    fprintf(stderr, "Failed to finalise digest\n");
    exit(1);
  };

  EVP_MD_CTX_free(hash_ctx);

  return result;
}
