#include <string>
#include "../include/json.hpp"

#ifndef PROJECT_AIP_DB_USER_H
#define PROJECT_AIP_DB_USER_H

using namespace std;

/**
 * @class User
 * @brief Класс для представления пользователя.
 *
 * Этот класс содержит информацию о пользователе, включая идентификатор, имя пользователя
 * и пароль.
 */
class User {
 private:
  int id;                ///< Идентификатор пользователя.
  std::string username;  ///< Имя пользователя.
  std::string password;  ///< Пароль пользователя.

 public:
  /**
   * @brief Конструктор для создания нового пользователя без идентификатора.
   *
   * @param username Имя пользователя.
   * @param password Пароль пользователя.
   */
  User(const std::string &username, const std::string &password);

  /**
   * @brief Конструктор для создания нового пользователя с идентификатором.
   *
   * @param id Идентификатор пользователя.
   * @param username Имя пользователя.
   * @param password Пароль пользователя.
   */
  User(int id, const std::string &username, const std::string &password);

  /**
   * @brief Получает идентификатор пользователя.
   *
   * @return int Идентификатор пользователя.
   */
  int getId() const;

  /**
   * @brief Получает имя пользователя.
   *
   * @return const std::string& Ссылка на имя пользователя.
   */
  const std::string &getUsername() const;

  /**
   * @brief Получает пароль пользователя.
   *
   * @return const std::string& Ссылка на пароль пользователя.
   */
  const std::string &getPassword() const;

  /**
   * @brief Устанавливает новое имя пользователя.
   *
   * @param newUsername Новое имя пользователя.
   */
  void setUsername(const std::string &newUsername);

  /**
   * @brief Устанавливает новый пароль пользователя.
   *
   * @param newPassword Новый пароль пользователя.
   */
  void setPassword(const std::string &newPassword);

  /**
   * @brief Получает строковое представление пользователя.
   *
   * @return std::string Строковое представление пользователя.
   */
  std::string toString();

  /**
   * @brief Получает JSON-представление пользователя.
   *
   * @return nlohmann::json JSON-представление пользователя.
   */

  nlohmann::json toJson() const {
    return {
        {"id", id},
        {"username", username},
        {"password", password}
    };
  }

};

#endif
