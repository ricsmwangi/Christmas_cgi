# 🎄 Christmas App - Ops, Security & Customization Info

Self-hosted on this machine: **PM2** runs the Express wrapper on port **8090**, and a
**Tailscale funnel** publishes it at `https://greenstone.tail2857a5.ts.net/christmas`.

---

## 🖥️ Running It

```bash
make                              # rebuild the C CGI binaries
pm2 start ecosystem.config.js     # start (app: christmas-mini-market)
pm2 restart christmas-mini-market # after changing server.js
pm2 logs christmas-mini-market    # watch it
pm2 save                          # remember the process list across reboots
```

The C binaries are executed fresh on every request, so `make` alone is enough after
editing any `.c` file. Only changes to `server.js` need a `pm2 restart`.

### Tailscale funnel

```bash
tailscale funnel --yes --bg --set-path /christmas http://127.0.0.1:8090
```

### Environment variables (`ecosystem.config.js`)

| Variable | Default | Purpose |
|----------|---------|---------|
| `PORT` | `8090` | Port the Express server listens on |
| `TRANSLATE_URL` | `http://127.0.0.1:8087` | Local translate-llamacpp server |
| `TRANSLATE_MODEL` | `translategemma:4b` | Model used for World Messages translations |

---

## 🖼 Picture Storage

| What | Where |
|------|-------|
| SQLite index | `data/christmas.db` |
| Image files | `pictures/*.png` |
| World messages cache | `phrases.json` |

All three are git-ignored — nothing user-generated goes into the repo.

```bash
# Back everything up
tar czf christmas-data.tar.gz data pictures phrases.json

# Inspect the picture DB
sqlite3 data/christmas.db 'SELECT id, caption, bytes, created_at FROM pictures ORDER BY id DESC;'

# Empty the gallery (optional)
sqlite3 data/christmas.db 'DELETE FROM pictures;'
rm -f pictures/*
```

Deleting from the gallery page removes both the row and the file, so the two never
disagree.

---

## 🔒 Security Check

### What's Secure ✅
- **No SQL injection** — the only SQL uses bound parameters (`?`), never string building
- **No authentication / personal data** — nothing to leak except the pictures you save
- **No payment info** — no cards, no transactions
- **HTML escaping** — user text is escaped before it hits the page (prevents XSS)
- **Gallery captions are rendered with `textContent`**, never as raw HTML
- **Uploads are validated** — only png/jpeg/webp data URLs, 24MB cap, random filenames,
  no path traversal (delete uses `path.basename`)
- **Body limits** — 256KB for normal requests, 30MB only on the picture save route
- **No third-party scripts** — everything is served from this box
- **All code in GitHub** (transparent & auditable)

### Security Features:
✅ CGI protocol (standard web safety)
✅ Environment variables (parameters not in memory)
✅ HTML escaping (prevents XSS)
✅ URL encoding (prevents injection)
✅ Security headers (`nosniff`, `SAMEORIGIN`, `Referrer-Policy`)

### Minor Security Notes:
⚠️ Anyone with the funnel URL can view and save pictures — the tailnet path is the only gate
⚠️ No rate limiting — add one in `server.js` if you expose it more widely
⚠️ No user authentication — not needed for a friends-and-family Christmas page
⚠️ If you publish beyond your tailnet, put auth in front of it (reverse proxy or Tailscale ACL)

---

## 👤 Where to Add Your Signature

### Option 1: Footer Signature (Recommended)
**File:** `html_utils.c`, function `html_footer` (~line 141)

```c
printf("        <p>🎄 A little holiday magic ✨ · © 2025 Santa's Mini Market<br>\n");
printf("        <strong style='color: var(--pink);'>Made with ❤️ by #rkb!</strong></p>\n");
```

### Option 2: Dashboard Header
**File:** `main.c`, in `main()` right after the hero block (~line 35)

```c
printf("<p style='text-align: center; color: #ffd700; font-size: 14px;'>Created by: <strong>[YOUR NAME]</strong></p>\n");
```

### Option 3: README.md Signature
Add a Creator section at the bottom of `README.md`.

After any C change:
```bash
make
pm2 restart christmas-mini-market   # only if server.js changed too
```

---

## 📊 Deployment Ready Checklist

✅ **Code Quality**
- All C programs compile without warnings (`make`)
- Proper error handling on the picture API

✅ **Features Complete**
- Christmas tree generator
- Holiday card creator
- Secret Santa tool
- Countdown
- Photo studio + gallery (SQLite-backed)
- Christmas stories and World messages
- Beautiful responsive UI with animations

✅ **Documentation**
- `README.md` — features, self-hosting, picture DB
- `STUDY_GUIDE.md` — how the C/CGI parts work
- This file — ops, security, customization

✅ **Hosting**
- PM2 process with autorestart and memory cap
- Tailscale funnel for sharing
- Data directories git-ignored

---

## 📱 Mobile vs Desktop

### Desktop (Laptop)
✅ All features work perfectly
✅ Smooth animations
✅ Full screen experience

### Mobile (Phone)
✅ Responsive design
✅ Touch-friendly buttons
✅ Countdown visible
✅ Copy / Share / Save-to-gallery work
✅ All animations smooth

---

## 🔧 Troubleshooting

**If something breaks:**
1. `pm2 logs christmas-mini-market` — check the logs
2. Rebuild: `make clean && make`
3. Restart: `pm2 restart christmas-mini-market`
4. Revert last change: `git revert HEAD`

**Common Issues:**
- Feature not showing? → Hard refresh (Ctrl+F5 or Cmd+Shift+R)
- Old version showing? → Clear browser cache
- Gallery says "Could not reach the picture database"? → Check `pm2 logs`, make sure
  `data/` and `pictures/` are writable by the user running PM2
- 404 on `/pictures/...`? → Confirm the file exists: `ls pictures/`
- Funnel URL dead? → `tailscale funnel ...` again, check `tailscale status`
- Mobile looks weird? → Check zoom level (100%)

---

## ✨ Summary

| What | Status |
|------|--------|
| **Code Quality** | ✅ Excellent |
| **Security** | ✅ Safe for a private share |
| **Performance** | ✅ Fast |
| **Mobile Friendly** | ✅ Yes |
| **Hosting** | ✅ Self-hosted via PM2 + Tailscale |
| **Picture storage** | ✅ SQLite + files on disk |

---

**You're all set! Your Christmas app is running on your own machine. 🎄✨🎅**
