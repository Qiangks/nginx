#include <ngx_core.h>  
#include <ngx_http.h>  
#include <ngx_config.h>  
  
static char *ngx_http_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);  
static ngx_int_t ngx_http_mytest_handler(ngx_http_request_t *r);  
  
/* 配置指令 */  
static ngx_command_t ngx_http_mytest_commands[] = {  
        {  
             ngx_string("mytest"),  
             NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LMT_CONF|NGX_CONF_NOARGS,  
             ngx_http_mytest,  
             NGX_HTTP_LOC_CONF_OFFSET,  
             0,  
             NULL },  
  
        ngx_null_command  
};  
  
/* 模块上下文 */  
static ngx_http_module_t ngx_http_mytest_module_ctx = {  
        NULL,  
        NULL,  
  
        NULL,  
        NULL,  
  
        NULL,  
        NULL,  
  
        NULL,  
        NULL  
};  
  
/* 模块定义 */  
ngx_module_t ngx_http_mytest_module = {  
        NGX_MODULE_V1,  
        &ngx_http_mytest_module_ctx,  
        ngx_http_mytest_commands,  
        NGX_HTTP_MODULE,  
        NULL,  
        NULL,  
        NULL,  
        NULL,  
        NULL,  
        NULL,  
        NULL,  
        NGX_MODULE_V1_PADDING  
};  
  
/* 挂载handler函数 */  
static char *ngx_http_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)  
{  
        ngx_http_core_loc_conf_t *clcf;  
        clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);  
        clcf->handler = ngx_http_mytest_handler;  
  
        return NGX_CONF_OK;  
}  
  
/* handler函数 */  
static ngx_int_t ngx_http_mytest_handler(ngx_http_request_t *r)  
{  
        //必须是GET或HEAD方法  
        if (!(r->method & (NGX_HTTP_GET|NGX_HTTP_HEAD)))  
            return NGX_HTTP_NOT_ALLOWED;  
  
        //丢弃请求包体  
        ngx_int_t rc = ngx_http_discard_request_body(r);  
        if (rc != NGX_OK) {  
            return rc;  
        }  
  
        ngx_str_t type = ngx_string("text/plain");//响应的Content-Type  
        ngx_str_t response = ngx_string("Hello World !");//响应的包体  
  
        r->headers_out.status = NGX_HTTP_OK;//响应的状态码  
        r->headers_out.content_length_n = response.len;//包体的长度  
        r->headers_out.content_type = type;//响应的Content-Type  
  
        rc = ngx_http_send_header(r);//发送响应头部  
        if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {  
            return rc;  
        }  
  
        ngx_buf_t *b;//构造ngx_buf_t结构，准备发送包体  
        b = ngx_create_temp_buf(r->pool, response.len);  
        if (b == NULL) {  
            return NGX_HTTP_INTERNAL_SERVER_ERROR;  
        }  
  
        ngx_memcpy(b->pos, response.data, response.len);//将"Hello World !"拷贝到ngx_buf_t中  
        b->last = b->pos + response.len;//设置last指针  
        b->last_buf = 1;//声明是最后一块缓冲区  
  
        ngx_chain_t out;//构造发送的ngx_chain_t结构  
        out.buf = b;  
        out.next = NULL;  
  
        return ngx_http_output_filter(r, &out);//发送包体  
}  
