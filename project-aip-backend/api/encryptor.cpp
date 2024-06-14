#include "encryptor.h"
#include <string>

std::string encrypt(const std::string &plaintext) {

  const std::string key = "SecretKey";
  std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  std::string ciphertext;
  int keyLength = key.length();

  for (int i = 0; i < plaintext.length(); i++) {
    char currentChar = plaintext[i];
    int charIndex = alphabet.find(currentChar);

    if (charIndex != std::string::npos) {
      char keyChar = key[i % keyLength];
      int keyIndex = alphabet.find(keyChar);
      int encryptedIndex = (charIndex + keyIndex) % alphabet.length();
      ciphertext += alphabet[encryptedIndex];
    } else {
      ciphertext += currentChar;
    }
  }

  return ciphertext;
}

std::string decrypt(const std::string &ciphertext) {

  const std::string key = "SecretKey";
  std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  std::string plaintext;
  int keyLength = key.length();

  for (int i = 0; i < ciphertext.length(); i++) {
    char currentChar = ciphertext[i];
    int charIndex = alphabet.find(currentChar);

    if (charIndex != std::string::npos) {
      char keyChar = key[i % keyLength];
      int keyIndex = alphabet.find(keyChar);
      int decryptedIndex = (charIndex - keyIndex + alphabet.length()) % alphabet.length();
      plaintext += alphabet[decryptedIndex];
    } else {
      plaintext += currentChar;
    }
  }

  return plaintext;
}
