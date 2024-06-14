#include <string>
#include "../json.hpp"

#ifndef PROJECT_AIP_DB_USER_H
#define PROJECT_AIP_DB_USER_H

using namespace std;

/**
 * @brief Класс для представления пользователя.
 */
class User {
 private:
  int id;                 ///< Идентификатор пользователя.
  std::string username;   ///< Имя пользователя.
  std::string password;   ///< Пароль пользователя.

 public:
  /**
   * @brief Конструктор для создания нового пользователя.
   * @param username Имя пользователя.
   * @param password Пароль пользователя.
   */
  User(const std::string &username, const std::string &password);

  /**
   * @brief Конструктор для создания пользователя с идентификатором.
   * @param id Идентификатор пользователя.
   * @param username Имя пользователя.
   * @param password Пароль пользователя.
   */
  User(int id, const std::string &username, const std::string &password);

  /**
   * @brief Конструктор по умолчанию.
   */
  User();

  /**
   * @brief Возвращает идентификатор пользователя.
   * @return Идентификатор пользователя.
   */
  int getId() const;

  /**
   * @brief Возвращает имя пользователя.
   * @return Имя пользователя.
   */
  const std::string &getUsername() const;

  /**
   * @brief Возвращает пароль пользователя.
   * @return Пароль пользователя.
   */
  const std::string &getPassword() const;

  /**
   * @brief Устанавливает новое имя пользователя.
   * @param newUsername Новое имя пользователя.
   */
  void setUsername(const std::string &newUsername);

  /**
   * @brief Устанавливает новый пароль пользователя.
   * @param newPassword Новый пароль пользователя.
   */
  void setPassword(const std::string &newPassword);

  /**
   * @brief Устанавливает идентификатор пользователя.
   * @param id Идентификатор пользователя.
   */
  void setId(int id);

  /**
   * @brief Возвращает строковое представление пользователя.
   * @return Строковое представление пользователя.
   */
  std::string toString();

  /**
   * @brief Возвращает JSON-представление пользователя.
   * @return JSON-представление пользователя.
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
