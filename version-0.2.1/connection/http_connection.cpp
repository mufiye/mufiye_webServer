#include "http_connection.h"

#define ISspace(x) isspace((int)(x))
// 一些通用的报文信息
const char *ok_200_title = "OK";
const char *error_400_title = "Bad Request";
const char *error_400_form = "Your request has bad syntax or is inherently impossible to staisfy.\n";
const char *error_403_title = "Forbidden";
const char *error_403_form = "You do not have permission to get file form this server.\n";
const char *error_404_title = "Not Found";
const char *error_404_form = "The requested file was not found on this server.\n";
const char *error_500_title = "Internal Error";
const char *error_500_form = "There was an unusual problem serving the request file.\n";

/* 初始化 */
http_connection::http_connection(/* args */) : tcp_connection()
{
    check_state = CHECK_STATE_REQUESTLINE;
    request_method = GET;
    request_content_length = 0;
    if_linger = false;
    if_cgi = false;
}

http_connection::~http_connection()
{
}

/**
 * 处理请求报文
 */
http_connection::HTTP_CODE http_connection::process_read()
{
#ifdef DEBUG
    printf("pthread: %ld, into to function http_connection::process_read()\n", pthread_self());
    fflush(stdout);
#endif
    LINE_STATUS line_status = LINE_OK;
    HTTP_CODE retcode = NO_REQUEST;
    //char *text = 0;

    while ((check_state == CHECK_STATE_CONTENT && line_status == LINE_OK) || ((line_status = parse_line()) == LINE_OK))
    {
        this->get_line();
        printf("the new line is: %s\n", this->current_line);
        fflush(stdout);
        switch (check_state)
        {
        case CHECK_STATE_REQUESTLINE:
        {
            retcode = parse_request_line(current_line);
            if (retcode == BAD_REQUEST)
                return BAD_REQUEST;
            break;
        }
        case CHECK_STATE_HEADER:
        {
            retcode = parse_headers(current_line);
            if (retcode == BAD_REQUEST)
                return BAD_REQUEST;
            else if (retcode == GET_REQUEST)
            {
                return do_request();
            }
            break;
        }
        case CHECK_STATE_CONTENT:
        {
            retcode = parse_content(current_line);
            if (retcode == GET_REQUEST)
                return do_request();
            line_status = LINE_OPEN;
            break;
        }
        default:
            return INTERNAL_ERROR;
        }
    }
    return NO_REQUEST;
}

void http_connection::get_line()
{
#ifdef DEBUG
    printf("pthread: %ld, into to function get_line()\n", pthread_self());
    fflush(stdout);
#endif
    memset(current_line, 0, sizeof current_line);
    int num = this->input_buffer->get_read_line_char_num();
    char *temp = current_line;
    for (int i = 0; i < num; i++)
    {
        *temp = this->input_buffer->buffer_read_char();
        temp++;
    }
}

http_connection::LINE_STATUS http_connection::parse_line()
{
#ifdef DEBUG
    printf("pthread: %ld, into to function parse_line()\n", pthread_self());
    fflush(stdout);
#endif
    int ret = this->input_buffer->buffer_check_line();
    printf("the parse_line()'s ret is: %d\n",ret);
    fflush(stdout);
    switch (ret)
    {
    case 0:
        return LINE_OK;
    case 1:
        return LINE_BAD;
    case 2:
        return LINE_OPEN;
    }
}

/* 解析http请求报文的请求行 */
http_connection::HTTP_CODE http_connection::parse_request_line(char *text)
{
#ifdef DEBUG
    printf("pthread: %ld, into to function parse_request_line()\n", pthread_self());
    fflush(stdout);
#endif
    /**
     * 获取method
     * 将"制表符\t"替换为"结束符\0"
     * char *strpbrk(const char *str1, const char *str2) 检索字符串str1中第一个匹配字符串str2中字符的字符
     */
    char *temp_request_url = nullptr;
    char *temp_request_version = nullptr;

    temp_request_url = strpbrk(text, " \t");
    if (!temp_request_url)
    {
        return BAD_REQUEST;
    }
    *temp_request_url++ = '\0'; //将"制表符\t"替换为"结束符\0"

    char *method = text;
    printf("the method is: %s\n",method);
    fflush(stdout);
    if (strcasecmp(method, "GET") == 0)
        request_method = GET;
    else if (strcasecmp(method, "POST") == 0)
    {
        request_method = POST;
        if_cgi = true;
    }
    else
        return BAD_REQUEST;

    /**
     * 获取url
     * strspn(const char *str1, const char *str2) 检索字符串str1中第一个不在字符串str2中出现的字符下标。
     */
    temp_request_url += strspn(temp_request_url, " \t");

    /**
     * 获取http版本号
     * strchr(const char *str, int c) 在参数str所指向的字符串中搜索第一次出现字符c（一个无符号字符）的位置
     */
    temp_request_version = strpbrk(temp_request_url, " \t");
    if (!temp_request_version)
        return BAD_REQUEST;
    *temp_request_version++ = '\0';
    temp_request_version += strspn(temp_request_version, " \t");
    if (strcasecmp(temp_request_version, "HTTP/1.1") != 0)
        return BAD_REQUEST;

    /**
     * 进一步处理url
     */
    if (strncasecmp(temp_request_url, "http://", 7) == 0)
    {
        temp_request_url += 7;
        temp_request_url = strchr(temp_request_url, '/');
    }
    if (strncasecmp(temp_request_url, "https://", 8) == 0)
    {
        temp_request_url += 8;
        temp_request_url = strchr(temp_request_url, '/');
    }

    if (!temp_request_url || temp_request_url[0] != '/')
        return BAD_REQUEST;

    request_version = temp_request_version;

    //默认为根目录，index.html
    if (strlen(temp_request_url) == 1)
        strcat(temp_request_url, "index.html");

    check_state = CHECK_STATE_HEADER;
    request_url = temp_request_url;
    return NO_REQUEST;
}

/* 解析http请求报文的头部信息 */
http_connection::HTTP_CODE http_connection::parse_headers(char *text)
{
#ifdef DEBUG
    printf("pthread: %ld, into to function parse_headers()\n", pthread_self());
    fflush(stdout);
#endif
    if (text[0] == '\0')
    {
        printf("into here: text[0] == null\n");
        fflush(stdout);
        if (request_content_length != 0)
        {
            check_state = CHECK_STATE_CONTENT;
            return NO_REQUEST;
        }
        return GET_REQUEST;
    }
    else if (strncasecmp(text, "Connection:", 11) == 0)
    {
        text += 11;
        text += strspn(text, " \t");
        if (strcasecmp(text, "keep-alive") == 0)
        {
            if_linger = true;
        }
    }
    else if (strncasecmp(text, "Content-length:", 15) == 0)
    {
        text += 15;
        text += strspn(text, " \t");
        request_content_length = atol(text);
    }
    else if (strncasecmp(text, "Host:", 5) == 0)
    {
        text += 5;
        text += strspn(text, " \t");
        request_host = text;
    }
    else
    {
        /* 不知道的头信息就让它继续解析 */
        return NO_REQUEST;
    }
    return NO_REQUEST;
}

/* 解析http请求报文的主体数据 */
http_connection::HTTP_CODE http_connection::parse_content(char *text)
{
#ifdef DEBUG
    printf("pthread: %ld, into to function parse_content()\n", pthread_self());
    fflush(stdout);
#endif
    printf("pthread: %ld, into to function parse_content()\n", pthread_self());
    //POST请求的内容在主体数据中
}

/* 根据解析得到的内容处理请求 */
http_connection::HTTP_CODE http_connection::do_request()
{
#ifdef DEBUG
    printf("pthread: %ld, into to function do_request()\n", pthread_self());
    fflush(stdout);
#endif
    if (request_method == GET)
    {
        //解析url分析是否有request string，如果有也要运行cgi程序
        printf("the http url is %s\n", this->request_url.c_str());
        printf("the http version is %s\n", this->request_version.c_str());
        printf("the request content length is: %d\n",this->request_content_length);
        printf("the linger information is: %s\n",this->if_linger?"keep-alive":"close");
        fflush(stdout);
        return FILE_REQUEST;
    }
    else if (request_method == POST)
    {
        //运行cgi程序，POST的数据在content中
    }
}

/**
 * 处理响应报文
 */
bool http_connection::process_write(HTTP_CODE retcode)
{
#ifdef DEBUG
    printf("pthread: %ld, into to function process_write()\n", pthread_self());
    fflush(stdout);
#endif
    switch (retcode)
    {
    case INTERNAL_ERROR:
    {
        append_status_line(500, error_500_title);
        append_headers(strlen(error_500_form));
        if (!append_content(error_500_form))
            return false;
        break;
    }
    case BAD_REQUEST:
    {
        append_status_line(404, error_404_title);
        append_headers(strlen(error_404_form));
        if (!append_content(error_404_form))
            return false;
        break;
    }
    case FORBIDDEN_REQUEST:
    {
        append_status_line(403, error_403_title);
        append_headers(strlen(error_403_form));
        if (!append_content(error_403_form))
            return false;
        break;
    }
    case FILE_REQUEST:
    {
        append_status_line(200, ok_200_title);
        /* 如何将静态html文件发送给客户端？ */

        /* 尝试直接放在字符串常量中发送 */
        const char *ok_string = "<html><head><title>mufiye_server</title></head><body><h1>Hello,this is mufiye server</h1></body></html>";
        append_headers(strlen(ok_string));
        if (!append_content(ok_string))
            return false;
        break;
    }
    case CGI_REQUEST:
    {
        append_status_line(200, ok_200_title);
        break;
    }
    default:
        return false;
    }
    return true;
}

/* 添加状态行 */
bool http_connection::append_status_line(int status, const char *title)
{
#ifdef DEBUG
    printf("pthread: %ld, into to function append_status_line()\n", pthread_self());
    fflush(stdout);
#endif
    char buf[128];
    snprintf(buf, sizeof buf, "%s %d %s\r\n", "HTTP/1.1", status, title);
    this->output_buffer->buffer_append_string(buf);
}

/* 首部信息中的报文类型 */
bool http_connection::append_content_type()
{
#ifdef DEBUG
    printf("pthread: %ld, into to function append_content_type()\n", pthread_self());
    fflush(stdout);
#endif
    char buf[128];
    snprintf(buf, sizeof buf, "Content-Type:%s\r\n", "text/html");
    this->output_buffer->buffer_append_string(buf);
}

/* 首部信息中的报文长度 */
bool http_connection::append_content_length(int content_length)
{
#ifdef DEBUG
    printf("pthread: %ld, into to function append_content_length()\n", pthread_self());
    fflush(stdout);
#endif
    char buf[128];
    snprintf(buf, sizeof buf, "Content-Length:%d\r\n", content_length);
    this->output_buffer->buffer_append_string(buf);
}

/* 首部信息中的连接是否保活 */
bool http_connection::append_linger()
{
#ifdef DEBUG
    printf("pthread: %ld, into to function append_linger()\n", pthread_self());
    fflush(stdout);
#endif
    char buf[128];
    snprintf(buf, sizeof buf, "Connection:%s\r\n", (if_linger == true) ? "keep-alive" : "close");
    this->output_buffer->buffer_append_string(buf);
}

/* 添加空行 */
bool http_connection::append_blank_line()
{
#ifdef DEBUG
    printf("pthread: %ld, into to function append_blank_line()\n", pthread_self());
    fflush(stdout);
#endif
    char buf[] = "\r\n";
    //snprintf(buf, sizeof buf, "%s", );
    this->output_buffer->buffer_append_string(buf);
}

/* 添加报文首部信息 */
bool http_connection::append_headers(int content_length)
{
#ifdef DEBUG
    printf("pthread: %ld, into to function append_headers()\n", pthread_self());
    fflush(stdout);
#endif
    append_content_length(content_length);
    append_linger();
    append_blank_line();
}

/* 添加报文主体内容,函数参数const与不const的区别 */
/* 待优化 */
bool http_connection::append_content(const char *content)
{
#ifdef DEBUG
    printf("pthread: %ld, into to function append_content()\n", pthread_self());
    fflush(stdout);
#endif
    char str[1024];
    memcpy(str, content, strlen(content));
    this->output_buffer->buffer_append_string(str);
    return true;
}

/* http连接处理的入口 */
void http_connection::process()
{
#ifdef DEBUG
    printf("pthread: %ld, into to function process()\n", pthread_self());
    fflush(stdout);
#endif
    /**
     * 处理读取的数据 
     * 使用状态机
     * 1.解析请求行  2.解析报文头部  3.解析报文主体
     * 尽量使用rw_buffer的buffer_read_char()方法或者自己封装其它方法
     */
    HTTP_CODE read_ret = process_read();
    if (read_ret == NO_REQUEST)
    {
        modfd(m_epollFd, connFd, EPOLLIN);
        return;
    }
    /**
     * 准备写入的数据 
     * 分情况处理
     * 1. 无参数GET -> 得到html文件(如何高效地发送html文件?)
     * 2. 有参数GET与POST -> cgi(在子线程中fork()?)
     */
    bool write_ret = process_write(read_ret);
    if (!write_ret)
    {
        printf("process_write() return false, something wrong\n");
    }
    modfd(m_epollFd, connFd, EPOLLOUT);
}