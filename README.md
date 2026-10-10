# Browser History Management Using Stacks

A modular C console application simulating browser navigation using the **Stack** data structure (LIFO - Last In, First Out).

---

## 📁 Project Structure

```text
Browser History Management Using Stacks/
├── stack.h        <- Header file for Stack data structure definitions and prototypes
├── stack.c        <- Implementation file for custom Stack operations
├── browser.h      <- Header file for Browser navigation definitions and prototypes
├── browser.c      <- Implementation file for Browser navigation logic
├── main.c         <- Console menu and user interaction entry point
└── README.md      <- Project documentation
```

---

## 💡 How the Two-Stack Browser Simulation Works

The browser maintains two stacks:
1. **Back Stack (`backStack`)**: Stores the history of previously visited URLs.
2. **Forward Stack (`forwardStack`)**: Stores URLs that were left behind when clicking "Back".

### Core Behavior:
- **Visit a new URL**:
  - If a page is currently open, push `currentPage` onto `backStack`.
  - Clear `forwardStack` (visiting a new link resets the forward history).
  - Update `currentPage` with the new URL.
- **Go Back**:
  - Check if `backStack` is empty. If so, navigation is not possible.
  - Push `currentPage` onto `forwardStack`.
  - Pop the top URL from `backStack` and make it the new `currentPage`.
- **Go Forward**:
  - Check if `forwardStack` is empty. If so, navigation is not possible.
  - Push `currentPage` onto `backStack`.
  - Pop the top URL from `forwardStack` and make it the new `currentPage`.

---

## 🛠️ How to Compile and Run

### 1. Terminal / CLI Version
If using GCC (MSYS2 UCRT64 / MinGW):
```bash
gcc main.c browser.c stack.c -o browser.exe
./browser.exe
```

### 2. Web Frontend Version (Interactive Visual UI)
Run the lightweight REST bridge server (uses Python built-in standard library, 0 external pip packages needed):
```bash
python server.py
```
Then open your browser at:
```text
http://127.0.0.1:8000/
```
The web frontend provides:
- Live visual **Back Stack** and **Forward Stack** containers displaying items in **LIFO** order from `[TOP]` to base.
- Address bar with quick preset links (`google.com`, `github.com`, `wikipedia.org`, etc.).
- Back, Forward, Visit, and Clear buttons directly calling the underlying C Stack ADT functions.
- Execution event log showing stack push, pop, and boundary checks.
