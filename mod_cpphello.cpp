#include "ap_config.h"
#include "apr.h"
#include "apr_lib.h"
#include "apr_strings.h"
#include "apr_network_io.h"
#include "apr_want.h"
#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#include "http_log.h"
#include "http_request.h"
#include "http_protocol.h"
#include <iostream>
#include <string>

extern "C" module AP_MODULE_DECLARE_DATA cpphello_module;

/* Define prototypes of our functions in this module */
static void register_hooks(apr_pool_t *pool);
static int cpphello_handler(request_rec *r);

static int util_read(request_rec *r, const char **rbuf, apr_off_t *size)
{
    /*~~~~~~~~*/
    int rc = OK;
    /*~~~~~~~~*/

    if((rc = ap_setup_client_block(r, REQUEST_CHUNKED_ERROR))) {
        return(rc);
    }

    if(ap_should_client_block(r)) {

        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        char         argsbuffer[HUGE_STRING_LEN];
        apr_off_t    rsize, len_read, rpos = 0;
        apr_off_t length = r->remaining;
        /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

        *rbuf = (const char *) apr_pcalloc(r->pool, (apr_size_t) (length + 1));
        *size = length;
        while((len_read = ap_get_client_block(r, argsbuffer, sizeof(argsbuffer))) > 0) {
            if((rpos + len_read) > length) {
                rsize = length - rpos;
            }
            else {
                rsize = len_read;
            }

            memcpy((char *) *rbuf + rpos, argsbuffer, (size_t) rsize);
            rpos += rsize;
        }
    }
    return(rc);
}

typedef struct {
    char *hellomessage;
} cpphello_dir_config;

static void *cpphello_create_dir_config(apr_pool_t *p, char *path)
{
    cpphello_dir_config *cfg = (cpphello_dir_config *)apr_pcalloc(p, sizeof(cpphello_dir_config));
    cfg->hellomessage = (char *)"{\"text\":\"hello\"}";
    return cfg;
}

static int cpphello_handler(request_rec *r)
{
    /* First off, we need to check if this is a call for the "example-handler" handler.
     * If it is, we accept it and do our things, if not, we simply return DECLINED,
     * and the server will try somewhere else.
     */
    if (!r->handler || strcmp(r->handler, "cpphello")) return (DECLINED);

    apr_off_t size;
    const char* buffer;
    if (util_read(r, &buffer, &size) == OK) {
        ap_rprintf(r, "We read a request body that was %" APR_OFF_T_FMT " bytes long", size);
    }
    /* Now that we are handling this request, we'll write out "Hello, world!" to the client.
     * To do so, we must first set the appropriate content type, followed by our output.
     */

    ap_set_content_type(r, "application/json; charset=utf-8");
    ap_rprintf(r, "{\"text\":\"hello\"}");

    /* Lastly, we must tell the server that we took care of this request and everything went fine.
     * We do so by simply returning the value OK to the server.
     */
    return OK;
}
//   if (strcmp(r->handler, "cpphello")) {
//     return DECLINED;
//   }
//   cpphello_dir_config *cfg = (cpphello_dir_config *) ap_get_module_config(r->per_dir_config, &cpphello_module);
//   std::string messagetosend = std::string("<html><p>") + std::string(cfg->hellomessage) + std::string("</p></html>\n");
//   r->content_type = "text/html";
//   if (!r->header_only) {
//     ap_rputs(messagetosend.c_str(), r);
//   }
//   return OK;
// }

static void register_hooks(apr_pool_t *p)
{
  ap_hook_handler(cpphello_handler,NULL,NULL,APR_HOOK_MIDDLE);
}

extern "C" {
    module AP_MODULE_DECLARE_DATA cpphello_module = {
		STANDARD20_MODULE_STUFF,
        NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		register_hooks
	};
};
