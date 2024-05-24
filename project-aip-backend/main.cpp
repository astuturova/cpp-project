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

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace pt = boost::property_tree;
// Простое хранилище пользователей
std::unordered_map<std::string, std::string> users;

// Функция для регистрации пользователя
bool register_user(const std::string& username, const std::string& password) {
    // Проверяем, существует ли уже пользователь с таким именем
    if (users.find(username) != users.end()) {
        return false;
    }

    // Добавляем нового пользователя
    users[username] = password;
    return true;
}

// Функция для аутентификации пользователя
bool authenticate_user(const std::string& username, const std::string& password) {
    auto it = users.find(username);
    if (it != users.end() && it->second == password) {
        return true;
    }
    return false;
}

// Функция для генерации JWT токена
std::string generate_jwt_token(const std::string& username) {
    const std::string secret_key = "your_secret_key";
//    auto token = jwt::create()
//            .set_issuer("auth_service")
//            .set_type("JWT")
//            .set_payload_claim("username", username)
//            .sign(jwt::algorithm::hs256(secret_key));
    return "token";
}

// Функция для извлечения пути из URL-адреса
std::string extract_path(const std::string& url) {
    std::stringstream ss(url);
    std::string path;
    std::getline(ss, path, '?');
    return path;
}

int main() {
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
                    std::cout << data["username"] << std::endl;
                    auto username = data["username"].get<std::string>();
                    auto password = data["password"].get<std::string>();

                    if (register_user(username, password)) {
                        res.result(http::status::ok);
                        res.set(http::field::content_type, "application/json");
                        res.body() = "{\"message\":\"User registered successfully\"}";
                    } else {
                        res.result(http::status::conflict);
                        res.set(http::field::content_type, "application/json");
                        res.body() = "{\"error\":\"Username already exists\"}";
                    }
                } else if (request_path == "/login") {
                    pt::ptree data;

                    pt::read_json(req.body(), data);
                    auto username = data.get<std::string>("username");
                    auto password = data.get<std::string>("password");

                    if (authenticate_user(username, password)) {
                        res.result(http::status::ok);
                        res.set(http::field::content_type, "application/json");
                        std::string jwt_token = generate_jwt_token(username);
                        res.body() = "{\"token\":\"" + jwt_token + "\"}";
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
            } else {
                res.result(http::status::method_not_allowed);
                res.set(http::field::content_type, "application/json");
                res.body() = "{\"error\":\"Method not allowed\"}";
            }

            res.prepare_payload();
            http::write(socket, res);
            socket.shutdown(net::ip::tcp::socket::shutdown_send);
        }
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
