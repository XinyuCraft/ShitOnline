#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QApplication>

class Logger : public QObject
{
    Q_OBJECT
public:
    static Logger *getInstance();

    //日志信息抓取函数
    static void logMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    //启动函数
    void start();

    //关闭函数
    void stop();

signals:

private:
    explicit Logger(QObject *parent = nullptr);

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    //日志信息处理函数
    void handleLogMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    //日志文件
    QFile logFile;
    QTextStream logStream;
    QMutex mutex;
};

#endif // LOGGER_H
