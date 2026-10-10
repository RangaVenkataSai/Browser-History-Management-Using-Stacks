"""
=============================================================================
Browser History Management Using Stacks - REST API Bridge
=============================================================================
Serves the web frontend and exposes REST API endpoints that execute the
underlying C DSA implementation (browser.c and stack.c) via ctypes.

Endpoints:
  GET  /api/state    -> Returns current browser state & stack elements
  POST /api/visit    -> Visits new URL (JSON body: {"url": "..."})
  POST /api/back     -> Navigates back (pops Back stack, pushes to Forward)
  POST /api/forward  -> Navigates forward (pops Forward stack, pushes to Back)
  POST /api/clear    -> Clears both stacks and current page
  GET  /             -> Serves web frontend (web/index.html)
=============================================================================
"""

import os
import sys
import json
import ctypes
import subprocess
from http.server import HTTPServer, SimpleHTTPRequestHandler

# Define C structures matching stack.h and browser.h
MAX_SIZE = 100
MAX_URL_LEN = 256

class CStack(ctypes.Structure):
    _fields_ = [
        ("items", (ctypes.c_char * MAX_URL_LEN) * MAX_SIZE),
        ("top", ctypes.c_int)
    ]

class CBrowser(ctypes.Structure):
    _fields_ = [
        ("backStack", CStack),
        ("forwardStack", CStack),
        ("currentPage", ctypes.c_char * MAX_URL_LEN),
        ("hasCurrentPage", ctypes.c_int)
    ]

# Paths
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
WEB_DIR = os.path.join(BASE_DIR, "web")
DLL_PATH = os.path.join(BASE_DIR, "browser_lib.dll")

def compile_dll_if_needed():
    """Compiles browser.c and stack.c into browser_lib.dll if missing or outdated."""
    c_files = [os.path.join(BASE_DIR, "browser.c"), os.path.join(BASE_DIR, "stack.c")]
    needs_build = not os.path.exists(DLL_PATH)
    if not needs_build:
        dll_mtime = os.path.getmtime(DLL_PATH)
        if any(os.path.getmtime(f) > dll_mtime for f in c_files if os.path.exists(f)):
            needs_build = True

    if needs_build:
        print("[Build] Compiling C core engine into shared library (browser_lib.dll)...")
        env = os.environ.copy()
        if "C:\\msys64\\ucrt64\\bin" not in env.get("PATH", ""):
            env["PATH"] = "C:\\msys64\\ucrt64\\bin;" + env.get("PATH", "")
        cmd = ["gcc", "-shared", "-fPIC", "-o", DLL_PATH, "browser.c", "stack.c"]
        subprocess.check_call(cmd, cwd=BASE_DIR, env=env)
        print("[Build] browser_lib.dll compiled successfully!")

# Ensure DLL and load functions
compile_dll_if_needed()
c_lib = ctypes.CDLL(DLL_PATH)

c_lib.initBrowser.argtypes = [ctypes.POINTER(CBrowser)]
c_lib.initBrowser.restype = None

c_lib.visitPage.argtypes = [ctypes.POINTER(CBrowser), ctypes.c_char_p]
c_lib.visitPage.restype = None

c_lib.goBack.argtypes = [ctypes.POINTER(CBrowser)]
c_lib.goBack.restype = None

c_lib.goForward.argtypes = [ctypes.POINTER(CBrowser)]
c_lib.goForward.restype = None

c_lib.clearHistory.argtypes = [ctypes.POINTER(CBrowser)]
c_lib.clearHistory.restype = None

# Global Browser state in memory
browser_instance = CBrowser()
c_lib.initBrowser(ctypes.byref(browser_instance))

def decode_c_string(val):
    """Safely decodes ctypes c_char array or bytes to Python str."""
    if hasattr(val, "value"):
        val = val.value
    if isinstance(val, (bytes, bytearray)):
        return val.decode("utf-8", errors="replace")
    return str(val)

def get_stack_items(c_stack):
    """Extracts URL strings from a CStack structure from index 0 to top."""
    items = []
    top = c_stack.top
    for i in range(top + 1):
        items.append(decode_c_string(c_stack.items[i]))
    return items

def get_current_state():
    """Returns the full browser state as a dictionary."""
    has_page = bool(browser_instance.hasCurrentPage)
    current_page = (
        decode_c_string(browser_instance.currentPage)
        if has_page
        else None
    )
    back_items = get_stack_items(browser_instance.backStack)
    forward_items = get_stack_items(browser_instance.forwardStack)

    return {
        "hasCurrentPage": has_page,
        "currentPage": current_page,
        "backStack": back_items,
        "forwardStack": forward_items,
        "backCount": len(back_items),
        "forwardCount": len(forward_items),
        "maxSize": MAX_SIZE
    }

class BrowserRequestHandler(SimpleHTTPRequestHandler):
    """Custom HTTP handler serving web files and handling REST endpoints."""

    def __init__(self, *args, **kwargs):
        # Serve static files from the 'web' subdirectory
        super().__init__(*args, directory=WEB_DIR, **kwargs)

    def _send_json(self, data, status=200):
        body = json.dumps(data).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", str(len(body)))
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.end_headers()
        self.wfile.write(body)

    def do_OPTIONS(self):
        self.send_response(200)
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.end_headers()

    def do_GET(self):
        if self.path == "/" or self.path == "/index.html":
            return super().do_GET()

        if self.path == "/api/state":
            state = get_current_state()
            state["message"] = "Current state retrieved"
            return self._send_json(state)

        # Fallback to serving static files from web/
        return super().do_GET()

    def do_POST(self):
        content_length = int(self.headers.get("Content-Length", 0))
        body = self.rfile.read(content_length) if content_length > 0 else b""
        payload = {}
        if body:
            try:
                payload = json.loads(body.decode("utf-8"))
            except Exception:
                payload = {}

        if self.path == "/api/visit":
            url = str(payload.get("url", "")).strip()
            if not url:
                return self._send_json({"error": "URL cannot be empty"}, status=400)

            c_lib.visitPage(ctypes.byref(browser_instance), url.encode("utf-8"))
            state = get_current_state()
            state["lastAction"] = "visit"
            state["message"] = f"Successfully visited {url}"
            return self._send_json(state)

        elif self.path == "/api/back":
            if browser_instance.backStack.top < 0:
                return self._send_json({"error": "Back stack is empty", **get_current_state()}, status=400)

            c_lib.goBack(ctypes.byref(browser_instance))
            state = get_current_state()
            state["lastAction"] = "back"
            state["message"] = f"Went back to {state['currentPage']}"
            return self._send_json(state)

        elif self.path == "/api/forward":
            if browser_instance.forwardStack.top < 0:
                return self._send_json({"error": "Forward stack is empty", **get_current_state()}, status=400)

            c_lib.goForward(ctypes.byref(browser_instance))
            state = get_current_state()
            state["lastAction"] = "forward"
            state["message"] = f"Went forward to {state['currentPage']}"
            return self._send_json(state)

        elif self.path == "/api/clear":
            c_lib.clearHistory(ctypes.byref(browser_instance))
            state = get_current_state()
            state["lastAction"] = "clear"
            state["message"] = "Browser history cleared successfully"
            return self._send_json(state)

        else:
            self.send_error(404, "Endpoint not found")

def run(port=8000):
    server_address = ("127.0.0.1", port)
    httpd = HTTPServer(server_address, BrowserRequestHandler)
    print("=" * 60)
    print(f"[*] C DSA Browser REST Bridge running at http://127.0.0.1:{port}/")
    print(f"    Frontend URL : http://127.0.0.1:{port}/")
    print(f"    API Endpoints: http://127.0.0.1:{port}/api/state")
    print("    Powered by   : C Engine (stack.c + browser.c via ctypes)")
    print("=" * 60)
    print("Press Ctrl+C to stop the server.")
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\nServer shutting down gracefully.")
        httpd.server_close()

if __name__ == "__main__":
    port = 8000
    if len(sys.argv) > 1:
        try:
            port = int(sys.argv[1])
        except ValueError:
            pass
    run(port)
