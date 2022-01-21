/**
 * 每个字段的长度如何控制？（给每个char数组长度定义多少）
 */
class http_request
{
private:
    /* data */
    //可能用enum类型声明更好？
    char message[1024];
    int method;
    int version;
    char url[1024];
    char entity_body[1024];

public:
    http_request(/* args */);
    ~http_request();
};

http_request::http_request(/* args */)
{
}

http_request::~http_request()
{
}
