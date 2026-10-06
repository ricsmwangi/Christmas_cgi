# 🎄 Santa's Christmas Mini Market

A festive pure **C CGI web application** with four holiday activities built entirely from scratch - no frameworks, no Node.js bloat, just pure web performance.

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
greeting. Download, share, or copy the result. Runs 100% in your browser, nothing is uploaded.

### ❄️ **Bonus: Snowfall Animation**
Every page includes beautiful animated snowflakes falling in the background!

---

## 🌐 Deploy to Render.com (Free!)

### Quick 3-Step Setup

**Step 1:** Go to [render.com](https://render.com)

**Step 2:** Click **"New +"** → **"Web Service"**
- Select this GitHub repository
- Name it: `christmas-cgi`
- Choose Node environment

**Step 3:** Configure Build & Start
```
Build Command: npm install && make
Start Command: npm start
```

Your app goes live in **2-3 minutes**! 🎅

## 🖥️ Run with PM2 (this server)

```bash
make                     # build the CGI binaries
pm2 start ecosystem.config.js   # christmas-mini-market on port 8090
pm2 save
tailscale funnel --yes --bg --set-path /christmas http://127.0.0.1:8090
```

Live at `https://greenstone.tail2857a5.ts.net/christmas`

---

## 💻 Run Locally

### Requirements
- GCC compiler (gcc)
- Node.js 14+
- Make
- Git

### Installation
```bash
# Clone the repo
git clone https://github.com/ricsmwangi/Christmas_cgi.git
cd Christmas_cgi

# Build everything
make

# Install Node dependencies
npm install

# Start the server
npm start
```

Then visit `http://localhost:3000` 🎄

---

## 🎯 How It Works

1. **Web Server** (Node.js Express) listens for requests
2. **Server** executes the C CGI program with your input
3. **C Code** generates HTML instantly
4. **Browser** displays the festive result with animations

**No backend database. No APIs. Just pure C speed.** ⚡

---

## 📁 What's Inside

```
Christmas_cgi/
├── main.c              ← Main CGI app (router + menu)
├── tree.c              ← Tree generator
├── card.c              ← Card creator
├── santa.c             ← Secret Santa randomizer
├── countdown.c         ← Countdown timer
├── cgi_utils.c/h       ← CGI protocol handling
├── html_utils.c/h      ← HTML/CSS generation
├── server.js           ← Express wrapper
├── Makefile            ← Build configuration
├── package.json        ← Dependencies
├── render.yaml         ← Render deployment config
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
✅ **No Database** - Everything computed on-the-fly  
✅ **Easy Deploy** - One-click Render.com deployment  
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

---

## 📝 Your Custom Footer

Every page includes:
```
🎄 Hey guys, so this festive season you can enjoy 
something I made for everyone! 🎄
```

This can be edited in `html_utils.c` lines 38-45.

---

## 🚀 Performance

- **Load Time:** < 100ms
- **Memory Per Request:** ~500KB
- **Concurrent Users:** Unlimited
- **No Scaling Issues:** Stateless CGI design

---

## 🎯 Next Steps

1. **Fork** this repository to your GitHub
2. **Deploy** to Render.com (connect GitHub)
3. **Share** the live link with friends
4. **Customize** the code and redeploy automatically

---

## 🎅 About This Project

Built as a demonstration of:
- **C Programming**: Systems-level web development
- **CGI Protocols**: How websites work at the protocol level
- **Web Standards**: HTTP, HTML, CSS, JavaScript
- **DevOps**: GitHub → Render.com deployment pipeline

**No frameworks. No databases. Just pure C and the web.** 

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
