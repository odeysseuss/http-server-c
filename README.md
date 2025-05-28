### HTTP server to host my [Where in the World](https://github.com/ridwanalmahmud/Where-in-the-World) project

Key features
- Request & response parsing
- Serves static files listed in the ./static directory
- Supports html, css, js & images
- CORS Support `Access-Control-Allow-Origin: *` headers for api requests
- Prevents directory traversal attacks
- Process isolation with `fork()`
- Chunked file transfer with 8KB buffer
- Connection reuse
