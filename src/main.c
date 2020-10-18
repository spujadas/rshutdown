#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "http.h" /* the HTTP facil.io extension */
#include "log.h" /* log.c logging library*/

/* function prototypes */
void on_request(http_s *request);
int request_contains_shutdown_token(http_s *request);


/* listen to HTTP requests and start facil.io */
int main(void) {
  char default_http_port[] = "3000";
  char *http_port = default_http_port;
  long int http_port_long_int;

  /* check that RSHUTDOWN_TOKEN env var is set */
  if (getenv("RSHUTDOWN_TOKEN") == NULL || getenv("RSHUTDOWN_TOKEN")[0] == '\0') {
    log_error("RSHUTDOWN_TOKEN environment variable not set - exiting");
    return 1;
  }

  /* debug mode if compiled with NO_SHUTDOWN: display a message but don't shutdown */
#ifdef NO_SHUTDOWN
  log_info("NO_SHUTDOWN set - won't shutdown\n");
#endif

  /* use default port unless RSHUTDOWN_HTTP_PORT is set to another port */
  if (getenv("RSHUTDOWN_HTTP_PORT") == NULL
    || getenv("RSHUTDOWN_HTTP_PORT")[0] == '\0'
  ) {
    log_debug("RSHUTDOWN_HTTP_PORT environment variable not set - using default port");
  }
  else {
    http_port = getenv("RSHUTDOWN_HTTP_PORT");

    /* check if port is a valid number by attempting to convert it to a long int */
    if (sscanf(http_port, "%ld", &http_port_long_int) != 1) {
        log_error("RSHUTDOWN_HTTP_PORT set to invalid port number - exiting");
        return 1;
    }

    log_debug("RSHUTDOWN_HTTP_PORT environment variable set");
  }

  /* listen on any available network binding (NULL == 0.0.0.0) */
  log_info("Listening on port %s", http_port);
  http_listen(http_port, NULL, .on_request = on_request, .log = 1);

  /* start the server */
  fio_start(.threads = 1);
}


/* use this callback in http_listen() to handle HTTP requests */
void on_request(http_s *request) {
  /* honour non-POST requests with HTTP 200 */
  if (strcmp("POST", fiobj_obj2cstr(request->method).data)) {
    http_send_body(request, "{\"status\": 1}\r\n", 15);
    return;
  }

  /* shutdown only if body is JSON object {"token": "<RSHUTDOWN_TOKEN_ENV_VAR>"} */
  if (request_contains_shutdown_token(request)) {
      http_send_body(request, "{\"status\": 0}\r\n", 15);
#ifdef NO_SHUTDOWN
      log_info("NO_SHUTDOWN set - not shutting down");
#else
      log_info("Shutting down!");
      system("sudo shutdown -h now");
#endif
  }
  else {
      http_send_body(request, "{\"status\": -1}\r\n", 16);
  }
}


/* check if HTTP request contains valid shutdown token */
int request_contains_shutdown_token(http_s *request) {
  int contains_shutdown_token = 0;

  if (http_parse_body(request) != 0) {
    return 0;
  }

  FIOBJ key = fiobj_str_new("token", 5);
  if (
    FIOBJ_TYPE_IS(request->params, FIOBJ_T_HASH)  /* check if JSON object is a hash, */
    && fiobj_hash_get(request->params, key)  /* test for existence of "token", */
    && !strcmp(fiobj_obj2cstr(fiobj_hash_get(request->params, key)).data,
        getenv("RSHUTDOWN_TOKEN"))  /* and test if token matches RSHUTDOWN_TOKEN */
  ) {
    contains_shutdown_token = 1;
  }
  fiobj_free(key);

  return contains_shutdown_token;
}
