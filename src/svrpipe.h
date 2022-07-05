#ifndef SVRPIPE_H
#define SVRPIPE_H

#include <vector>
#include <string>
#include <mutex>
#include <string_view>

struct RequestHeader {
    char req[64];
    size_t content_length;
};

struct Request {
    RequestHeader header;
    void *content;
};

/**
 * @brief 管道通信服务服务端
 * @author zymelaii
 * @date 2022-07-03
 */
class PipeServer {
public:
	PipeServer();
	~PipeServer();

    /**
     * @brief 初始化服务端
     * @note init完成管道的命名，所有的操作应该在init之后
     *  且init应被调用且仅调用一次
     * @author zymelaii
     * @date 2022-07-03
     */
    virtual PipeServer& init(const char *pipename);

    /**
     * @brief 运行服务端
     * @author zymelaii
     * @date 2022-07-03
     */
    void run();

    /**
     * @brief 关闭服务端
     * @author zymelaii
     * @date 2022-07-03
     */
    void shutdown() const;

    /**
     * @brief 请求路由
     * @author zymelaii
     * @date 2022-07-03
     */
    virtual void route(const Request &request) = 0;
private:
    const char *pipe_;
    mutable bool should_exit_;
};

/**
 * @brief 管道通信服务服务端
 * @author zymelaii
 * @date 2022-07-03
 */
class PipeClient {
public:
    ~PipeClient();
    bool connect(const char *pipename);
    bool disconnect();
    bool request(const char *req, const void *content, size_t size);
    bool request(const char *req, const std::string &content);
private:
    void *instance_;
};

#endif /*SVRPIPE_H*/