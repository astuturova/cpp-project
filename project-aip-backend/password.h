#include <string>
#include "json.hpp"

#ifndef PROJECT_AIP_DB_PASSWORD_H
#define PROJECT_AIP_DB_PASSWORD_H


class Password {
private:
    int id;
    std::string name;
    std::string password;
    std::string notes;
    int userId;

public:
    Password(const std::string& username, const std::string& password, const std::string& notes, int userId);
    Password(int id, const std::string& username, const std::string& password, const std::string& notes, int userId);
    int getId();
    const std::string& getName();
    const std::string& getPassword();
    const std::string& getNotes();
    int getUserId();

    void setPassword(const std::string &newPassword);

    void setName(const std::string &newName);

    void setUserId(int newUserId);

    std::string toString();

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
