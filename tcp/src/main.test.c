void md5_test(void);
void hash_map_test(void);
void checksum_test(void);
void event_queue_test(void);

int main(void) {
  md5_test();
  hash_map_test();
  checksum_test();
  event_queue_test();
  return 0;
}
