# 🎄 Christmas CGI Project - Study Guide

> A pure C web application that generates festive Christmas activities. No frameworks, just C + CGI + HTML!

---

## 🚀 Quick Start

**What does this do?**
- Generates Christmas trees, holiday cards, secret Santa assignments, countdown timers
- Built entirely in C (compiled to CGI programs)
- Runs on Node.js Express server
- Self-hosted with PM2, shared over a Tailscale funnel

**Try it:**
1. Visit the app (locally at `http://localhost:3000`, or the funnel URL)
2. Click "Create Tree" - generate custom ASCII trees
3. Click "Create Card" - make personalized holiday cards
4. Copy and share!

---

## 📁 Project Structure

```
christmas/
├── main.c           ← Main router (decides what to show)
├── tree.c           ← Tree generator
├── card.c           ← Card creator
├── santa.c          ← Secret Santa
├── countdown.c      ← Christmas countdown
├── cgi_utils.c      ← Reads URL parameters
├── html_utils.c     ← Generates HTML/CSS
├── server.js        ← Express web server
└── Makefile         ← Build configuration
```

---

## ⚙️ How It Works (Simple Version)

### The Flow:

```
1. You visit: www.myapp.com/?action=tree&height=10
   ↓
2. Express server receives the URL
   ↓
3. Express sets QUERY_STRING="action=tree&height=10"
   ↓
4. Express runs: ./main.cgi
   ↓
5. main.c reads QUERY_STRING
   ↓
6. main.c sees action=tree
   ↓
7. main.c calls generate_tree(height=10)
   ↓
8. C program prints HTML with printf()
   ↓
9. Express sends HTML to your browser
   ↓
10. Browser shows beautiful Christmas tree
```

### Example URL:
```
?action=card&recipient=John&message=Happy+Holidays
```

This tells the app:
- `action=card` → Show card creator
- `recipient=John` → For John
- `message=Happy Holidays` → With this message

---

## 🎯 Key Concepts

### 1. **CGI (Common Gateway Interface)**

CGI is how web servers talk to programs:

```
Program Input:  Environment variables (QUERY_STRING)
Program Output: HTML to stdout (printf)
Web Server:     Returns output to browser
```

**Example:**
```bash
# URL: /?action=tree&height=5
# CGI converts this to:
QUERY_STRING="action=tree&height=5"

# C program reads it:
char *action = cgi_get_param("action");      // Gets "tree"
char *height = cgi_get_param("height");      // Gets "5"
```

### 2. **Parameter Parsing**

The `cgi_get_param()` function reads URL parameters:

```c
// URL: /?name=Santa&color=red
char *name = cgi_get_param("name");      // → "Santa"
char *color = cgi_get_param("color");    // → "red"
```

### 3. **HTML Generation**

C program generates HTML with printf():

```c
printf("<h1>Hello, %s!</h1>\n", name);  // → <h1>Hello, Santa!</h1>
printf("<div style='color: %s;'>...</div>\n", color);
```

---

## 🎨 Current Features

### ✅ Working & Tested

| Feature | What It Does |
|---------|-------------|
| **Christmas Tree** | Generate ASCII trees with custom ornaments and colors |
| **Holiday Card** | Create personalized cards with recipient name & message |
| **Copy Card** | Copy card text to clipboard, paste anywhere |
| **Dancing Santa** | Animated Santa on main dashboard |
| **Falling Snow** | Snowflakes & trees fall in background |
| **Countdown** | Days/hours/minutes until Christmas |
| **Secret Santa** | Random gift assignment generator |

### 📋 How to Use Each Feature:

**1. Tree Generator**
- Select height (5-15 lines)
- Choose ornament symbols
- Pick color (red, green, gold, etc.)
- See beautiful ASCII tree

**2. Holiday Card**
- Enter recipient name
- Write your message
- Click "Copy Card"
- Paste in WhatsApp/Email/Text message
- Recipient sees formatted card!

**3. Secret Santa**
- Add participant names
- Click "Generate"
- Get random assignments

**4. Countdown**
- Shows days until Christmas
- Counts down in real-time
- Festive styling

---

## 🔧 File Explanations

### `main.c` (Main Router)
**What it does:**
- Reads the `action` parameter from URL
- Decides which function to call
- Routes to tree/card/santa/countdown

**Key part:**
```c
char *action = cgi_get_param("action");

if (strcmp(action, "tree") == 0) {
    show_tree_section();
} else if (strcmp(action, "card") == 0) {
    show_card_section();
}
// etc...
```

### `html_utils.c` (HTML Generator)
**What it does:**
- Generates all HTML/CSS
- Creates the Christmas tree
- Designs the holiday card
- Adds animations (snowfall, dancing Santa)

**Example:**
```c
printf("<div style='color: red; font-size: 20px;'>");
printf("🎄 Merry Christmas! 🎄");
printf("</div>\n");
```

### `cgi_utils.c` (Parameter Parser)
**What it does:**
- Reads QUERY_STRING from environment
- Extracts individual parameters
- Returns them as strings

**Usage:**
```c
char *name = cgi_get_param("recipient");  // Gets URL parameter
```

### `server.js` (Web Server)
**What it does:**
- Runs Express web server
- Receives HTTP requests
- Passes parameters to C program
- Returns HTML response

---

## 🏗️ Build & Deploy

### Building Locally

```bash
# Compile all C programs to .cgi files
make

# Run the web server
npm install
npm start

# Visit browser: http://localhost:3000
```

### Deploying (self-hosted)

```
1. make                              ← rebuild the C CGI binaries
2. pm2 restart christmas-mini-market ← if server.js changed
3. tailscale funnel ...              ← republish the URL if needed
4. App is live!
```

---

## 🐛 Recent Fixes (December 22)

| Problem | Solution |
|---------|----------|
| Tree wasn't centered | Changed to CSS flexbox centering |
| Share link broken | Removed - now just copy button with instructions |
| Pointing hand distracting | Removed from card display |
| Special characters breaking | Added proper character escaping in C |

---

## 💡 Learning Tips

### Understand the Flow:
1. Pick a feature (like tree generator)
2. Follow the code:
   - `main.c` → detects `action=tree`
   - Calls `show_tree_section()`
   - Calls `html_christmas_tree()`
   - Prints HTML with `printf()`
3. Browser renders the HTML

### Try Modifying:
1. Change tree ornament symbols in `html_utils.c`
2. Change colors in CSS
3. Add new form fields
4. Recompile with `make`
5. Refresh browser

### Experiment with URLs:
```
/?action=tree&height=20&ornaments=*!#
/?action=card&recipient=Mom&message=Love+You
/?action=countdown
```

---

## 📚 Key Files to Learn

**Start with:**
1. `main.c` - Understand routing
2. `cgi_utils.c` - Understand parameter reading
3. `html_utils.c` - See HTML generation

**Then explore:**
4. `tree.c` - Algorithm for ASCII tree
5. `server.js` - Web server setup
6. `Makefile` - Build process

---

## 🎯 Next Steps

1. **Modify colors** - Edit CSS in `html_utils.c`
2. **Change ornaments** - Edit tree symbols
3. **Add new theme** - Create new card style
4. **Create new feature** - Add a new .c file
5. **Ship changes** - `make`, `pm2 restart christmas-mini-market` if `server.js` changed

---

## ✅ Status: FULLY WORKING

- ✅ All features implemented
- ✅ All bugs fixed
- ✅ Running self-hosted (PM2 + Tailscale funnel)
- ✅ Picture gallery backed by SQLite (`data/christmas.db` + `pictures/`)
- ✅ Ready to share with friends!

**Latest commits:**
```
0bc3487 - Remove share link button, keep copy with instructions
658e347 - Fix: Proper character escaping for share link
6ee4571 - Remove pointing hand from card
```

---

**Happy learning! Enjoy your Christmas app! 🎄✨🎅**

