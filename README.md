## HTTP server to host my [Where in the World](https://github.com/ridwanalmahmud/Where-in-the-World) project

### Key features
- Request & response parsing
- Serves static files listed in the ./static directory
- Supports html, css, js & images
- CORS Support `Access-Control-Allow-Origin: *` headers for api requests
- Prevents directory traversal attacks
- Process isolation with `fork()`
- Chunked file transfer with 8KB buffer
- Connection reuse

BTW, the browser does not recognize the ssl cert as it's a self signed cert           
so it will show an error. But it's not actually a bug in the code or the browser.
For production use we need to implement certs that the browser recognizes.

### Usage
```sh
# generate certificate
openssl req -new -x509 -newkey rsa:2048 -nodes -keyout private.key -out certificate.pem -days 365
```

```sh
./bin/server certificate.pem private.key
```
