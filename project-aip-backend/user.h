#include <string>
#include "json.hpp"

#ifndef PROJECT_AIP_DB_USER_H
#define PROJECT_AIP_DB_USER_H

using namespace std;

class User {
private:
    int id;
    string username;
    string password;

public:
    User(const string &username, const string &password);

    User(int id, const string &username, const string &password);

    int getId() const;

    const string &getUsername() const;

    const string &getPassword() const;

    void setUsername(const string &newUsername);

    void setPassword(const string &newPassword);

    string toString();

    nlohmann::json toJson() const {
        return {
                {"id", id},
                {"username", username},
                {"password", password}
        };
    }

};


#endif
