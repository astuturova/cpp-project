#include <iostream>
#include <string>
#include <unordered_map>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include "json.hpp"
#include "user.h"
#include "database.h"
#include "password.h"


namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace pt = boost::property_tree;
// Простое хранилище пользователей
std::unordered_map<std::string, std::string> users;

// Функция для регистрации пользователя
bool register_user(const std::string &username, const std::string &password) {
    // Проверяем, существует ли уже пользователь с таким именем
    if (users.find(username) != users.end()) {
        return false;
    }

    // Добавляем нового пользователя
    users[username] = password;
    return true;
}

// Функция для аутентификации пользователя
bool authenticate_user(User &user) {
    try {
        User userEntity = database::selectUserByUsername(user.getUsername());
        return userEntity.getPassword() == user.getPassword();
    } catch (const std::exception &e) {
        return false;
    }
}

// Функция для извлечения пути из URL-адреса
std::string extract_path(const std::string &url) {
    std::stringstream ss(url);
    std::string path;
    std::getline(ss, path, '?');
    return path;
}

int main() {
    database::createTables();
    try {
        net::io_context ioc;
        net::ip::tcp::acceptor acceptor(ioc, {net::ip::tcp::v4(), 8080});

        std::cout << "HTTP server running on port 8080" << std::endl;

        for (;;) {
            net::ip::tcp::socket socket(ioc);
            acceptor.accept(socket);

            beast::flat_buffer buffer;
            http::request<http::string_body> req;
            http::read(socket, buffer, req);

            http::response<http::string_body> res;
            res.version(req.version());
            res.keep_alive(req.keep_alive());

            if (req.method() == http::verb::post) {
                std::string request_path = extract_path(std::string(req.target().data(), req.target().size()));
                if (request_path == "/register") {
                    using json = nlohmann::json;
                    json data = json::parse(req.body());

                    auto username = data["username"].get<std::string>();
                    auto password = data["password"].get<std::string>();

                    bool access = true;

                    User user(username, password);
                    try {
                        database::insertUser(user);
                    } catch (const std::exception &e) {
                        access = false;
                    }

                    if (access) {
                        User responseEntity = database::selectUserByUsername(username);
                        res.result(http::status::ok);
                        res.set(http::field::content_type, "application/json");
                        res.body() = responseEntity.toJson().dump();
                        res.prepare_payload();
                    } else {
                        res.result(http::status::conflict);
                        res.set(http::field::content_type, "application/json");
                        res.body() = "{\"error\":\"Username already exists\"}";
                    }
                } else if (request_path == "/login") {
                    using json = nlohmann::json;
                    json data = json::parse(req.body());

                    auto username = data["username"].get<std::string>();
                    auto password = data["password"].get<std::string>();

                    User user(username, password);

                    if (authenticate_user(user)) {
                        User responseEntity = database::selectUserByUsername(username);
                        res.result(http::status::ok);
                        res.set(http::field::content_type, "application/json");
                        res.body() = responseEntity.toJson().dump();
                        res.prepare_payload();
                    } else {
                        res.result(http::status::unauthorized);
                        res.set(http::field::content_type, "application/json");
                        res.body() = "{\"error\":\"Invalid username or password\"}";
                    }
                } else if (request_path == "/password/save") {
                    using json = nlohmann::json;
                    json data = json::parse(req.body());

                    json user_data = data["user"];
                    int user_id = user_data["id"].get<int>();
                    std::string user_username = user_data["username"].get<std::string>();
                    std::string user_password = user_data["password"].get<std::string>();
                    User user(user_id, user_username, user_password);

                    auto name = data["name"].get<std::string>();
                    auto password = data["password"].get<std::string>();
                    auto notes = data["notes"].get<std::string>();
                    auto userId = data["userId"].get<int>();

                    if (authenticate_user(user)) {
                        Password requestEntity(name, password, notes, userId);
                        database::updatePassword(requestEntity);
                        res.result(http::status::ok);
                        res.set(http::field::content_type, "application/json");
                    } else {
                        res.result(http::status::unauthorized);
                        res.set(http::field::content_type, "application/json");
                        res.body() = "{\"error\":\"Invalid username or password\"}";
                    }

                } else if (request_path == "/password/delete") {
                    using json = nlohmann::json;
                    json data = json::parse(req.body());

                    json user_data = data["user"];
                    int user_id = user_data["id"].get<int>();
                    std::string user_username = user_data["username"].get<std::string>();
                    std::string user_password = user_data["password"].get<std::string>();
                    User user(user_id, user_username, user_password);

                    auto id = data["id"].get<int>();
                    auto name = data["name"].get<std::string>();
                    auto password = data["password"].get<std::string>();
                    auto notes = data["notes"].get<std::string>();
                    auto userId = data["userId"].get<int>();

                    if (authenticate_user(user)) {
                        database::deletePasswordById(id);
                        res.result(http::status::ok);
                        res.set(http::field::content_type, "application/json");
                    } else {
                        res.result(http::status::unauthorized);
                        res.set(http::field::content_type, "application/json");
                        res.body() = "{\"error\":\"Invalid username or password\"}";
                    }
                } else if (request_path == "/password/update") {
                    using json = nlohmann::json;
                    json data = json::parse(req.body());

                    json user_data = data["user"];
                    int user_id = user_data["id"].get<int>();
                    std::string user_username = user_data["username"].get<std::string>();
                    std::string user_password = user_data["password"].get<std::string>();
                    User user(user_id, user_username, user_password);

                    auto id = data["id"].get<int>();
                    auto name = data["name"].get<std::string>();
                    auto password = data["password"].get<std::string>();
                    auto notes = data["notes"].get<std::string>();
                    auto userId = data["userId"].get<int>();

                    if (authenticate_user(user)) {
                        database::deletePasswordById(id);
                        Password requestEntity(name, password, notes, userId);
                        database::updatePassword(requestEntity);
                        res.result(http::status::ok);
                        res.set(http::field::content_type, "application/json");
                    } else {
                        res.result(http::status::unauthorized);
                        res.set(http::field::content_type, "application/json");
                        res.body() = "{\"error\":\"Invalid username or password\"}";
                    }
                } else {

                    res.result(http::status::not_found);
                    res.set(http::field::content_type, "application/json");
                    res.body() = "{\"error\":\"Endpoint not found\"}";
                }
            } else if (req.method() == http::verb::get) {
                std::string request_path = extract_path(std::string(req.target().data(), req.target().size()));
                if (request_path == "/password") {
                    std::string target_str(req.target().data(), req.target().size());
                    std::size_t query_pos = target_str.find('?');
                    if (query_pos != std::string::npos) {
                        std::string query_str = target_str.substr(query_pos + 1);
                        std::size_t id_pos = query_str.find("id=");
                        if (id_pos != std::string::npos) {
                            std::string id_str = query_str.substr(id_pos + 3);
                            try {

                                using json = nlohmann::json;
                                json data = json::parse(req.body());

                                json user_data = data["user"];
                                int user_id = user_data["id"].get<int>();
                                std::string user_username = user_data["username"].get<std::string>();
                                std::string user_password = user_data["password"].get<std::string>();
                                User user(user_id, user_username, user_password);
                                if (authenticate_user(user)) {
                                    int id = std::stoi(id_str);
                                    Password password = database::selectPasswordById(id);
                                    res.result(http::status::ok);
                                    res.set(http::field::content_type, "application/json");
                                    res.body() = password.toJson().dump();
                                    res.prepare_payload();
                                } else {
                                    res.result(http::status::unauthorized);
                                    res.set(http::field::content_type, "application/json");
                                    res.body() = "{\"error\":\"Invalid username or password\"}";
                                }
                            } catch (const std::exception &e) {
                                res.result(http::status::not_found);
                                res.set(http::field::content_type, "application/json");
                                res.body() = "{\"error\":\"Password not found\"}";
                            }
                        }
                    }
                } else if (request_path == "/password/list") {
                    using json = nlohmann::json;
                    json data = json::parse(req.body());

                    json user_data = data["user"];
                    int user_id = user_data["id"].get<int>();
                    std::string user_username = user_data["username"].get<std::string>();
                    std::string user_password = user_data["password"].get<std::string>();
                    User user(user_id, user_username, user_password);
                    if (authenticate_user(user)) {
                        auto vector = database::selectPasswordByUserId(user_id);

                        json response = json::array();
                        for (auto &password: vector) {
                            json password_json = {
                                    {"id",       password.getId()},
                                    {"name",     password.getName()},
                                    {"password", password.getPassword()},
                                    {"notes",    password.getNotes()},
                                    {"userId",   password.getUserId()}
                            };
                            response.push_back(password_json);
                        }

                        res.result(http::status::ok);
                        res.set(http::field::content_type, "application/json");
                        res.body() = response.dump();
                        res.prepare_payload();
                    } else {
                        res.result(http::status::unauthorized);
                        res.set(http::field::content_type, "application/json");
                        res.body() = "{\"error\":\"Invalid username or password\"}";
                    }
                }
            } else {
                res.result(http::status::method_not_allowed);
                res.set(http::field::content_type, "application/json");
                res.body() = "{\"error\":\"Method not allowed\"}";
            }

            res.prepare_payload();
            http::write(socket, res);
            socket.shutdown(net::ip::tcp::socket::shutdown_send);
        }
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
