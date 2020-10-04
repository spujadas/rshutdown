#include <stdlib.h>
#include <string.h>

#include "http.h" /* the HTTP facil.io extension */

// We'll use this callback in `http_listen`, to handles HTTP requests
void on_request(http_s *request);
int request_contains_shutdown_token(http_s *request);

// Listen to HTTP requests and start facil.io
int main(void) {
  if (getenv("SHUTDOWN_TOKEN") == NULL) {
    printf("! SHUTDOWN_TOKEN environment variable not set - exiting\n");
    exit(1);
  }

#ifdef NO_SHUTDOWN
  printf("NO_SHUTDOWN set - won't shutdown\n");
#endif

  // listen on port 3000 and any available network binding (NULL == 0.0.0.0)
  http_listen("3000", NULL, .on_request = on_request, .log = 1);

  // start the server
  fio_start(.threads = 1);
}

// Easy HTTP handling
void on_request(http_s *request) {
  /* honour non-POST requests with HTTP 200 */
  if (strcmp("POST", fiobj_obj2cstr(request->method).data)) {
    http_send_body(request, "pong\r\n", 6);
    return;
  }

  /* shutdown only if body is JSON object {"token": "<SHUTDOWN_TOKEN_ENV_VAR>"} */
  if (request_contains_shutdown_token(request)) {
      http_send_body(request, "{\"status\": 0}\r\n", 15);
#ifdef NO_SHUTDOWN
      printf("NO_SHUTDOWN set - not shutting down\n");
#else
      printf("Shutting down!\n");
      system("sudo shutdown -h now");
#endif
  }
  else {
      http_send_body(request, "{\"status\": 1}\r\n", 15);
  }
}

int request_contains_shutdown_token(http_s *request) {
  int contains_shutdown_token = 0;

  if (http_parse_body(request) != 0) {
    return 0;
  }

  FIOBJ key = fiobj_str_new("token", 5);
  if (FIOBJ_TYPE_IS(request->params, FIOBJ_T_HASH)  // check if JSON object is a hash
      && fiobj_hash_get(request->params, key)  // and test for existence of "token"
      && !strcmp(fiobj_obj2cstr(fiobj_hash_get(request->params, key)).data,
          getenv("SHUTDOWN_TOKEN"))  // and test if token matches SHUTDOWN_TOKEN
      ) {
    contains_shutdown_token = 1;
  }
  fiobj_free(key);

  return contains_shutdown_token;
}
