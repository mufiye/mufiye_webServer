#include "http_response.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
http_response::http_response(/* args */)
{
    //this->body = NULL;
    //this->statusCode = Unknown;
    //this->statusMessage = NULL;
    this->response_headers = (response_header *)malloc(sizeof(struct response_header) * INIT_RESPONSE_HEADER_SIZE);
    this->response_headers_number = 0;
    this->keep_connected = 1;

    this->statusCode = OK;
    this->statusMessage = "OK";
    this->contentType = "text/html";
    this->body = "<html><head><title>This is network programming</title></head><body><h1>Hello, network programming</h1></body></html>";
}

http_response::~http_response()
{
    free(this->response_headers);
}

char* http_response::encode_response()
{
    char *ptr = this->buffer;
    char buf[1024];
    int len;
    memset(&buf, 0, sizeof(buf));
    snprintf(buf, sizeof buf, "HTTP/1.1 %d ", this->statusCode);
    len = strlen(buf);
    memcpy(ptr, buf, len);
    ptr += len;

    ptr = assemble_message(ptr, this->statusMessage);
    ptr = assemble_message(ptr, "\r\n");

    if (this->keep_connected)
    {
        ptr = assemble_message(ptr, "Connection: close\r\n");
    }
    else
    {
        memset(&buf, 0, sizeof(buf));
        snprintf(buf, sizeof buf, "Content-Length: %zd\r\n", strlen(this->body));
        len = strlen(buf);
        memcpy(ptr, buf, len);
        ptr += len;

        ptr = assemble_message(ptr, "Connection: Keep-Alive\r\n");
    }

    if (this->response_headers != NULL && this->response_headers_number > 0)
    {
        for (int i = 0; i < this->response_headers_number; i++)
        {
            ptr = assemble_message(ptr, this->response_headers[i].key);
            ptr = assemble_message(ptr, ": ");
            ptr = assemble_message(ptr, this->response_headers[i].value);
            ptr = assemble_message(ptr, "\r\n");
        }
    }

    ptr = assemble_message(ptr, "\r\n");
    ptr = assemble_message(ptr, this->body);
    return this->buffer;
}

char *http_response::assemble_message(char *dst, char *src)
{
    char buf[1024];
    int len;
    memset(&buf, 0, sizeof(buf));
    snprintf(buf, sizeof buf, src);
    len = strlen(src);
    memcpy(dst, buf, len);
    dst += len;
    return dst;
}