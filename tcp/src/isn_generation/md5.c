#include "./md5.h"
#include "./secret_key.h"
#include <openssl/evp.h>
#include <stdint.h>

md5_result md5(EVP_MD *md5_algorithm, uint32_t local_ipv4_addr,
               uint16_t local_port, uint32_t remote_ipv4_addr,
               uint16_t remote_port) {
  EVP_MD_CTX *hash_ctx = EVP_MD_CTX_new();
  if (hash_ctx == NULL) {
    fprintf(stderr, "Failed to create hash ctx\n");
    exit(1);
  }

  if (!EVP_DigestInit(hash_ctx, md5_algorithm)) {
    fprintf(stderr, "Failed to init MD5 digest\n");
    exit(1);
  };

  if (!EVP_DigestUpdate(hash_ctx, &local_ipv4_addr, sizeof(local_ipv4_addr))) {
    fprintf(stderr, "Failed to digest local IPv4 address\n");
    exit(1);
  };
  if (!EVP_DigestUpdate(hash_ctx, &local_port, sizeof(local_port))) {
    fprintf(stderr, "Failed to digest local port\n");
    exit(1);
  };
  if (!EVP_DigestUpdate(hash_ctx, &remote_ipv4_addr,
                        sizeof(remote_ipv4_addr))) {
    fprintf(stderr, "Failed to digest remote IPv4 address\n");
    exit(1);
  };
  if (!EVP_DigestUpdate(hash_ctx, &remote_port, sizeof(remote_port))) {
    fprintf(stderr, "Failed to digest remote port\n");
    exit(1);
  };
  if (!EVP_DigestUpdate(hash_ctx, secret_key,
                        secret_key_len * sizeof(unsigned char))) {
    fprintf(stderr, "Failed to digest secret key\n");
    exit(1);
  };

  md5_result result = {0};
  if (!EVP_DigestFinal(hash_ctx, &result.hash, NULL)) {
    fprintf(stderr, "Failed to finalise digest\n");
    exit(1);
  };

  return result;
}
