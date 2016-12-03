# Nginx Upstream Idempotent

## Description

The Nginx idempotent module will avoid retying with POST requests.

## Note
This issue is fixed on Nginx 1.9.13.
This module should not more requird for futur version.

## Installation

You'll need to re-compile Nginx from source to include this module.
Modify your compile of Nginx by adding the following directive
(modified to suit your path of course):

```bash
./configure --with-http_ssl_module --add-module=/absolute/path/to/nginx-upstream-idempotent
make
make install
```

## Usage

Change your Nginx config file's upstream block to include the 'fair' directive:

```nginx
upstream something {
    only_retry_idempotent;

    server 127.0.0.1:5000;
    server 127.0.0.1:5001;
    server 127.0.0.1:5002;
}
```

If you encounter any issues, please report them using the bugtracker

## Contributing

Please feel free to fork the project at GitHub and submit pull requests or patches.

