#include <string>
#include "user.h"
#include "password.h"
#include <vector>
#ifndef PROJECT_AIP_DB_DATABASE_H
#define PROJECT_AIP_DB_DATABASE_H

using namespace std;

namespace database {
    void createTables();
    int insertUser(User& user);
    User selectUserByUsername(string username);
    void updatePassword(Password& password);
    void deletePasswordById(int id);
    Password selectPasswordById(int id);
    std::vector<Password> selectPasswordByName(string name);
    std::vector<Password> selectPasswordByUserId(int id);
};


#endif
