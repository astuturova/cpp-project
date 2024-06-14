#include "password.h"
#include <sstream>

Password::Password(const std::string &name, const std::string &password, const std::string &notes, int userId)
    : name(name), password(password), notes(notes), userId(userId) {}

Password::Password(int id, const std::string &name, const std::string &password, const std::string &notes, int userId)
    : id(id), name(name), password(password), notes(notes), userId(userId) {}

int Password::getId() {
  return id;
}

const std::string &Password::getName() {
  return name;
}

const std::string &Password::getPassword() {
  return password;
}

const std::string &Password::getNotes() {
  return notes;
}

int Password::getUserId() {
  return userId;
}

void Password::setName(const std::string &newName) {
  name = newName;
}

void Password::setPassword(const std::string &newPassword) {
  password = newPassword;
}

void Password::setUserId(int newUserId) {
  userId = newUserId;
}

std::string Password::toString() {
  std::stringstream result;
  result << "('" << name << "', '" << password << "', '" << notes << "', " << userId << ")";
  return result.str();
}