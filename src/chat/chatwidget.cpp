#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif


#include "chatwidget.h"

chatWidget::chatWidget(QWidget *parent)
    : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 显示聊天记录的文本编辑区
    chatDisplay = new QTextEdit(this);
    chatDisplay->setReadOnly(true);

    // 输入消息的行编辑区
    inputLine = new QLineEdit(this);

    // 发送消息的按钮
    sendButton = new QPushButton("发送", this);

    // 垂直布局，用于排列输入行和按钮
    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->addWidget(inputLine);
    inputLayout->addWidget(sendButton);

    mainLayout->addWidget(chatDisplay);
    mainLayout->addLayout(inputLayout);


    // 连接发送按钮点击信号和槽函数
    connect(sendButton, &QPushButton::clicked, this, &chatWidget::sendMessage);
    connect(inputLine, &QLineEdit::returnPressed, this, &chatWidget::sendMessage);
}

void chatWidget::sendMessage() {
    QString message = inputLine->text();
    if (!message.isEmpty()) {
        chatDisplay->append("User: " + message);
        inputLine->clear();

        // 这里可以添加生成回复的代码
        QString response = generateResponse(message);
        chatDisplay->append("ChatGPT: " + response);
    }
}

QString chatWidget::generateResponse(const QString &message) {
    // 这是一个模拟的回复函数，可以根据需要替换成实际的AI生成逻辑
    return "这是一个回复：" + message;
}

