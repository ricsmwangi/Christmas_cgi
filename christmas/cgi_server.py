#!/usr/bin/env python3
"""
Simple CGI Server for Christmas Mini Market
This server properly executes CGI scripts instead of just serving files
"""

import http.server
import socketserver
import subprocess
import os
import sys
from urllib.parse import urlparse, parse_qs
import cgi

class CGIServer(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        """Handle GET requests"""
        parsed_path = urlparse(self.path)
        path = parsed_path.path

        # Check if this is a CGI request
        if path.startswith('/cgi-bin/') and path.endswith('.cgi'):
            self.handle_cgi(path, parsed_path.query)
        else:
            # Serve static files
            self.serve_static_file(path)

    def do_POST(self):
        """Handle POST requests"""
        parsed_path = urlparse(self.path)
        path = parsed_path.path

        if path.startswith('/cgi-bin/') and path.endswith('.cgi'):
            # Read POST data
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length).decode('utf-8')

            self.handle_cgi(path, post_data)
        else:
            self.send_error(404, "File not found")

    def handle_cgi(self, path, query_string):
        """Execute CGI script and return output"""
        script_path = os.path.join(os.getcwd(), path[1:])  # Remove leading /

        if not os.path.exists(script_path):
            self.send_error(404, "CGI script not found")
            return

        if not os.access(script_path, os.X_OK):
            self.send_error(403, "CGI script not executable")
            return

        try:
            # Set up environment variables for CGI
            env = os.environ.copy()
            env['REQUEST_METHOD'] = self.command
            env['QUERY_STRING'] = query_string
            env['SCRIPT_NAME'] = path
            env['SERVER_NAME'] = 'localhost'
            env['SERVER_PORT'] = str(self.server.server_address[1])
            env['REMOTE_ADDR'] = self.client_address[0]
            env['CONTENT_TYPE'] = self.headers.get('Content-Type', '')
            env['CONTENT_LENGTH'] = self.headers.get('Content-Length', '0')

            # Execute CGI script
            result = subprocess.run(
                [script_path],
                input=query_string if self.command == 'POST' else None,
                capture_output=True,
                text=True,
                env=env,
                cwd=os.getcwd()
            )

            # Parse CGI output
            output = result.stdout
            if result.stderr:
                print(f"CGI stderr: {result.stderr}", file=sys.stderr)

            # Split headers and body
            if '\n\n' in output:
                headers_part, body = output.split('\n\n', 1)
            elif '\r\n\r\n' in output:
                headers_part, body = output.split('\r\n\r\n', 1)
            else:
                # No headers, assume HTML
                headers_part = "Content-Type: text/html"
                body = output

            # Send response
            self.send_response(200)

            # Parse and send headers
            for header_line in headers_part.split('\n'):
                if ':' in header_line:
                    header_name, header_value = header_line.split(':', 1)
                    header_name = header_name.strip()
                    header_value = header_value.strip()
                    self.send_header(header_name, header_value)

            self.end_headers()
            self.wfile.write(body.encode('utf-8'))

        except Exception as e:
            self.send_error(500, f"CGI execution failed: {str(e)}")

    def serve_static_file(self, path):
        """Serve static files"""
        if path == '/':
            path = '/index.html'

        file_path = os.path.join(os.getcwd(), path[1:])  # Remove leading /

        if not os.path.exists(file_path) or not os.path.isfile(file_path):
            self.send_error(404, "File not found")
            return

        # Send file
        try:
            with open(file_path, 'rb') as f:
                content = f.read()

            self.send_response(200)
            # Set content type based on file extension
            if path.endswith('.html'):
                self.send_header('Content-Type', 'text/html')
            elif path.endswith('.css'):
                self.send_header('Content-Type', 'text/css')
            elif path.endswith('.js'):
                self.send_header('Content-Type', 'application/javascript')
            else:
                self.send_header('Content-Type', 'text/plain')

            self.end_headers()
            self.wfile.write(content)

        except Exception as e:
            self.send_error(500, f"Error serving file: {str(e)}")

    def log_message(self, format, *args):
        """Override to reduce log noise"""
        pass

def run_server(port=8080):
    """Run the CGI server"""
    with socketserver.TCPServer(("", port), CGIServer) as httpd:
        print(f"🎄 Christmas Mini Market CGI Server running on port {port}")
        print(f"🌐 Access at: http://localhost:{port}/cgi-bin/main.cgi")
        print("Press Ctrl+C to stop")
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\n🛑 Server stopped")

if __name__ == "__main__":
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 8080
    run_server(port)