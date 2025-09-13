#include "logger.h"



Logger *Logger::getInstance()
{
    static Logger *instance = new Logger();
    return instance;
}

void Logger::logMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Logger::getInstance()->handleLogMessage(type, context, msg);
}

Logger::Logger(QObject *parent)
    : QObject{parent}
{
    // 确保日志目录存在
    QString logDirPath = QApplication::applicationDirPath() + "/.shitonline/log/";
    QDir().mkpath(logDirPath);
}

void Logger::start()
{
    if (logFile.isOpen()) {
        logFile.close();
    }

    //将日志重定向到logMessageHandler函数
    qInstallMessageHandler(Logger::logMessageHandler);

    //日志文件名
    QString logFilePath = QApplication::applicationDirPath() + "/.shitonline/log/" + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".log";

    //设置日志文件目录
    logFile.setFileName(logFilePath);

    if(!logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)){
        qWarning() << "打开日志文件失败!";
    }
    else{
        logStream.setDevice(&logFile);
        qDebug() << "成功打开日志文件";
    }
}

void Logger::stop()
{
    //关闭日志文件
    if(logFile.isOpen()){
        logFile.close();
    }

    //卸载自定义消息函数
    qInstallMessageHandler(nullptr);
}

void Logger::handleLogMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QMutexLocker locker(&mutex); //使用互斥锁确保线程安全

    //获取日志类型
    QString logType;
    switch (type) {
    case QtDebugMsg:
        logType = "DEBUG";
        break;
    case QtInfoMsg:
        logType = "INFO";
        break;
    case QtWarningMsg:
        logType = "WARNING";
        break;
    case QtCriticalMsg:
        logType = "CRITICAL";
        break;
    case QtFatalMsg:
        logType = "FATAL";
        break;
    default:
        logType = "UNKNOWN";
        break;
    }

    //格式化日志信息
    QString message = QString("[%1] [%2]: %3").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(logType).arg(msg);

    //写入日志文件
    logStream << message << Qt::endl;
    logStream.flush();

    if(type == QtFatalMsg){
        abort();
    }
}
