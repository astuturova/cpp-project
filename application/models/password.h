#include <string>
#include "../json.hpp"

#ifndef PROJECT_AIP_DB_PASSWORD_H
#define PROJECT_AIP_DB_PASSWORD_H

/**
 * @brief Класс для представления пароля.
 *
 * Класс Password содержит информацию о пароле, включая идентификатор, название, сам пароль,
 * заметки и идентификатор пользователя.
 */
class Password {
 private:
  int id;                     ///< Идентификатор пароля.
  std::string name;           ///< Название пароля.
  std::string password;       ///< Пароль.
  std::string notes;          ///< Заметки.
  int userId;                 ///< Идентификатор пользователя.

 public:
  /**
   * @brief Конструктор для создания нового пароля.
   * @param username Имя пользователя.
   * @param password Пароль.
   * @param notes Заметки.
   * @param userId Идентификатор пользователя.
   */
  Password(const std::string &username, const std::string &password, const std::string &notes, int userId);

  /**
   * @brief Конструктор для создания пароля с идентификатором.
   * @param id Идентификатор пароля.
   * @param username Имя пользователя.
   * @param password Пароль.
   * @param notes Заметки.
   * @param userId Идентификатор пользователя.
   */
  Password(int id, const std::string &username, const std::string &password, const std::string &notes, int userId);

  /**
   * @brief Конструктор по умолчанию.
   */
  Password();

  /**
   * @brief Возвращает название пароля.
   * @return Название пароля.
   */
  const std::string &getName() const;

  /**
   * @brief Возвращает пароль.
   * @return Пароль.
   */
  const std::string &getPassword() const;

  /**
   * @brief Возвращает заметки.
   * @return Заметки.
   */
  const std::string &getNotes() const;

  /**
   * @brief Возвращает идентификатор пароля.
   * @return Идентификатор пароля.
   */
  int getId();

  /**
   * @brief Возвращает идентификатор пользователя.
   * @return Идентификатор пользователя.
   */
  int getUserId();

  /**
   * @brief Устанавливает идентификатор пароля.
   * @param id Идентификатор пароля.
   */
  void setId(int id);

  /**
   * @brief Устанавливает пароль.
   * @param newPassword Новый пароль.
   */
  void setPassword(const std::string &newPassword);

  /**
   * @brief Устанавливает название пароля.
   * @param newName Новое название пароля.
   */
  void setName(const std::string &newName);

  /**
   * @brief Устанавливает идентификатор пользователя.
   * @param newUserId Новый идентификатор пользователя.
   */
  void setUserId(int newUserId);

  /**
   * @brief Возвращает строковое представление пароля.
   * @return Строковое представление пароля.
   */
  std::string toString();

  /**
   * @brief Возвращает JSON-представление пароля.
   * @return JSON-представление пароля.
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

#endif //PROJECT_AIP_DB_PASSWORD_H
