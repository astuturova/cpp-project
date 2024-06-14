#define DOCTEST_CONFIG_IMPLEMENT
#include "../include/doctest.h"
#include "../api/encryptor.h"

TEST_CASE("Encrypt function 1") {
  std::string plaintext = "PasswordExample!";
  std::string expected_ciphertext = "HeuJAHbhcpeoGpx!";

  std::string ciphertext = encrypt(plaintext);

  REQUIRE(ciphertext == expected_ciphertext);
}

TEST_CASE("Encrypt function 2") {
  std::string plaintext = "pa112s33w66or6d#$!@##!@#!@#";
  std::string expected_ciphertext = "he112L33U66qI6w#$!@##!@#!@#";

  std::string ciphertext = encrypt(plaintext);

  REQUIRE(ciphertext == expected_ciphertext);
}

TEST_CASE("Decrypt function 1") {
  std::string ciphertext = "HeuJAHbhcpeoGpx!";
  std::string expected_plaintext = "PasswordExample!";

  std::string plaintext = decrypt(ciphertext);

  REQUIRE(plaintext == expected_plaintext);
}

TEST_CASE("Decrypt function 2") {
  std::string ciphertext = "he112L33U66qI6w#$!@##!@#!@#";
  std::string expected_plaintext = "pa112s33w66or6d#$!@##!@#!@#";

  std::string plaintext = decrypt(ciphertext);

  REQUIRE(plaintext == expected_plaintext);
}