#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QMessageBox>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <QTextEdit>
#include <iostream>
#include <string>
#include "json.hpp"
#include <sstream>
#include "./models/user.h"
#include <QStringListModel>
#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QUrl>
#include <QListWidget>
#include "./models/password.h"

using json = nlohmann::json;

/**
 * @brief Парсит ответ HTTP-сервера и возвращает пару, содержащую строку состояния и тело ответа.
 *
 * Функция принимает строку ответа HTTP-сервера и разбирает ее на строку состояния и тело ответа.
 *
 * @param response Строка ответа HTTP-сервера.
 * @return Пара, содержащая строку состояния (первый элемент) и тело ответа (второй элемент).
 */
std::pair<std::string, std::string> parseResponse(const std::string &response) {
  std::istringstream responseStream(response);
  std::string line;
  std::string statusLine;
  std::string body;

  if (std::getline(responseStream, line)) {
    statusLine = line;
  }

  while (std::getline(responseStream, line) && line != "\r") {}

  while (std::getline(responseStream, line)) {
    body += line;
  }

  return std::make_pair(statusLine, body);
}

/**
 * @brief Removes all leading and trailing whitespace characters from a string.
 *
 * @param str The input string to be trimmed.
 * @return QString A new string with all leading and trailing whitespace characters removed.
 *
 * This function takes a QString `str` as input and removes all whitespace characters (spaces, tabs, newlines, etc.)
 * from the beginning and end of the string. It returns a new QString containing the trimmed content.
 *
 * Example usage:
 * @code
 * QString original = "   Hello, World!   ";
 * QString trimmed = trim(original); // trimmed = "Hello, World!"
 * @endcode
 */
QString trim(const QString &str) {
  int start = 0;
  while (start < str.length() && str[start].isSpace()) {
    ++start;
  }
  int end = str.length() - 1;
  while (end >= 0 && str[end].isSpace()) {
    --end;
  }
  return str.mid(start, end - start + 1);
}

/**
 * @brief Парсит JSON-данные из ответа сервера и создает объект класса User.
 *
 * Функция принимает строку ответа сервера и извлекает из нее JSON-данные, содержащие
 * идентификатор, имя пользователя и пароль. Затем создается объект класса User с полученными данными.
 *
 * @param response Строка ответа сервера.
 * @return Объект класса User с данными из ответа сервера.
 */
User parseUser(const std::string &response) {
  const std::pair<std::string, std::string> res = parseResponse(response);
  auto status = res.first;
  auto body = res.second;

  User user(-1, "", "");

  try {
    json data = json::parse(body);
    auto id = data["id"].get<int>();
    auto username = data["username"].get<std::string>();
    auto password = data["password"].get<std::string>();
    user.setId(id);
    user.setUsername(username);
    user.setPassword(password);
  } catch (const json::exception &e) {
    std::cerr << "JSON parse error: " << e.what() << std::endl;
  }

  return user;
}

/**
 * @brief Метка, которая реагирует на нажатие мыши и излучает сигналы.
 */
class ClickableLabel : public QLabel {
 Q_OBJECT

 public:
  /**
   * @brief Конструктор класса ClickableLabel.
   * @param text Текст, отображаемый на метке.
   * @param parent Указатель на родительский виджет.
   */
  explicit ClickableLabel(const QString &text, QWidget *parent = nullptr) : QLabel(text, parent),
                                                                            isRegistered(false) {
    setStyleSheet("QLabel { cursor: pointer; }");
    connect(this, &ClickableLabel::mouseClicked, this, &ClickableLabel::handleClick);
  }

  /**
   * @brief Флаг, указывающий, зарегистрирован ли пользователь.
   */
  bool isRegistered;

 signals:

  /**
  * @brief Сигнал, излучаемый при нажатии на метку.
  * @param event Событие нажатия мыши.
  */
  void mouseClicked(QMouseEvent *event);

  /**
   * @brief Сигнал, излучаемый при нажатии на метку.
   */
  void labelClicked();

 protected:
  /**
   * @brief Переопределенный метод для обработки события нажатия мыши.
   * @param event Событие нажатия мыши.
   */
  void mousePressEvent(QMouseEvent *event) override {
    if (event->button() == Qt::LeftButton) {
      emit mouseClicked(event);
    }
    QLabel::mousePressEvent(event);
  }

 private slots:

  /**
   * @brief Слот, вызываемый при нажатии на метку.
   * @param event Событие нажатия мыши.
   */
  void handleClick(QMouseEvent *event) {
    isRegistered = !isRegistered;
    setText(isRegistered ? "Зарегистрироваться" : "Уже зарегистрированы?");
    emit labelClicked();
  }
};

/**
 * @brief Отправляет HTTP-запрос на сервер для регистрации или авторизации пользователя.
 *
 * Функция создает сокет, устанавливает соединение с сервером и отправляет HTTP-запрос
 * POST с телом, содержащим имя пользователя и пароль. Затем она получает ответ от сервера
 * и возвращает его в виде строки.
 *
 * @param username Имя пользователя.
 * @param password Пароль пользователя.
 * @param isLogin Флаг, указывающий, является ли запрос авторизацией (true) или регистрацией (false).
 * @return Строка ответа, полученная от сервера.
 */
std::string sendRequest(const std::string &username, const std::string &password, bool isLogin) {
  std::string response;

  try {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
      throw std::runtime_error("Failed to create socket");
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);

    if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) <= 0) {
      throw std::runtime_error("Invalid address or address not supported");
    }

    if (connect(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0) {
      throw std::runtime_error("Connection failed");
    }

    std::string path = isLogin ? "/login" : "/register";
    std::string body = "{\"username\":\"" + username + "\",\"password\":\"" + password + "\"}";

    std::string request = "POST " + path + " HTTP/1.1\r\n";
    request += "Host: example.com\r\n";
    request += "Content-Type: application/json\r\n";
    request += "Content-Length: " + std::to_string(body.length()) + "\r\n";
    request += "Connection: close\r\n\r\n";
    request += body;

    if (send(sockfd, request.c_str(), request.length(), 0) < 0) {
      throw std::runtime_error("Failed to send request");
    }

    char buffer[4096];
    ssize_t bytes_read;
    while ((bytes_read = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
      buffer[bytes_read] = '\0';
      response += buffer;
    }

    close(sockfd);
  } catch (const std::exception &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return response;
}

/**
 * @brief Виджет для отображения и редактирования деталей пароля.
 */
class PasswordDetailsWidget : public QWidget {
 Q_OBJECT

 public:
  /**
   * @brief Конструктор класса PasswordDetailsWidget.
   * @param parent Указатель на родительский виджет.
   *
   * Конструктор создает и настраивает виджеты для отображения и редактирования
   * деталей пароля, включая название сервиса, пароль, заметки, а также кнопки
   * "Удалить пароль" и "Сохранить изменения".
   */
  explicit PasswordDetailsWidget(QWidget *parent = nullptr) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);

    auto *usernameLabel = new QLabel("Название сервиса:", this);
    usernameLabel->setAlignment(Qt::AlignCenter);
    usernameLabel->setStyleSheet("font-size: 16px");

    m_usernameLineEdit = new QLineEdit(this);
    m_usernameLineEdit->setStyleSheet("font-size: 16px");

    auto *passwordLabel = new QLabel("Пароль:", this);
    passwordLabel->setAlignment(Qt::AlignCenter);
    passwordLabel->setStyleSheet("margin-top: 30px; font-size: 16px;");

    m_passwordLineEdit = new QLineEdit(this);
    m_passwordLineEdit->setStyleSheet("font-size: 16px");

    auto *notesLabel = new QLabel("Notes:", this);
    notesLabel->setAlignment(Qt::AlignCenter);
    notesLabel->setStyleSheet("font-size: 16px");

    m_notesTextEdit = new QTextEdit(this);
    m_notesTextEdit->setStyleSheet("font-size: 16px");

    auto *deleteButton = new QPushButton("Удалить пароль", this);
    deleteButton->setFixedSize(250, 70);
    deleteButton->setStyleSheet("font-size: 20px; margin-left: 30px");
    connect(deleteButton, &QPushButton::clicked, this, &PasswordDetailsWidget::onDeleteButtonClicked);

    auto *editButton = new QPushButton("Сохранить изменения", this);
    editButton->setFixedSize(250, 70);
    editButton->setStyleSheet("font-size: 20px");
    connect(editButton, &QPushButton::clicked, this, &PasswordDetailsWidget::onEditButtonClicked);

    auto *leftWidget = new QWidget(this);
    auto *rightWidget = new QWidget(this);
    auto *buttons = new QWidget(this);

    auto leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->addWidget(usernameLabel);
    leftLayout->addWidget(m_usernameLineEdit);
    leftLayout->addWidget(passwordLabel);
    leftLayout->addWidget(m_passwordLineEdit);
    leftLayout->setAlignment(Qt::AlignCenter);

    auto rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->addWidget(notesLabel);
    rightLayout->addWidget(m_notesTextEdit);
    rightLayout->setAlignment(Qt::AlignCenter);

    auto *upperWidget = new QWidget(this);
    auto upperLayout = new QHBoxLayout(upperWidget);
    upperLayout->addWidget(leftWidget);
    upperLayout->addWidget(rightWidget);

    auto buttonsLayout = new QHBoxLayout(buttons);
    buttonsLayout->addWidget(deleteButton);
    buttonsLayout->addWidget(editButton);

    layout->addWidget(upperWidget);
    layout->addWidget(buttons);
    layout->setAlignment(Qt::AlignCenter);
  }

  /**
   * @brief Обновляет детали пароля в виджете.
   * @param passwordId Идентификатор пароля.
   * @param userId Идентификатор пользователя.
   * @param username Имя пользователя.
   * @param user_password Пароль пользователя.
   * @param name Название сервиса.
   * @param password Пароль.
   * @param notes Заметки.
   *
   * Этот метод обновляет поля ввода виджета с предоставленными деталями пароля.
   */
  void updateDetails(int passwordId, int userId, std::string username, std::string user_password, const QString &name,
                     const QString &password, const QString &notes) {
    m_passwordId = passwordId;
    m_userId = userId;
    m_username = username;
    m_user_password = user_password;
    m_usernameLineEdit->setText(name);
    m_passwordLineEdit->setText(password);
    m_notesTextEdit->setText(notes);
  }

 signals:

  /**
   * @brief Сигнал, излучаемый при успешном удалении пароля.
   */
  void passwordDeleted();

  /**
   * @brief Сигнал, излучаемый при успешном обновлении пароля.
   */
  void passwordUpdate();

 private slots:

  /**
   * @brief Слот, вызываемый при нажатии кнопки "Удалить пароль".
   *
   * Этот слот отправляет запрос на сервер для удаления пароля.
   */
  void onDeleteButtonClicked() {
    auto *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &PasswordDetailsWidget::onDeleteRequestFinished);

    QUrl url("http://localhost:8080/password/delete");

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject userObject;
    userObject["username"] = QString::fromStdString(m_username);
    userObject["password"] = QString::fromStdString(m_user_password);
    userObject["id"] = m_userId;

    QJsonObject json;
    json["id"] = m_passwordId;
    json["user"] = userObject;

    QByteArray data = QJsonDocument(json).toJson();

    manager->post(request, data);
  }

  /**
   * @brief Слот, вызываемый после завершения запроса на удаление пароля.
   * @param reply Ответ на запрос от сервера.
   *
   * Этот слот обрабатывает ответ от сервера на запрос удаления пароля.
   * Если запрос успешен, поля ввода очищаются, виджет скрывается,
   * и излучается сигнал passwordDeleted.
   * Если произошла ошибка, отображается предупреждающее сообщение.
   */
  void onDeleteRequestFinished(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
      m_usernameLineEdit->clear();
      m_passwordLineEdit->clear();
      m_notesTextEdit->clear();

      this->setVisible(false);

      emit passwordDeleted();
    } else {
      QMessageBox::warning(nullptr, "Ошибка", "Ошибка сервера.");
    }

    reply->deleteLater();
  }

  /**
   * @brief Слот, вызываемый при нажатии кнопки "Сохранить изменения".
   *
   * Этот слот отправляет запрос на сервер для обновления пароля.
   */
  void onEditButtonClicked() {
    auto *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &PasswordDetailsWidget::onEditRequestFinished);

    QUrl url("http://localhost:8080/password/update");

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString updatedName = trim(m_usernameLineEdit->text());
    QString updatedPassword = trim(m_passwordLineEdit->text());
    QString updatedNotes = trim(m_notesTextEdit->toPlainText());

    if (updatedName.isEmpty() && updatedPassword.isEmpty()) {
      QMessageBox::warning(nullptr, "Ошибка", "Заполните пустые поля.");
      return;
    }

    QJsonObject userObject;
    userObject["username"] = QString::fromStdString(m_username);
    userObject["password"] = QString::fromStdString(m_user_password);
    userObject["id"] = m_userId;

    QJsonObject json;
    json["id"] = m_passwordId;
    json["name"] = updatedName;
    json["password"] = updatedPassword;
    json["notes"] = updatedNotes;
    json["userId"] = m_userId;
    json["user"] = userObject;

    QByteArray data = QJsonDocument(json).toJson();

    manager->post(request, data);
  }

  /**
   * @brief Слот, вызываемый после завершения запроса на обновление пароля.
   * @param reply Ответ на запрос от сервера.
   *
   * Этот слот обрабатывает ответ от сервера на запрос обновления пароля.
   * Если запрос успешен, излучается сигнал passwordUpdate.
   * Если произошла ошибка, отображается предупреждающее сообщение.
   */
  void onEditRequestFinished(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
      emit passwordUpdate();
    } else {
      QMessageBox::warning(nullptr, "Ошибка", "Ошибка сервера.");
    }

    reply->deleteLater();
  }

 private:
  /**
   * @brief Идентификатор пароля.
   */
  int m_passwordId;

  /**
   * @brief Идентификатор пользователя.
   */
  int m_userId;

  /**
   * @brief Имя пользователя.
   */
  std::string m_username;

  /**
   * @brief Пароль пользователя.
   */
  std::string m_user_password;

  /**
   * @brief Виджет для ввода названия сервиса.
   */
  QLineEdit *m_usernameLineEdit;

  /**
   * @brief Виджет для ввода пароля.
   */
  QLineEdit *m_passwordLineEdit;

  /**
   * @brief Виджет для ввода заметок.
   */
  QTextEdit *m_notesTextEdit;
};

/**
 * @brief Виджет для добавления нового пароля.
 */
class AddPasswordWidget : public QWidget {
 Q_OBJECT

 public:
  /**
   * @brief Конструктор класса AddPasswordWidget.
   * @param parent Указатель на родительский виджет.
   */
  explicit AddPasswordWidget(QWidget *parent = nullptr) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);

    auto *usernameLabel = new QLabel("Название сервиса:", this);
    usernameLabel->setAlignment(Qt::AlignCenter);
    usernameLabel->setStyleSheet("font-size: 16px");

    usernameLineEdit = new QLineEdit(this);
    usernameLineEdit->setStyleSheet("font-size: 16px");

    auto *passwordLabel = new QLabel("Пароль:", this);
    passwordLabel->setAlignment(Qt::AlignCenter);
    passwordLabel->setStyleSheet("margin-top: 30px; font-size: 16px;");

    passwordLineEdit = new QLineEdit(this);
    passwordLineEdit->setStyleSheet("font-size: 16px");

    auto *notesLabel = new QLabel("Notes:", this);
    notesLabel->setAlignment(Qt::AlignCenter);
    notesLabel->setStyleSheet("font-size: 16px");

    notesTextEdit = new QTextEdit(this);
    notesTextEdit->setStyleSheet("font-size: 16px");

    auto *addButton = new QPushButton("Добавить пароль", this);
    addButton->setFixedSize(350, 70);
    addButton->setStyleSheet("font-size: 20px");
    connect(addButton, &QPushButton::clicked, this, &AddPasswordWidget::onAddButtonClicked);

    auto *leftWidget = new QWidget(this);
    auto *rightWidget = new QWidget(this);

    auto leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->addWidget(usernameLabel);
    leftLayout->addWidget(usernameLineEdit);
    leftLayout->addWidget(passwordLabel);
    leftLayout->addWidget(passwordLineEdit);
    leftLayout->setAlignment(Qt::AlignCenter);

    auto rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->addWidget(notesLabel);
    rightLayout->addWidget(notesTextEdit);
    rightLayout->setAlignment(Qt::AlignCenter);

    auto *upperWidget = new QWidget(this);
    auto upperLayout = new QHBoxLayout(upperWidget);
    upperLayout->addWidget(leftWidget);
    upperLayout->addWidget(rightWidget);

    layout->addWidget(upperWidget);
    layout->addWidget(addButton, 0, Qt::AlignCenter);
    layout->setAlignment(Qt::AlignCenter);
  }

  /**
   * @brief Обновляет данные пользователя в виджете.
   * @param userId Идентификатор пользователя.
   * @param username Имя пользователя.
   * @param user_password Пароль пользователя.
   */
  void updateDetails(int userId, std::string username, std::string user_password) {
    m_userId = userId;
    m_username = username;
    m_password = user_password;
  }

 signals:

  /**
   * @brief Сигнал, излучаемый при сохранении нового пароля.
   */
  void passwordSave();

 private slots:

  /**
   * @brief Слот, вызываемый при нажатии кнопки "Добавить пароль".
   *
   * Этот слот отправляет POST-запрос на сервер для сохранения нового пароля.
   * Данные пароля (название, пароль, заметки) и данные пользователя (имя пользователя,
   * пароль, идентификатор) отправляются в JSON-формате в теле запроса.
   */
  void onAddButtonClicked() {
    auto *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &AddPasswordWidget::onAddRequestFinished);

    QUrl url("http://localhost:8080/password/save");

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString updatedName = trim(usernameLineEdit->text());
    QString updatedPassword = trim(passwordLineEdit->text());
    QString updatedNotes = trim(notesTextEdit->toPlainText());

    if (updatedName.isEmpty() && updatedPassword.isEmpty()) {
      QMessageBox::warning(nullptr, "Ошибка", "Заполните пустые поля.");
      return;
    }

    QJsonObject userObject;
    userObject["username"] = QString::fromStdString(m_username);
    userObject["password"] = QString::fromStdString(m_password);
    userObject["id"] = m_userId;

    QJsonObject json;
    json["name"] = updatedName;
    json["password"] = updatedPassword;
    json["notes"] = updatedNotes;
    json["userId"] = m_userId;
    json["user"] = userObject;

    QByteArray data = QJsonDocument(json).toJson();

    manager->post(request, data);
  }

  /**
   * @brief Слот, вызываемый после завершения запроса на сохранение пароля.
   * @param reply Ответ на запрос от сервера.
   *
   * Этот слот обрабатывает ответ от сервера на запрос сохранения пароля.
   * Если запрос успешен, поля ввода очищаются, виджет скрывается, и излучается
   * сигнал passwordSave. Если произошла ошибка, отображается предупреждающее сообщение.
   */
  void onAddRequestFinished(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
      usernameLineEdit->clear();
      passwordLineEdit->clear();
      notesTextEdit->clear();

      this->setVisible(false);

      emit passwordSave();
    } else {
      QMessageBox::warning(nullptr, "Ошибка", "Ошибка сервера.");
    }
    reply->deleteLater();
  }

 private:
  /**
   * @brief Идентификатор пользователя.
   */
  int m_userId;

  /**
   * @brief Имя пользователя.
   */
  std::string m_username;

  /**
   * @brief Пароль пользователя.
   */
  std::string m_password;

  /**
   * @brief Виджет для ввода названия сервиса.
   */
  QLineEdit *usernameLineEdit;

  /**
   * @brief Виджет для ввода пароля.
   */
  QLineEdit *passwordLineEdit;

  /**
   * @brief Виджет для ввода заметок.
   */
  QTextEdit *notesTextEdit;
};

/**
 * @brief Виджет для отображения списка паролей пользователя.
 *
 * Этот виджет содержит QListWidget для отображения списка паролей пользователя.
 * При инициализации виджет отправляет GET-запрос на сервер для получения списка паролей.
 * При выборе элемента в списке излучается сигнал itemClicked с передачей информации о выбранном пароле.
 */

class DataListWidget : public QWidget {
 Q_OBJECT

 public:
  /**
   * @brief Конструктор класса DataListWidget.
   * @param user Объект класса User, содержащий информацию о пользователе.
   * @param parent Указатель на родительский виджет.
   *
   * Конструктор инициализирует виджет, создает QListWidget для отображения списка паролей,
   * настраивает сигналы и слоты, а также отправляет GET-запрос на сервер для получения списка паролей.
   */
  explicit DataListWidget(const User &user, QWidget *parent = nullptr) : QWidget(parent), m_user(user) {
    auto *layout = new QVBoxLayout(this);

    m_listWidget = new QListWidget(this);
    m_listWidget->setFixedSize(300, 400);
    m_listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    connect(m_listWidget, &QListWidget::itemClicked, this, &DataListWidget::onItemClicked);
    layout->addWidget(m_listWidget);

    m_networkManager = new QNetworkAccessManager(this);
    connect(m_networkManager, &QNetworkAccessManager::finished, this, &DataListWidget::onHttpRequestFinished);

    QUrl url("http://localhost:8080/password/list");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject userObject;
    userObject["username"] = QString::fromStdString(m_user.getUsername());
    userObject["password"] = QString::fromStdString(m_user.getPassword());
    userObject["id"] = m_user.getId();

    QJsonObject requestBody;
    requestBody["user"] = userObject;

    QJsonDocument jsonDoc(requestBody);
    QByteArray requestData = jsonDoc.toJson();

    m_networkManager->sendCustomRequest(request, "GET", requestData);
  }

 signals:

  /**
   * @brief Сигнал, излучаемый при выборе элемента в списке паролей.
   * @param passwordId Идентификатор выбранного пароля.
   * @param password Объект класса Password, содержащий информацию о выбранном пароле.
   */

  void itemClicked(int passwordId, const Password &password);

 public slots:

  /**
   * @brief Слот для обновления списка паролей.
   *
   * Этот слот очищает текущий список паролей в QListWidget и вектор m_passwords,
   * а затем отправляет GET-запрос на сервер для получения обновленного списка паролей.
   *
   * После получения ответа от сервера, список паролей будет заполнен новыми данными
   * в методе parseJsonResponse.
   */

  void updatePasswordList() {
    m_listWidget->clear();
    m_passwords.clear();

    QUrl url("http://localhost:8080/password/list");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject userObject;
    userObject["username"] = QString::fromStdString(m_user.getUsername());
    userObject["password"] = QString::fromStdString(m_user.getPassword());
    userObject["id"] = m_user.getId();

    QJsonObject requestBody;
    requestBody["user"] = userObject;

    QJsonDocument jsonDoc(requestBody);
    QByteArray requestData = jsonDoc.toJson();

    m_networkManager->sendCustomRequest(request, "GET", requestData);
  }

 private slots:

  /**
   * @brief Слот, вызываемый после получения ответа от сервера на запрос списка паролей.
   * @param reply Объект QNetworkReply, содержащий ответ от сервера.
   *
   * Этот слот обрабатывает ответ от сервера и заполняет QListWidget элементами списка паролей.
   */
  void onHttpRequestFinished(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
      QByteArray responseData = reply->readAll();
      parseJsonResponse(responseData);
    } else {
      qDebug() << "Error:" << reply->errorString();
    }

    reply->deleteLater();
  }

  /**
   * @brief Слот, вызываемый при выборе элемента в списке паролей.
   * @param item Указатель на выбранный элемент списка.
   *
   * Этот слот излучает сигнал itemClicked с передачей информации о выбранном пароле.
   */

  void onItemClicked(QListWidgetItem *item) {
    int index = m_listWidget->row(item);
    if (index >= 0 && index < m_passwords.size()) {
      int passwordId = m_passwords[index].getId();
      emit itemClicked(passwordId, m_passwords[index]);
    }
  }

 private:
  /**
   * @brief Вектор объектов класса Password для хранения списка паролей.
   */
  QVector<Password> m_passwords;

  /**
   * @brief Разбирает JSON-данные, полученные от сервера, и заполняет список паролей.
   * @param jsonData Массив байтов, содержащий JSON-данные.
   *
   * Эта функция принимает массив байтов, содержащий JSON-данные, полученные от сервера.
   * Она разбирает эти данные и заполняет вектор m_passwords объектами класса Password,
   * а также заполняет QListWidget элементами списка паролей.
   *
   * Ожидается, что JSON-данные представляют собой массив объектов, каждый из которых
   * содержит информацию о пароле: идентификатор (id), название (name), пароль (password),
   * заметки (notes) и идентификатор пользователя (userId).
   *
   * Функция очищает существующий вектор m_passwords перед заполнением его новыми данными.
   * Для каждого объекта в JSON-массиве создается объект класса Password и добавляется в
   * вектор m_passwords. Также создается элемент списка QListWidgetItem с названием пароля
   * и добавляется в QListWidget m_listWidget.
   */
  void parseJsonResponse(const QByteArray &jsonData) {
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (!jsonDoc.isNull() && jsonDoc.isArray()) {
      QJsonArray jsonArray = jsonDoc.array();
      m_passwords.clear();

      for (const QJsonValue &value : jsonArray) {
        if (value.isObject()) {
          QJsonObject jsonObj = value.toObject();
          int id = jsonObj["id"].toInt();
          QString name = jsonObj["name"].toString();
          QString password = jsonObj["password"].toString();
          QString notes = jsonObj["notes"].toString();
          int userId = jsonObj["userId"].toInt();

          Password passwordObj(id, name.toStdString(), password.toStdString(), notes.toStdString(), userId);
          m_passwords.append(passwordObj);

          QListWidgetItem *item = new QListWidgetItem(name);
          item->setSizeHint(QSize(0, 40));
          m_listWidget->addItem(item);
        }
      }
    }
  }

  /**
   * @brief Указатель на QNetworkAccessManager для выполнения сетевых запросов.
   */
  QNetworkAccessManager *m_networkManager;

  /**
   * @brief Объект класса User, содержащий информацию о пользователе.
   */
  User m_user;

  /**
   * @brief Указатель на QListWidget для отображения списка паролей.
   */
  QListWidget *m_listWidget;
};

/**
 * @brief Главный виджет панели управления.
 *
 * Этот виджет является основным виджетом панели управления и содержит другие виджеты,
 * такие как список паролей, форма добавления пароля и форма деталей пароля.
 * Он также обрабатывает взаимодействие между этими виджетами.
 */
class DashboardWidget : public QWidget {
 Q_OBJECT

 public:
  /**
   * @brief Конструктор класса DashboardWidget.
   * @param user Объект класса User, содержащий информацию о пользователе.
   * @param parent Указатель на родительский виджет.
   *
   * Конструктор инициализирует главный виджет, создает и настраивает дочерние виджеты,
   * такие как список паролей, форма добавления пароля и форма деталей пароля.
   * Также устанавливаются необходимые соединения сигналов и слотов между виджетами.
   */
  explicit DashboardWidget(const User &user, QWidget *parent = nullptr) : QWidget(parent), user_data(user) {
    this->setWindowTitle("Dashboard");
    this->setMinimumSize(1100, 600);

    auto *layout = new QHBoxLayout(this);

    auto addButton = new QPushButton("+ Добавить пароль");
    addButton->setStyleSheet("width: 300px; height: 40px; font-size: 16px");
    connect(addButton, &QPushButton::clicked, this, &DashboardWidget::onAddPasswordClicked);

    m_dataListWidget = new DataListWidget(user, this);

    QWidget *leftWidget = new QWidget(this);
    leftWidget->setFixedWidth(300);
    leftWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    auto *verticalLayout = new QVBoxLayout(leftWidget);
    verticalLayout->addWidget(addButton);
    verticalLayout->addWidget(m_dataListWidget);

    m_addPasswordWidget = new AddPasswordWidget(this);
    m_addPasswordWidget->setVisible(false);

    m_passwordDetailsWidget = new PasswordDetailsWidget(this);
    m_passwordDetailsWidget->setVisible(false);

    layout->addWidget(leftWidget);
    layout->addWidget(m_addPasswordWidget, 1);
    layout->addWidget(m_passwordDetailsWidget, 1);
    layout->setAlignment(leftWidget, Qt::AlignLeft);

    connect(m_dataListWidget, &DataListWidget::itemClicked, this, &DashboardWidget::onItemClicked);
    connect(m_passwordDetailsWidget, &PasswordDetailsWidget::passwordDeleted,
            m_dataListWidget, &DataListWidget::updatePasswordList);

    connect(m_addPasswordWidget, &AddPasswordWidget::passwordSave,
            m_dataListWidget, &DataListWidget::updatePasswordList);

    connect(m_passwordDetailsWidget, &PasswordDetailsWidget::passwordUpdate,
            m_dataListWidget, &DataListWidget::updatePasswordList);

  }

 private slots:

  /**
   * @brief Слот, вызываемый при нажатии кнопки "Добавить пароль".
   *
   * Этот слот отображает виджет AddPasswordWidget для добавления нового пароля
   * и скрывает виджет PasswordDetailsWidget.
   */
  void onAddPasswordClicked() {
    m_addPasswordWidget->setVisible(true);
    m_addPasswordWidget->updateDetails(user_data.getId(), user_data.getUsername(), user_data.getPassword());
    m_passwordDetailsWidget->setVisible(false);
  }

  /**
   * @brief Слот, вызываемый при выборе элемента в списке паролей.
   * @param passwordId Идентификатор выбранного пароля.
   * @param password Объект класса Password, содержащий информацию о выбранном пароле.
   *
   * Этот слот отображает виджет PasswordDetailsWidget с деталями выбранного пароля
   * и скрывает виджет AddPasswordWidget.
   */
  void onItemClicked(int passwordId, const Password &password) {
    m_addPasswordWidget->setVisible(false);
    m_passwordDetailsWidget->setVisible(true);
    m_passwordDetailsWidget->updateDetails(passwordId, user_data.getId(), user_data.getUsername(),
                                           user_data.getPassword(),
                                           QString::fromStdString(password.getName()),
                                           QString::fromStdString(password.getPassword()),
                                           QString::fromStdString(password.getNotes()));
  }

 private:
  /**
   * @brief Объект класса User, содержащий информацию о пользователе.
   */
  User user_data;

  /**
   * @brief Указатель на виджет DataListWidget для отображения списка паролей.
   */
  DataListWidget *m_dataListWidget;

  /**
   * @brief Указатель на виджет AddPasswordWidget для добавления нового пароля.
   */
  AddPasswordWidget *m_addPasswordWidget;

  /**
   * @brief Указатель на виджет PasswordDetailsWidget для отображения деталей пароля.
   */
  PasswordDetailsWidget *m_passwordDetailsWidget;
};

/**
 * @brief Обрабатывает нажатие кнопки регистрации/входа.
 * @param authWidget Указатель на виджет авторизации.
 * @param label Указатель на метку ClickableLabel.
 * @param usernameEdit Указатель на поле ввода имени пользователя.
 * @param passwordEdit Указатель на поле ввода пароля.
 *
 * Эта функция обрабатывает нажатие кнопки регистрации или входа.
 * Она получает имя пользователя и пароль из полей ввода, проверяет их на пустоту,
 * отправляет запрос на сервер для регистрации или входа, а затем обрабатывает ответ.
 * В случае успешной авторизации открывается главный виджет панели управления DashboardWidget.
 * В случае ошибки отображается предупреждающее сообщение.
 */
void
onClickRegisterButton(QWidget *authWidget, ClickableLabel *label, QLineEdit *usernameEdit, QLineEdit *passwordEdit) {
  std::string username = usernameEdit->text().toStdString();
  std::string password = passwordEdit->text().toStdString();

  if (username.empty() || password.empty()) {
    QMessageBox::warning(nullptr, "Предупреждение", "Пожалуйста, введите имя пользователя и пароль.");
    return;
  }

  bool isLogin = label->isRegistered;
  std::string response = sendRequest(username, password, isLogin);
  User user = parseUser(response);

  if (user.getId() != -1) {
    auto *dashboardWidget = new DashboardWidget(user);
    dashboardWidget->show();
    authWidget->close();
  } else {
    QMessageBox::warning(nullptr, "Ошибка", "Неверные учетные данные или ошибка сервера.");
  }
}

/**
 * @brief Точка входа в приложение.
 * @param argc Количество аргументов командной строки.
 * @param argv Массив аргументов командной строки.
 * @return Код завершения приложения.
 *
 * Эта функция является точкой входа в приложение.
 * Она создает виджет авторизации, настраивает его компоненты (метки, поля ввода, кнопки)
 * и связывает их с соответствующими обработчиками событий.
 * Затем запускается главный цикл событий Qt, и приложение ожидает взаимодействия с пользователем.
 */
int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  auto *widget = new QWidget();
  widget->setWindowTitle("Login widget");
  widget->setMinimumSize(600, 400);

  auto *userNameLabel = new QLabel("Имя пользователя");
  userNameLabel->setAlignment(Qt::AlignCenter);

  auto *passwordLabel = new QLabel("Пароль");
  passwordLabel->setAlignment(Qt::AlignCenter);

  auto usernameEdit = new QLineEdit();
  usernameEdit->setMaximumWidth(300);
  usernameEdit->setAlignment(Qt::AlignCenter);

  auto passwordEdit = new QLineEdit();
  passwordEdit->setMaximumWidth(300);
  passwordEdit->setAlignment(Qt::AlignCenter);

  auto registerButton = new QPushButton("Зарегистрироваться");
  registerButton->setStyleSheet("margin-top: 20px; width: 200px; height: 40px");

  auto label = new ClickableLabel("Уже зарегистрированы?");

  QObject::connect(registerButton, &QPushButton::clicked, [widget, label, usernameEdit, passwordEdit] {
    onClickRegisterButton(widget, label, usernameEdit, passwordEdit);
  });

  QObject::connect(label, &ClickableLabel::labelClicked, [registerButton, label]() {
    registerButton->setText(label->isRegistered ? "Войти" : "Зарегистрироваться");
  });

  auto *verticalLayout = new QVBoxLayout();
  verticalLayout->setAlignment(Qt::AlignCenter);

  verticalLayout->addWidget(userNameLabel);
  verticalLayout->addWidget(usernameEdit);
  verticalLayout->addWidget(passwordLabel);
  verticalLayout->addWidget(passwordEdit);
  verticalLayout->addWidget(registerButton, 0, Qt::AlignCenter);
  verticalLayout->addWidget(label, 0, Qt::AlignCenter);

  widget->setLayout(verticalLayout);
  widget->show();

  int result = QApplication::exec();

  return result;
}

#include "main.moc"
