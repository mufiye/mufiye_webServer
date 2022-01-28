#ifndef HTTP_CONNECTION_H
#define HTTP_CONNECTION_H

#include "tcp_connection.h"

class http_connection : public tcp_connection
{
private:
    enum HTTP_CODE
    {
        NO_REQUEST,
        GET_REQUEST,
        FILE_REQUEST, //发送静态html文件
        CGI_REQUEST,  //运行cgi程序，发送结果
        BAD_REQUEST,
        FORBIDDEN_REQUEST,
        INTERNAL_ERROR
    };
    enum CHECK_STATE
    {
        CHECK_STATE_REQUESTLINE = 0,
        CHECK_STATE_HEADER,
        CHECK_STATE_CONTENT
    };
    enum LINE_STATUS
    {
        LINE_OK = 0,
        LINE_BAD,
        LINE_OPEN
    };
    enum METHOD
    {
        GET = 0,
        POST
    };

private:
    CHECK_STATE check_state;
    METHOD request_method;
    int request_content_length;
    std::string request_version; //请求报文的请求行中的版本信息
    std::string request_url;    //请求报文的请求行中的url
    std::string query_string;   //请求报文中的请求字符串
    std::string request_host;   //请求报文中的主机信息
    std::string request_content; //请求报文的主体部分
    char current_line[1500];
    bool if_linger;
    bool if_cgi; //是否要运行cgi程序

private:
    /* 关于处理请求报文 */
    HTTP_CODE process_read();
    void get_line();
    LINE_STATUS parse_line();
    HTTP_CODE parse_request_line(char *text);
    HTTP_CODE parse_headers(char *text);
    HTTP_CODE parse_content(char *text);
    HTTP_CODE do_request();

    /* 关于修改响应报文 */
    bool process_write(HTTP_CODE ret);
    bool append_status_line(int status,const char *title);
    bool append_content_length(int content_length);
    bool append_linger();
    bool append_content_type();
    bool append_blank_line();
    bool append_headers(int content_length);
    bool append_content(const char *content);
    
public:
    http_connection(/* args */);
    ~http_connection();
    void process();
};

#endif