# 🎄 Santa's Christmas Mini Market

A festive **pure C CGI web application** with holiday activities built from scratch — no
frameworks, no frontend build step. It runs **on my own machine** under PM2 and is shared
over Tailscale, and it now has a **real picture database** so saved photos stay on the host.

## ✨ What You Can Do

### 🎄 **Christmas Tree Generator**
Generate beautiful ASCII art Christmas trees with custom heights and decorations.

### 💌 **Holiday Card Creator**
Design personalized Christmas cards with custom messages. **Users can share their cards via URL!**
- Enter recipient name
- Write your message
- Share the generated link with friends

### 🎅 **Secret Santa & Gift Exchange**
A full even-gift planner, not just a randomizer:
- Draw where everyone **gives 1 and receives 1** — no self-gifting, always fair
- Optional **budget** splits the pool into *small &amp; sweet / just right / big-ticket* tiers
- Each pair gets a **tailored gift idea** at that price band (no repeat ideas while the pool lasts)
- Group-total estimate, wrapping tips, **re-roll**, **copy** and **download-as-text** exports
- A **gift idea bank** shown even before you draw

### ⏰ **Christmas Countdown**
Live countdown timer to Christmas Day with real-time updates and festive styling.

### 📸 **Christmas Photo Studio**
Upload a photo (or snap one with your webcam) and give it a festive makeover —
golden/tinsel/holly frames, snow, Santa hat, sparkles, warm & frosty filters, custom
greeting. Download, share, copy — or **💾 Save to Gallery**, which stores the finished
picture on the server (file in `pictures/`, row in the SQLite DB).

### 🖼 **Christmas Photo Gallery**
Every picture saved from the studio, browsable at `?action=gallery` — with caption,
size and date, plus a delete button that removes both the DB row and the image file.

### ❄️ **Bonus: Snowfall Animation**
Every page includes beautiful animated snowflakes falling in the background!

---

## 🖥️ How It Runs (self-hosted)

This is self-hosted, not on Render. One Node/Express process wraps the C CGI binaries and
publishes them over Tailscale:

```bash
make                        # compile the CGI binaries
npm install                 # only dependency is express
pm2 start ecosystem.config.js   # app name: christmas-mini-market, port 8090
pm2 save
tailscale funnel --yes --bg --set-path /christmas http://127.0.0.1:8090
```

Live at `https://greenstone.tail2857a5.ts.net/christmas`

Useful commands:

```bash
pm2 logs christmas-mini-market   # watch it
pm2 restart christmas-mini-market
pm2 stop christmas-mini-market
```

### Environment variables (`ecosystem.config.js`)

| Variable | Default | Purpose |
|----------|---------|---------|
| `PORT` | `8090` | Port the Express server listens on |
| `NODE_ENV` | `production` | Node environment |
| `TRANSLATE_URL` | `http://127.0.0.1:8087` | Local translate-llamacpp server (World Messages) |
| `TRANSLATE_MODEL` | `translategemma:4b` | Model used for translations |

---

## 🖼 Picture Database

The gallery is backed by **SQLite** — no database server to install, the whole thing is two
directories next to the code:

```
Christmas_cgi/
├── data/christmas.db   ← SQLite index (one row per picture)
└── pictures/*.png      ← the actual image files (you keep these on your disk)
```

Both directories are created automatically on first start and are **git-ignored** — your
photos never end up in the repo. Back them up (or move them) with a plain copy:

```bash
tar czf christmas-pictures.tar.gz data pictures
```

### Schema

```sql
CREATE TABLE pictures (
  id         INTEGER PRIMARY KEY AUTOINCREMENT,
  file       TEXT    NOT NULL UNIQUE,   -- filename inside ./pictures
  caption    TEXT    NOT NULL DEFAULT '',
  mime       TEXT    NOT NULL DEFAULT 'image/png',
  bytes      INTEGER NOT NULL DEFAULT 0,
  created_at TEXT    NOT NULL DEFAULT (datetime('now'))
);
```

Inspect it with the `sqlite3` CLI if you like:

```bash
sqlite3 data/christmas.db 'SELECT id, caption, bytes, created_at FROM pictures ORDER BY id DESC;'
```

### API

| Method | Route | What it does |
|--------|-------|--------------|
| `POST` | `/api/pictures` | Save a picture. JSON body: `{"caption": "...", "data": "data:image/png;base64,..."}` — png/jpeg/webp, max 24MB decoded |
| `GET` | `/api/pictures` | List saved pictures, newest first |
| `DELETE` | `/api/pictures/:id` | Delete a row **and** its image file |
| `GET` | `/pictures/<file>` | Serve the image file itself |

Example:

```bash
curl -X POST http://127.0.0.1:8090/api/pictures \
  -H 'Content-Type: application/json' \
  -d '{"caption":"Merry Christmas!","data":"data:image/png;base64,iVBORw0KGgo..."}'
```

Photos are only reachable through your server (and whatever tailnet/funnel path you publish
it under). Nothing is sent anywhere else.

---

## 💻 Run Locally (development)

### Requirements
- GCC (`gcc`) and `make`
- **Node.js 22.5+** (the picture DB uses the built-in `node:sqlite` module — no npm DB driver)
- Git

### Installation
```bash
git clone https://github.com/ricsmwangi/Christmas_cgi.git
cd Christmas_cgi

make            # build the CGI binaries
npm install     # express
npm start       # server on http://localhost:3000
```

Then visit `http://localhost:3000` 🎄

---

## 🎯 How It Works

1. **Web Server** (Node.js Express) listens for requests
2. **Server** executes the C CGI program with your input
3. **C Code** generates HTML instantly
4. **Browser** displays the festive result with animations
5. **Gallery saves** go through `/api/pictures` → `data/christmas.db` + `pictures/`

Everything except the gallery is stateless CGI. ⚡

---

## 📁 What's Inside

```
Christmas_cgi/
├── main.c              ← Main CGI app (router + menu, studio, gallery)
├── tree.c              ← Tree generator
├── card.c              ← Card creator
├── santa.c             ← Secret Santa randomizer
├── countdown.c         ← Countdown timer
├── cgi_utils.c/h       ← CGI protocol handling
├── html_utils.c/h      ← HTML/CSS generation
├── server.js           ← Express wrapper + /api/pictures + SQLite
├── ecosystem.config.js ← PM2 config (self-hosting)
├── Makefile            ← Build configuration
├── package.json        ← Dependencies (express only)
├── data/christmas.db   ← Picture DB (created at runtime, git-ignored)
├── pictures/           ← Saved images (created at runtime, git-ignored)
└── README.md           ← This file
```

---

## 🧪 Test It Locally

```bash
# Build
make clean && make

# Test main menu
./main.cgi | grep "Christmas Mini Market"

# Test tree generator
QUERY_STRING="action=tree&height=7" ./main.cgi | head -30

# Test card creator
QUERY_STRING="action=card" ./main.cgi | grep "Holiday Card"

# Test Secret Santa
QUERY_STRING="action=santa" ./main.cgi | grep "Secret Santa"

# Test countdown
QUERY_STRING="action=countdown" ./main.cgi | grep "Countdown"

# Test the gallery page
QUERY_STRING="action=gallery" ./main.cgi | grep "Photo Gallery"

# Test the picture API (with the server running)
curl -s http://127.0.0.1:8090/api/pictures
```

---

## 🎁 Share Custom Messages

**Holiday Card Creator enables sharing!**

Users can:
1. Open the app
2. Enter recipient name
3. Write custom message
4. Send the generated HTML link to friends
5. Each link is a unique festive card!

**Example shared link:**
```
http://localhost:3000/\?action\=card\&name\=Sarah\&message\=Have%20a%20wonderful%20Christmas\!
```

---

## 🛠️ Build & Compile

### Full Build
```bash
make              # Build all executables
make clean        # Remove build artifacts
```

### Individual Programs
```bash
make main.cgi      # Build main app
make tree.cgi      # Build tree generator
make card.cgi      # Build card creator
make santa.cgi     # Build Secret Santa
make countdown.cgi # Build countdown
```

---

## 📊 Features

✅ **Pure C Backend** - No bloated frameworks
✅ **Fast Response Times** - CGI is lightning-fast
✅ **Responsive Design** - Works on phones & desktop
✅ **Self-hosted** - PM2 + Tailscale funnel, no third-party hosting
✅ **SQLite picture database** - Saved photos live on your disk (`data/` + `pictures/`)
✅ **Open Source** - MIT License, modify as you like
✅ **📖 Christmas Stories** - Living canvas sceneries (village, aurora, fireside, night sky) built to stare at
✅ **🌍 World Messages** - Festive messages looping in 14+ languages, with live LLM translation

---

## 📖 Christmas Stories

`?action=story` — a stare-able screensaver screen with four slow, living scenes:
silent village night, aurora forest, fireside with stockings, and night sky with Santa's sleigh.
Tiny story lines fade in and out every ~17s. Controls: scene switcher, **Pause**, and
**Auto-tour** (rotates scenes every 2 minutes). Runs 100% client-side on one canvas.

## 🌍 World Messages (LLM translation loop)

`?action=world` — one festive message loops endlessly through English, French, Spanish,
German, Italian, Portuguese, Dutch, Arabic, Russian, Chinese, Japanese, Korean, Hindi and
Swahili. Add your own message from the page and it:

1. gets stored in `phrases.json` (cached, shown instantly to everyone afterwards),
2. is translated in the background by your local **translate-llamacpp** server
   (`translategemma:4b`, default `http://127.0.0.1:8087`) into 12 languages,
3. joins the loop as soon as it's ready (the page polls every 6s while translating).

API: `GET /api/phrases`, `POST /api/phrases {"text": "..."}`.
Configure via `TRANSLATE_URL` / `TRANSLATE_MODEL` env vars in `ecosystem.config.js`.
Fallback: if the llama is down, the message still loops in English using built-in translations.

---

## 🎨 Customization

Edit `html_utils.c` to customize:
- Colors and styling
- Background gradients
- Festive messages
- CSS and animations

Edit individual `.c` files to change functionality:
- Tree decoration patterns
- Card layouts
- Secret Santa logic
- Countdown styling
- Studio effects and gallery layout (`main.c`)

---

## 📝 Your Custom Footer

Every page includes:
```
🎄 A little holiday magic ✨ · © 2025 Santa's Mini Market
Made with ❤️ by #rkb!
```

This can be edited in `html_utils.c` (the `html_footer` function, around line 141).

---

## 🚀 Performance

- **Load Time:** < 100ms
- **Memory Per Request:** ~500KB
- **Concurrent Users:** Limited by your own box, not a hosting plan
- **Stateless CGI** for everything except the gallery DB

---

## 🎅 About This Project

Built as a demonstration of:
- **C Programming**: Systems-level web development
- **CGI Protocols**: How websites work at the protocol level
- **Web Standards**: HTTP, HTML, CSS, JavaScript
- **Self-hosting**: PM2, SQLite and a Tailscale funnel instead of a hosting platform

**No frameworks. No database server. Just pure C, SQLite and the web.**

---

## 📄 License

MIT License - Feel free to use, modify, and share!

---

## 🎄 Merry Christmas!

```
       *
      ***
     *****
    *******
   *********
  ***********
 *************
 ***************
      |||
      |||
  Ho Ho Ho! 🎅
```

**Built with ❤️ in pure C for the festive season**
