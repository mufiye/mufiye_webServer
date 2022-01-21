/**
 * 每个字段的长度如何控制？（给每个char数组长度定义多少）
 */
#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

const int INIT_RESPONSE_HEADER_SIZE = 128;

enum HttpStatusCode {
    Unknown,
    OK = 200,
    MovedPermanently = 301,
    BadRequest = 400,
    NotFound = 404,
};

struct response_header {
    char *key;
    char *value;
};

class http_response
{
private:
    /* data */
    char buffer[2048]; //目前假定的测试用的bufffer,后期要进行封装
    enum HttpStatusCode statusCode; //报文的状态
    char *statusMessage; //状态提示信息
    char *contentType; //报文的类型
    char *body; //报文主体
    struct response_header *response_headers; //报文首部
    int response_headers_number; //报文头的数量
    int keep_connected;
    char *assemble_message(char *dst, char *src);
    
public:
    http_response(/* args */);
    ~http_response();
    char* encode_response();
};

#endif

