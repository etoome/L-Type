#include "server/utils.hpp"

#include <gcrypt.h>

#include <cstdlib>
#include <ctime>
#include <fstream>

#include "utils.hpp"

const std::string HMAC_KEY_PATH = "./keys/HMAC_SHA256.key";
const std::string ALPHANUM_CHARS = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

std::string genRandomStr(unsigned int length) noexcept {
  std::string str;

  for (size_t i = 0; i < length; ++i) {
    str += ALPHANUM_CHARS[unsigned(genRandomInt(0, int(ALPHANUM_CHARS.length()) - 1))];
  }

  return str;
}

std::string getKey(const std::string& path) {
  std::string ret;
  std::ifstream file(path);
  getline(file, ret);
  return ret;
}

std::string hash(const std::string& str) {
  gcry_md_hd_t h;

  gcry_md_open(&h, GCRY_MD_SHA256, 0);
  gcry_md_write(h, str.c_str(), str.length());
  unsigned char* hash = gcry_md_read(h, GCRY_MD_SHA256);

  size_t len = gcry_md_get_algo_dlen(GCRY_MD_SHA256);

  char c[255];
  for (size_t i = 0; i != len; ++i) {
    sprintf(&c[2 * i], "%02x", hash[i]);
  }

  return c;
}

std::string genSignature(const std::string& str) {
  gcry_mac_hd_t h;
  unsigned char hash[255];

  size_t maclen = gcry_mac_get_algo_maclen(GCRY_MAC_HMAC_SHA256);

  std::string key = getKey(HMAC_KEY_PATH);

  gcry_mac_open(&h, GCRY_MAC_HMAC_SHA256, 0, NULL);
  gcry_mac_setkey(h, key.c_str(), key.length());
  gcry_mac_write(h, str.c_str(), str.length());
  gcry_mac_read(h, hash, &maclen);

  char c[255];
  unsigned i;
  for (i = 0; i != maclen; ++i) {
    sprintf(&c[2 * i], "%02x", hash[i]);
  }

  return c;
}
