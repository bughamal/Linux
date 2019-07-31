#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <Header.h>

//struct FILE;
//ʹ�ú�Ϳɱ�����������־��Ϣ������ʱֻҪճ���꼴��  LogInfo("This is test");
#define LogInfo(...) Logger::GetInstance().AddToQueue("INFO", __FILE__, __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__)
#define LogWarning(...) Logger::GetInstance().AddToQueue("WARNING", __FILE__, __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__)
#define LogError(...) Logger::GetInstance().AddToQueue("ERROR", __FILE__, __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__)

class Logger
{
public:
    static Logger& GetInstance();

    void SetFileName(const char* filename);
    bool Start();    //�����̣߳���ʼд��־
    void Stop();
	//ͨ������pszLevel���ж���ʲô�������־
    void AddToQueue(const char* pszLevel, const char* pszFile, int lineNo, const char* pszFuncSig, char* pszFmt, ...);

private:
    Logger() = default;
    Logger(const Logger& rhs) = delete;
    Logger& operator =(Logger& rhs) = delete;

    void threadfunc();


private:
    std::string                     filename_;
    FILE*                           fp_{};
    std::shared_ptr<std::thread>    spthread_;
    std::mutex                      mutex_;
    std::condition_variable         cv_;            //���µ���־�����ı�ʶ
    bool                            exit_{false};
    std::list<std::string>          queue_;
};
void my_error(char *);
#endif //!__LOGGER_H__
