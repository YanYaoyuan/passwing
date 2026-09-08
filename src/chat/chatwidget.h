#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QLabel>
#include <QWidget>

class chatWidget : public QWidget
{
    Q_OBJECT
public:
    chatWidget(QWidget *parent = nullptr);
private slots:
    void sendMessage();


private:
    QTextEdit *chatDisplay;
    QLineEdit *inputLine;
    QPushButton *sendButton;
private:
    QString generateResponse(const QString &);

};

#endif // CHATWIDGET_H
