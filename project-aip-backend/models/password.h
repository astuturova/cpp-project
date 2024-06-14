#include <string>
#include "../include/json.hpp"

#ifndef PROJECT_AIP_DB_PASSWORD_H
#define PROJECT_AIP_DB_PASSWORD_H

/**
 * @class Password
 * @brief Класс для представления записи пароля.
 *
 * Этот класс содержит информацию о пароле, включая идентификатор, имя, сам пароль,
 * примечания и идентификатор пользователя, которому принадлежит пароль.
 */
class Password {
 private:
  int id;                    ///< Идентификатор пароля.
  std::string name;          ///< Имя пароля.
  std::string password;      ///< Сам пароль.
  std::string notes;         ///< Примечания к паролю.
  int userId;                ///< Идентификатор пользователя, которому принадлежит пароль.

 public:
  /**
   * @brief Конструктор для создания нового пароля без идентификатора.
   *
   * @param username Имя пароля.
   * @param password Сам пароль.
   * @param notes Примечания к паролю.
   * @param userId Идентификатор пользователя, которому принадлежит пароль.
   */
  Password(const std::string &username, const std::string &password, const std::string &notes, int userId);

  /**
   * @brief Конструктор для создания нового пароля с идентификатором.
   *
   * @param id Идентификатор пароля.
   * @param username Имя пароля.
   * @param password Сам пароль.
   * @param notes Примечания к паролю.
   * @param userId Идентификатор пользователя, которому принадлежит пароль.
   */
  Password(int id, const std::string &username, const std::string &password, const std::string &notes, int userId);

  /**
   * @brief Получает идентификатор пароля.
   *
   * @return int Идентификатор пароля.
   */
  int getId();

  /**
   * @brief Получает имя пароля.
   *
   * @return const std::string& Ссылка на имя пароля.
   */
  const std::string &getName();

  /**
   * @brief Получает сам пароль.
   *
   * @return const std::string& Ссылка на значение пароля.
   */
  const std::string &getPassword();

  /**
   * @brief Получает примечания к паролю.
   *
   * @return const std::string& Ссылка на примечания к паролю.
   */
  const std::string &getNotes();

  /**
   * @brief Получает идентификатор пользователя, которому принадлежит пароль.
   *
   * @return int Идентификатор пользователя.
   */
  int getUserId();

  /**
   * @brief Устанавливает новое значение пароля.
   *
   * @param newPassword Новое значение пароля.
   */
  void setPassword(const std::string &newPassword);

  /**
   * @brief Устанавливает новое имя пароля.
   *
   * @param newName Новое имя пароля.
   */
  void setName(const std::string &newName);

  /**
   * @brief Устанавливает новый идентификатор пользователя, которому принадлежит пароль.
   *
   * @param newUserId Новый идентификатор пользователя.
   */
  void setUserId(int newUserId);

  /**
   * @brief Получает строковое представление пароля.
   *
   * @return std::string Строковое представление пароля.
   */
  std::string toString();

  /**
   * @brief Получает JSON-представление пароля.
   *
   * @return nlohmann::json JSON-представление пароля.
   */
  nlohmann::json toJson() const {
    return {
        {"id", id},
        {"name", name},
        {"password", password},
        {"userId", userId}
    };
  }
};

#endif
