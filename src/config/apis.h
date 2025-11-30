#ifndef APIS_H
#define APIS_H
//路由表
namespace API {
    namespace PROVIDER {
        static const int BASE = 100;
        static const int SETP = BASE+1;  //设置数据源参数
        static const int STARTP =BASE+2;  //启动连接
        static const int CLOSEP = BASE+3; //断开连接
    }
    namespace RECORDER {
        static const int BASE = 200;
        static const int SET = BASE+1;
        static const int START = BASE+2;
        static const int STOPR =BASE+3;
    }

}
    // //设置当前数据源
    // void setProvider(const QJsonValue &params);
    // void startProvider();
    // //断开数据源
    // void closeProvider();

    // //读取项目
    // void setReadProject(const QJsonValue &params);
    // //发送项目文件记录
    // void getProject();

    // //记录开关
    // void setRecorder(const QJsonValue &params);
    // //停止记录
    // void stopRecorder();


#endif // APIS_H
