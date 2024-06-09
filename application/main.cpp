#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QDebug>
#include <QMessageBox>


class ClickableLabel : public QLabel {
Q_OBJECT

public:
    explicit ClickableLabel(const QString &text, QWidget *parent = nullptr) : QLabel(text, parent) {
        setStyleSheet("QLabel { cursor: pointer; }");
        connect(this, &ClickableLabel::mouseClicked, this, &ClickableLabel::handleClick);
    }

signals:

    void mouseClicked(QMouseEvent *event);

protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            emit mouseClicked(event);
        }
        QLabel::mousePressEvent(event);
    }

private slots:

    void handleClick(QMouseEvent *event) {
        qDebug() << "Метка была нажата!";

    }
};

void onClickRegisterButton() {
    QMessageBox msgBox;     // диалоговое окно
    msgBox.setText("Button Clicked!"); // устанавливаем текст
    msgBox.exec();  // отображаем диалоговое окно
}


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QWidget widget;
    widget.setWindowTitle("Login widget");
    widget.setMinimumSize(600, 400);

    bool signedUp = false;

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
    QObject::connect(registerButton, &QPushButton::clicked, []{
        onClickRegisterButton();
    });

    auto label = new ClickableLabel("Уже зарегистрированы?");

    auto *verticalLayout = new QVBoxLayout();
    verticalLayout->setAlignment(Qt::AlignCenter);

    verticalLayout->addWidget(userNameLabel);
    verticalLayout->addWidget(usernameEdit);
    verticalLayout->addWidget(passwordLabel);
    verticalLayout->addWidget(passwordEdit);
    verticalLayout->addWidget(registerButton, 0, Qt::AlignCenter);
    verticalLayout->addWidget(label, 0, Qt::AlignCenter);


    widget.setLayout(verticalLayout);
    widget.show();
    return QApplication::exec();
}



#include "main.moc"

