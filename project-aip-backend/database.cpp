#include "database.h"
#include <string>
#include <iostream>
#include <sstream>
#include "sqlite3.h"
#include "user.h"

using namespace std;
namespace database {
    void createTables() {

        sqlite3 *DB;

        string createUsersTable = "CREATE TABLE if not exists user (\n"
                                  "    id INTEGER PRIMARY KEY,\n"
                                  "    username VARCHAR(50) UNIQUE NOT NULL,\n"
                                  "    password VARCHAR(255) NOT NULL\n"
                                  ");\n"
                                  "CREATE TABLE if not exists passwords (\n"
                                  "    id INTEGER PRIMARY KEY,\n"
                                  "    name VARCHAR(255) NOT NULL,\n"
                                  "    password VARCHAR(255) NOT NULL,\n"
                                  "    notes TEXT,\n"
                                  "    user_id INTEGER NOT NULL,\n"
                                  "    FOREIGN KEY (user_id) REFERENCES user(id)\n"
                                  ");";

        int exit = 0;
        exit = sqlite3_open("manager.db", &DB);
        char *messageError;
        exit = sqlite3_exec(DB, createUsersTable.c_str(), nullptr, nullptr, &messageError);

        if (exit != SQLITE_OK) {
            cout << "Error while creating tables!" << endl;
            sqlite3_free(messageError);
        }
        sqlite3_close(DB);

    }

    int insertUser(User &user) {
        const string sqlEntityString = user.toString();

        stringstream sqlString;
        sqlString << "INSERT INTO user (id, username, password) VALUES " << sqlEntityString << ";";
        std::string req = sqlString.str();
        const char *sqlRequest = req.c_str();

        sqlite3 *db;
        char *zErrMsg = 0;
        int rc;

        rc = sqlite3_open("manager.db", &db);
        if (rc) {
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        }

        rc = sqlite3_exec(db, sqlRequest, nullptr, nullptr, &zErrMsg);

        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << zErrMsg << std::endl;
            sqlite3_free(zErrMsg);
            sqlite3_close(db);
            throw std::invalid_argument("Пользователь уже зарегистрирован");
        }
        sqlite3_close(db);
    }

    User selectUserByUsername(string username) {
        sqlite3 *db;
        int rc;
        sqlite3_stmt *stmt;
        rc = sqlite3_open("manager.db", &db);

        stringstream stream;
        stream << "select * from user where username='" << username << "'";

        std::string req = stream.str();
        const char *sqlRequest = req.c_str();


        sqlite3_prepare_v2(db, sqlRequest, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            sqlite3_finalize(stmt);
        }

        rc = sqlite3_step(stmt);
        if (rc == 101){
            sqlite3_close(db);
            sqlite3_finalize(stmt);
            throw std::invalid_argument("Пользователь не найден");
        }

        while (rc == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const char *username = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            const char *password = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
            User user(id, username, password);
            sqlite3_close(db);
            sqlite3_finalize(stmt);
            return user;
        }
    }

    void updatePassword(Password &password) {
        const string sqlEntityString = password.toString();

        stringstream sqlString;
        sqlString << "INSERT INTO passwords (name, password, notes, user_id) VALUES " << sqlEntityString << ";";
        std::string req = sqlString.str();
        const char *sqlRequest = req.c_str();

        sqlite3 *db;
        char *zErrMsg = 0;
        int rc;

        rc = sqlite3_open("manager.db", &db);
        if (rc) {
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        }

        rc = sqlite3_exec(db, sqlRequest, nullptr, nullptr, &zErrMsg);

        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << zErrMsg << std::endl;
            sqlite3_free(zErrMsg);
            sqlite3_close(db);
        }
        sqlite3_close(db);
    }

    void deletePasswordById(int id) {

        stringstream sqlString;
        sqlString << "DELETE FROM passwords WHERE id=" << id << ";";
        string req = sqlString.str();
        const char *sqlRequest = req.c_str();

        sqlite3 *db;
        char *zErrMsg = 0;
        int rc;

        rc = sqlite3_open("manager.db", &db);
        if (rc) {
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        }

        rc = sqlite3_exec(db, sqlRequest, nullptr, nullptr, &zErrMsg);

        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << zErrMsg << std::endl;
            sqlite3_free(zErrMsg);
            sqlite3_close(db);
        }
        sqlite3_close(db);
    }

    Password selectPasswordById(int id) {
        sqlite3 *db;
        int rc;
        sqlite3_stmt *stmt;
        rc = sqlite3_open("manager.db", &db);

        stringstream stream;
        stream << "select * from passwords where id='" << id << "'";

        std::string req = stream.str();
        const char *sqlRequest = req.c_str();


        sqlite3_prepare_v2(db, sqlRequest, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            sqlite3_finalize(stmt);
        }

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const char *name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            const char *password = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
            const char *notes = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
            notes = notes == nullptr ? "" : notes;
            int userId = sqlite3_column_int(stmt, 4);
            Password entity(id, name, password, notes, userId);
            sqlite3_close(db);
            sqlite3_finalize(stmt);
            return entity;
        }

    };

    std::vector<Password> selectPasswordByName(string name) {
        std::vector<Password> passwords;
        sqlite3* db;
        int rc;
        sqlite3_stmt* stmt;
        rc = sqlite3_open("manager.db", &db);

        std::stringstream stream;
        stream << "SELECT * FROM passwords WHERE name='" << name << "'";
        std::string req = stream.str();
        const char* sqlRequest = req.c_str();

        rc = sqlite3_prepare_v2(db, sqlRequest, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            sqlite3_finalize(stmt);
            return passwords;
        }

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const char *name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            const char *password = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
            const char *notes = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
            notes = notes == nullptr ? "" : notes;
            int userId = sqlite3_column_int(stmt, 4);
            Password entity(id, name, password, notes, userId);
            passwords.push_back(entity);
        }

        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return passwords;
    };

    std::vector<Password> selectPasswordByUserId(int id) {
        std::vector<Password> passwords;
        sqlite3* db;
        int rc;
        sqlite3_stmt* stmt;
        rc = sqlite3_open("manager.db", &db);

        std::stringstream stream;
        stream << "SELECT * FROM passwords WHERE user_id=" << id;
        std::string req = stream.str();
        const char* sqlRequest = req.c_str();

        rc = sqlite3_prepare_v2(db, sqlRequest, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            sqlite3_finalize(stmt);
            return passwords;
        }

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const char *name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            const char *password = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
            const char *notes = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
            notes = notes == nullptr ? "" : notes;
            int userId = sqlite3_column_int(stmt, 4);
            Password entity(id, name, password, notes, userId);
            passwords.push_back(entity);
        }

        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return passwords;
    }

}

