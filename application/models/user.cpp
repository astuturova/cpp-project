#include "user.h"
#include <string>
#include <sstream>

User::User(const std::string &username, const std::string &password)
    : username(username), password(password) {}

User::User(int id, const std::string &username, const std::string &password)
    : id(id), username(username), password(password) {}

User::User() : id(0), username(""), password("") {}

int User::getId() const {
  return id;
}

const std::string &User::getUsername() const {
  return username;
}

const std::string &User::getPassword() const {
  return password;
}

void User::setUsername(const std::string &newUsername) {
  username = newUsername;
}

void User::setPassword(const std::string &newPassword) {
  password = newPassword;
}

std::string User::toString() {
  stringstream result;
  result << "(null, '" << username << "', '" << password << "')";
  return result.str();
}

void User::setId(int id) {
  User::id = id;
}
