/*
 * Copyright (C) 2011 Jonathan Leibiusky
 * Work sponsored by MercadoLibre.com
 *
 * Based on nginx source (C) Igor Sysoev
 */

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

static ngx_int_t ngx_http_upstream_init_idempotent(ngx_conf_t *cf,
    ngx_http_upstream_srv_conf_t *us);
static ngx_int_t ngx_http_upstream_init_idempotent_peer(ngx_http_request_t *r,
    ngx_http_upstream_srv_conf_t *us);
static char *ngx_http_upstream_idempotent(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static ngx_int_t ngx_http_upstream_idempotent_init_module(ngx_cycle_t *cycle);

void
ngx_http_upstream_free_idempotent_peer(ngx_peer_connection_t *pc, void *data,
    ngx_uint_t state);

typedef struct {
    /* the round robin data must be first */
    ngx_http_upstream_rr_peer_data_t   rrp;
    ngx_uint_t                          method;
} ngx_http_upstream_idempotent_peer_data_t;

static ngx_command_t ngx_http_upstream_idempotent_commands[] = {
    { ngx_string("only_retry_idempotent"),
      NGX_HTTP_UPS_CONF|NGX_CONF_NOARGS|NGX_ANY_CONF,
      ngx_http_upstream_idempotent,
      0,
      0,
      NULL },

      ngx_null_command
};

static ngx_http_module_t  ngx_http_upstream_idempotent_module_ctx = {
    NULL,                                  /* preconfiguration */
    NULL,                                  /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    NULL,                                  /* create location configuration */
    NULL,                                  /* merge location configuration */
};


ngx_module_t  ngx_http_upstream_idempotent_module = {
    NGX_MODULE_V1,
    &ngx_http_upstream_idempotent_module_ctx, /* module context */
    ngx_http_upstream_idempotent_commands,    /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    ngx_http_upstream_idempotent_init_module,    /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};

static ngx_int_t
ngx_http_upstream_idempotent_init_module(ngx_cycle_t *cycle)
{
    return NGX_OK;
}

static char *
ngx_http_upstream_idempotent(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_upstream_srv_conf_t  *uscf;

    uscf = ngx_http_conf_get_module_srv_conf(cf, ngx_http_upstream_module);

    uscf->peer.init_upstream = ngx_http_upstream_init_idempotent;

    uscf->flags = NGX_HTTP_UPSTREAM_CREATE
                  |NGX_HTTP_UPSTREAM_MAX_FAILS
                  |NGX_HTTP_UPSTREAM_FAIL_TIMEOUT
                  |NGX_HTTP_UPSTREAM_DOWN;

    return NGX_CONF_OK;
}

static ngx_int_t
ngx_http_upstream_init_idempotent(ngx_conf_t *cf, ngx_http_upstream_srv_conf_t *us)
{
    ngx_int_t resp = ngx_http_upstream_init_round_robin(cf, us);
    
    us->peer.init = ngx_http_upstream_init_idempotent_peer;

    return resp;
}


ngx_int_t
ngx_http_upstream_init_idempotent_peer(ngx_http_request_t *r,
    ngx_http_upstream_srv_conf_t *us)
{
    ngx_http_upstream_idempotent_peer_data_t  *iphp;

    iphp = ngx_palloc(r->pool, sizeof(ngx_http_upstream_idempotent_peer_data_t));
    if (iphp == NULL) {
        return NGX_ERROR;
    }

    iphp->method = r->method;
    r->upstream->peer.data = &iphp->rrp;

    if (ngx_http_upstream_init_round_robin_peer(r, us) != NGX_OK) {
        return NGX_ERROR;
    }

    r->upstream->peer.free = ngx_http_upstream_free_idempotent_peer;

    return NGX_OK;
}

void
ngx_http_upstream_free_idempotent_peer(ngx_peer_connection_t *pc, void *data,
    ngx_uint_t state)
{
    ngx_http_upstream_free_round_robin_peer(pc, data, state);

    ngx_http_upstream_idempotent_peer_data_t  *rrp = data;

    if (rrp->method == NGX_HTTP_POST) {
        pc->tries = 0;
    }
}

/* vim: set et ts=4 sw=4: */
