#include <string>
#include "../models/user.h"
#include "../models/password.h"
#include <vector>
#ifndef PROJECT_AIP_DB_DATABASE_H
#define PROJECT_AIP_DB_DATABASE_H

using namespace std;

/**
 * @brief Пространство имен, содержащее функции для работы с базой данных.
 *
 * Это пространство имен включает функции для создания таблиц, вставки, выборки, обновления
 * и удаления данных пользователей и паролей в базе данных.
 */
namespace database {
/**
 * @brief Создает таблицы в базе данных.
 *
 * Эта функция создает необходимые таблицы в базе данных для хранения данных пользователей
 * и паролей.
 */
void createTables();

/**
 * @brief Вставляет нового пользователя в базу данных.
 *
 * @param user Ссылка на объект User, содержащий данные нового пользователя.
 * @return int Идентификатор вставленного пользователя в базе данных.
 */
int insertUser(User &user);

/**
 * @brief Извлекает пользователя из базы данных по имени пользователя.
 *
 * @param username Имя пользователя для поиска.
 * @return User Объект User, содержащий данные найденного пользователя.
 */
User selectUserByUsername(string username);

/**
 * @brief Обновляет пароль в базе данных.
 *
 * @param password Ссылка на объект Password, содержащий обновленные данные пароля.
 */
void updatePassword(Password &password);

/**
 * @brief Удаляет пароль из базы данных по его идентификатору.
 *
 * @param id Идентификатор пароля, который нужно удалить.
 */
void deletePasswordById(int id);

/**
 * @brief Извлекает пароль из базы данных по его идентификатору.
 *
 * @param id Идентификатор пароля для извлечения.
 * @return Password Объект Password, содержащий данные найденного пароля.
 */
Password selectPasswordById(int id);

/**
 * @brief Извлекает пароли из базы данных по имени.
 *
 * @param name Имя для поиска паролей.
 * @return std::vector<Password> Вектор объектов Password, содержащий найденные пароли.
 */
std::vector<Password> selectPasswordByName(string name);

/**
 * @brief Извлекает пароли из базы данных по идентификатору пользователя.
 *
 * @param id Идентификатор пользователя для поиска паролей.
 * @return std::vector<Password> Вектор объектов Password, содержащий найденные пароли.
 */
std::vector<Password> selectPasswordByUserId(int id);
};

#endif
