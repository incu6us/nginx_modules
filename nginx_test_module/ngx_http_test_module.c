//
// Created by incu6us on 5/10/16.
//

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


static char *ngx_http_test(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_test_handler(ngx_http_request_t *r);

static u_char TEST[] = "TEST";

struct ngx_command_t {
    ngx_str_t             name;
    ngx_uint_t            type;
    char               *(*set)(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
    ngx_uint_t            conf;
    ngx_uint_t            offset;
    void                 *post;
};

/* Module params */
static ngx_command_t  ngx_http_test_commands[] = {
        { ngx_string("test_command"),
          NGX_HTTP_SRV_CONF_OFFSET|NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
          ngx_http_test,
          NGX_HTTP_LOC_CONF_OFFSET,
          0,
          NULL },
        ngx_null_command
};

/* The module context. */
static ngx_http_module_t ngx_http_test_module_ctx = {
        NULL, /* preconfiguration */
        NULL, /* postconfiguration */

        NULL, /* create main configuration */
        NULL, /* init main configuration */

        NULL, //        ngx_http_test_create_srv_conf, /* create server configuration */
        NULL, //        ngx_http_test_merge_srv_conf, /* merge server configuration */

        NULL, //        ngx_http_test_create_loc_conf, /* create location configuration */
        NULL  //        ngx_http_test_gif_merge_loc_conf /* merge location configuration */
};

/* Module definition. */
ngx_module_t ngx_http_test_module = {
        NGX_MODULE_V1,
        &ngx_http_test_module_ctx, /* module context */
        ngx_http_test_commands, /* module directives */
        NGX_HTTP_MODULE, /* module type */
        NULL, /* init master */
        NULL, /* init module */
        NULL, /* init process */
        NULL, /* init thread */
        NULL, /* exit thread */
        NULL, /* exit process */
        NULL, /* exit master */
        NGX_MODULE_V1_PADDING
};

/* Handler */
static ngx_int_t ngx_http_test_handler(ngx_http_request_t *r) {

    // sending a header
    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = sizeof(TEST);
    r->headers_out.content_type.len = sizeof("text/plain") - 1;
    r->headers_out.content_type.data = (u_char *) "text/plain";
    ngx_http_send_header(r);

    // sending a body
    ngx_buf_t    *b;
    ngx_chain_t   out;

    b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
    if (b == NULL) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                      "Failed to allocate response buffer.");
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    out.buf = b;
    out.next = NULL;

    b->pos = TEST; /* first position in memory of the data */
    b->last = TEST + sizeof(TEST); /* last position */

    b->memory = 1; /* content is in read-only memory */
    /* (i.e., filters should copy it rather than rewrite in place) */

    b->last_buf = 1; /* there will be no more buffers in the request */

    return ngx_http_output_filter(r, &out);

}

/* Installation block */
static char *ngx_http_test(ngx_conf_t *cf, ngx_command_t *cmd, void *conf) {
    ngx_http_core_loc_conf_t  *clcf;

    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_test_handler;

    return NGX_CONF_OK;
}