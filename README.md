# Installation
## building:
  ```
./configure --prefix=/path/to/nginx --add-dynamic-module=/path/to/nginx_test_module
make install
  ```
## configuration example:
  ```
...
          location ~test {
              test_command;
          }
...
  ```
