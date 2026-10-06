#include "html_utils.h"
#include "cgi_utils.h"
#include <string.h>
#include <ctype.h>

/* ---------------- CSS (shared) ---------------- */
static void css_block(void) {
    printf("    <style>\n");
    printf("        :root { --gold:#ffd700; --pink:#ff69b4; --teal:#4ecdc4; --ink:#1b2233; }\n");
    printf("        * { box-sizing: border-box; }\n");
    printf("        body { margin:0; min-height:100vh; font-family:'Segoe UI',system-ui,-apple-system,Arial,sans-serif; color:#f4f6ff; background: radial-gradient(1200px 800px at 80%% -10%%, #3b2d6e 0%%, transparent 60%%), radial-gradient(1000px 700px at -10%% 110%%, #123d33 0%%, transparent 55%%), linear-gradient(160deg,#151a3a 0%%, #0e1230 100%%); padding: 0 16px 40px; }\n");
    printf("        a { color: var(--gold); text-decoration:none; }\n");
    printf("        a:hover { text-decoration: underline; }\n");
    printf("        .nav { position: sticky; top:0; z-index:50; display:flex; flex-wrap:wrap; gap:8px 18px; align-items:center; padding:14px 20px; margin:0 -16px 28px; background: rgba(10,12,35,.72); backdrop-filter: blur(10px); border-bottom:1px solid rgba(255,255,255,.08); }\n");
    printf("        .nav .brand { font-weight:800; font-size:18px; letter-spacing:.5px; color:var(--gold); text-decoration:none; }\n");
    printf("        .nav a { color:#cdd6ff; font-weight:600; font-size:14px; text-decoration:none; padding:6px 10px; border-radius:8px; }\n");
    printf("        .nav a:hover { background: rgba(255,255,255,.08); text-decoration:none; }\n");
    printf("        .container { max-width: 960px; margin: 0 auto; background: rgba(255,255,255,.06); padding: 30px; border-radius: 18px; box-shadow: 0 20px 60px rgba(0,0,0,.45); border:1px solid rgba(255,255,255,.08); }\n");
    printf("        h1 { text-align:center; color:var(--gold); margin-top:0; text-shadow:0 2px 14px rgba(255,215,0,.35); }\n");
    printf("        h2 { color:#fff; } h3 { color:var(--gold); }\n");
    printf("        .grid { display:grid; grid-template-columns:repeat(auto-fit,minmax(220px,1fr)); gap:18px; margin:24px 0; }\n");
    printf("        .tile { padding:26px 18px; border-radius:18px; text-align:center; color:#fff; box-shadow:0 8px 24px rgba(0,0,0,.3); transition: transform .2s ease, box-shadow .2s ease; display:flex; flex-direction:column; align-items:center; justify-content:center; min-height:190px; }\n");
    printf("        .tile:hover { transform: translateY(-6px); box-shadow:0 16px 36px rgba(0,0,0,.45); }\n");
    printf("        .tile-ico { font-size:58px; line-height:1; margin-bottom:12px; filter: drop-shadow(0 6px 14px rgba(0,0,0,.4)); }\n");
    printf("        .tile h2 { margin:0 0 16px; font-size:22px; letter-spacing:1px; }\n");
    printf("        .tile p { margin:0 0 14px; font-size:14px; opacity:.95; }\n");
    printf("        .hero { text-align:center; margin: 10px 0 4px; }\n");
    printf("        .hero-emojis { display:block; font-size:46px; letter-spacing:16px; margin-bottom:6px; animation: heroBob 4s ease-in-out infinite; }\n");
    printf("        @keyframes heroBob { 0%%,100%% { transform:translateY(0); } 50%% { transform:translateY(-8px); } }\n");
    printf("        .btn { background: var(--gold); color:#5b4400; padding:10px 20px; border-radius:999px; border:none; cursor:pointer; font-weight:700; display:inline-block; text-decoration:none; transition:filter .15s, transform .15s; }\n");
    printf("        .btn:hover { filter:brightness(1.1); transform:translateY(-1px); text-decoration:none; }\n");
    printf("        .btn.alt { background:var(--teal); color:#04312c; }\n");
    printf("        form { margin:20px 0; }\n");
    printf("        label { display:block; font-weight:600; margin:12px 0 4px; color:#e7ebff; }\n");
    printf("        input, textarea, select { width:100%%; padding:11px 12px; border:1px solid rgba(255,255,255,.15); border-radius:10px; background: rgba(255,255,255,.92); color:var(--ink); font-size:15px; }\n");
    printf("        input:focus, textarea:focus, select:focus { outline:2px solid var(--gold); }\n");
    printf("        .row { display:grid; grid-template-columns:repeat(auto-fit,minmax(180px,1fr)); gap:14px; }\n");
    printf("        pre { background: rgba(0,0,0,.35); padding:20px; border-radius:12px; overflow-x:auto; }\n");
    printf("        .panel { background: rgba(255,255,255,.07); padding:20px; border-radius:14px; margin:20px 0; border:1px solid rgba(255,255,255,.08); }\n");
    printf("        .error { color:#ff9d9d; text-align:center; background:rgba(255,80,80,.12); border:1px solid rgba(255,80,80,.3); padding:12px; border-radius:10px; }\n");
    printf("        .success { color:#9ff5ec; background:rgba(78,205,196,.12); border:1px solid rgba(78,205,196,.3); padding:14px; border-radius:10px; }\n");
    printf("        .christmas-tree { text-align:center; font-family:ui-monospace,Menlo,Consolas,monospace; font-size:20px; line-height:1.4; }\n");
    printf("        .christmas-tree .row { display:flex; justify-content:center; gap:2px; grid-template-columns:none; }\n");
    printf("        .nav-back { background:var(--gold); color:#4d3a00; padding:9px 18px; border-radius:999px; display:inline-block; font-weight:700; margin-top:16px; }\n");
    printf("        .holiday-card { padding:24px; border-radius:16px; border:2px solid var(--gold); background:linear-gradient(135deg,#7a1020,#b3282d); box-shadow:0 12px 30px rgba(0,0,0,.35); }\n");
    printf("        .holiday-card.modern { background:linear-gradient(135deg,#5b3df5,#8a2be2); border-color:#d0b3ff; }\n");
    printf("        .holiday-card.winter { background:linear-gradient(135deg,#0f4c81,#3aa6c9); border-color:#bfefff; }\n");
    printf("        .holiday-card .msg { background: rgba(255,255,255,.94); color:#333; padding:18px; border-radius:12px; font-size:18px; font-style:italic; margin:0; }\n");
    printf("        .countdown-boxes { display:flex; flex-wrap:wrap; gap:12px; justify-content:center; }\n");
    printf("        .countdown-boxes .box { background:rgba(255,255,255,.08); border:1px solid rgba(255,255,255,.12); border-radius:14px; padding:16px 20px; min-width:92px; text-align:center; }\n");
    printf("        .countdown-boxes .num { font-size:34px; font-weight:800; color:var(--gold); }\n");
    printf("        .countdown-boxes .lbl { font-size:12px; text-transform:uppercase; letter-spacing:2px; color:#c9d2ff; }\n");
    printf("        footer { text-align:center; margin-top:36px; padding:20px; border-top:1px solid rgba(255,215,0,.25); color:#b9c2ee; font-size:14px; }\n");
    printf("        .tile-green { background: linear-gradient(135deg,#1f8a3f,#2fbf71); }\n");
    printf("        .tile-pink { background: linear-gradient(135deg,#c94b8f,#ff8fc7); }\n");
    printf("        .tile-orange { background: linear-gradient(135deg,#e0571f,#ff8a4c); }\n");
    printf("        .tile-blue { background: linear-gradient(135deg,#3b5bdb,#7aa5ff); }\n");
    printf("        .tile-red { background: linear-gradient(135deg,#b3242a,#ff6b6b); }\n");
    printf("        .tile-purple { background: linear-gradient(135deg,#5b2c8f,#9b59b6); }\n");
    printf("        .tile-teal { background: linear-gradient(135deg,#0f6b6b,#2bb3a3); }\n");
    printf("        .tile-gold { background: linear-gradient(135deg,#8a6d0f,#d4af37); }\n");
    printf("        #wmBox { max-width: 760px; margin: 18px auto; padding: 42px 26px; text-align: center;\n");
    printf("            background: linear-gradient(160deg, rgba(179,36,42,.92), rgba(15,45,84,.94));\n");
    printf("            border-radius: 18px; border: 1px solid rgba(255,255,255,.18);\n");
    printf("            box-shadow: 0 18px 50px rgba(0,0,0,.45), inset 0 0 60px rgba(255,215,0,.12);\n");
    printf("            min-height: 200px; display: flex; flex-direction: column; justify-content: center; overflow: hidden; }\n");
    printf("        #wmLang { font-size: 15px; letter-spacing: 3px; text-transform: uppercase; color: #ffd700; font-weight: 700; margin-bottom: 14px; }\n");
    printf("        #wmText { font-size: clamp(24px, 4.4vw, 44px); color: #fff; font-weight: 800; line-height: 1.25; text-shadow: 0 3px 14px rgba(0,0,0,.55); }\n");
    printf("        #wmText.pop { animation: wmpop .5s ease; }\n");
    printf("        @keyframes wmpop { from { opacity: 0; transform: translateY(10px) scale(.98); } to { opacity: 1; transform: none; } }\n");
    printf("        #wmCount { margin-top: 16px; font-size: 13px; color: rgba(255,255,255,.75); }\n");
    printf("        @media (prefers-reduced-motion: reduce) { #wmText.pop { animation: none; } }\n");
    printf("        .story-panel { padding: 12px; }\n");
    printf("        .story-stage { position: relative; border-radius: 14px; overflow: hidden; background: #050a24; box-shadow: 0 20px 50px rgba(0,0,0,.55); }\n");
    printf("        .story-stage canvas { display: block; width: 100%%; height: auto; }\n");
    printf("        .story-bar { position: absolute; top: 12px; left: 50%%; transform: translateX(-50%%); display: flex; gap: 7px; padding: 7px 9px; background: rgba(8,12,34,.55); backdrop-filter: blur(10px); border: 1px solid rgba(255,255,255,.2); border-radius: 999px; max-width: calc(100%% - 20px); z-index: 3; flex-wrap: wrap; justify-content: center; }\n");
    printf("        .story-bar .btn { padding: 7px 11px; font-size: 16px; line-height: 1; border-radius: 999px; }\n");
    printf("        .story-bar .btn.on { background: var(--gold); color: #241503; }\n");
    printf("        .story-stage:fullscreen { display: flex; align-items: center; justify-content: center; border-radius: 0; }\n");
    printf("        .story-stage:fullscreen canvas { width: 100%%; height: 100%%; object-fit: cover; }\n");
    printf("        #cap { position: absolute; left: 0; right: 0; bottom: 16px; text-align: center; padding: 0 30px; font-style: italic; font-size: 17px; color: #ffe9a8; text-shadow: 0 2px 10px rgba(0,0,0,.9); transition: opacity 1.5s ease; z-index: 2; }\n");
    printf("        @keyframes santaDance { 0%%,100%% { transform: translateX(-8px) rotate(-4deg);} 50%% { transform: translateX(8px) rotate(4deg);} }\n");
    printf("        .santa-dance { font-size:64px; text-align:center; animation: santaDance 1.6s ease-in-out infinite; }\n");
    printf("        .assignment { display:flex; justify-content:space-between; padding:10px 14px; border-bottom:1px solid rgba(255,255,255,.08); }\n");
    printf("        .assignment:last-child { border-bottom:none; }\n");
    printf("        .assignment .gift-idea { color:var(--gold); font-weight:600; }\n");
    printf("        @keyframes glow { 0%%,100%% { box-shadow:0 0 6px currentColor; opacity:1;} 50%% { box-shadow:0 0 18px currentColor; opacity:.55;} }\n");
    printf("        .lights { display:flex; justify-content:center; gap:14px; padding:8px 0; margin-top:-18px; margin-bottom:22px; flex-wrap:wrap; }\n");
    printf("        .lights span { width:10px; height:10px; border-radius:50%%; animation: glow 1.8s ease-in-out infinite; }\n");
    printf("        .lights span:nth-child(4n+1){ background:#ff5252; color:#ff5252; }\n");
    printf("        .lights span:nth-child(4n+2){ background:#4ecdc4; color:#4ecdc4; animation-delay:.4s; }\n");
    printf("        .lights span:nth-child(4n+3){ background:#ffd700; color:#ffd700; animation-delay:.8s; }\n");
    printf("        .lights span:nth-child(4n+4){ background:#9b59b6; color:#9b59b6; animation-delay:1.2s; }\n");
    printf("        @keyframes floaty { 0%%,100%% { transform:translateY(0) rotate(-4deg);} 50%% { transform:translateY(-14px) rotate(4deg);} }\n");
    printf("        .floaty { position:fixed; font-size:34px; z-index:1; animation: floaty 4s ease-in-out infinite; opacity:.9; pointer-events:none; }\n");
    printf("        .studio-row { display:flex; gap:10px; align-items:center; flex-wrap:wrap; }\n");
    printf("    </style>\n");
}

void html_header(const char *title, const char *css) {
    char safe_title[256];
    html_escape(title ? title : "Christmas", safe_title, sizeof(safe_title));
    printf("Content-Type: text/html\n\n");
    printf("<!DOCTYPE html>\n");
    printf("<html lang='en'>\n");
    printf("<head>\n");
    printf("    <meta charset='UTF-8'>\n");
    printf("    <meta name='viewport' content='width=device-width, initial-scale=1.0'>\n");
    printf("    <title>%s</title>\n", safe_title);
    if (css) {
        printf("    <style>%s</style>\n", css);
    } else {
        css_block();
    }
    printf("</head>\n<body>\n");
    printf("    <nav class='nav'>\n");
    printf("        <a class='brand' href='?'>🎄 Santa's Mini Market</a>\n");
    printf("        <a href='?action=tree' title='Tree'>🎄</a>\n");
    printf("        <a href='?action=card' title='Card'>💌</a>\n");
    printf("        <a href='?action=santa' title='Secret Santa'>🎁</a>\n");
    printf("        <a href='?action=countdown' title='Countdown'>⏰</a>\n");
    printf("        <a href='?action=studio' title='Photo Studio'>📸</a>\n");
    printf("        <a href='?action=gallery' title='Gallery'>🖼</a>\n");
    printf("        <a href='?action=story' title='Stories'>📖</a>\n");
    printf("        <a href='?action=world' title='World Messages'>🌍</a>\n");
    printf("        <a href='?action=about' title='About'>ℹ️</a>\n");
    printf("    </nav>\n");
    printf("    <div class='lights' id='lights'>");
    for (int i = 0; i < 28; i++) printf("<span></span>");
    printf("</div>\n");
    printf("    <div class='floaty' style='left:2vw; top:20vh;'>🎁</div>\n");
    printf("    <div class='floaty' style='right:2vw; top:35vh; animation-delay:1.2s;'>🎄</div>\n");
    printf("    <div class='floaty' style='left:3vw; top:60vh; animation-delay:2.1s;'>⛄</div>\n");
    printf("    <div class='floaty' style='right:3vw; top:70vh; animation-delay:.6s;'>🦌</div>\n");
    printf("    <main id='app' class='container'>\n");
}

void html_footer(void) {
    printf("    </main>\n");
    printf("    <footer>\n");
    printf("        <p>🎄 A little holiday magic ✨ · © 2025 Santa's Mini Market<br>\n");
    printf("        <strong style='color: var(--pink);'>Made with ❤️ by #rkb!</strong></p>\n");
    printf("    </footer>\n");
    printf("<script>\n");
    printf("(function(){\n");
    printf("  var cdTimer = null;\n");
    printf("  function startCountdown(){\n");
    printf("    var elem = document.getElementById('countdown-display');\n");
    printf("    if(!elem) return;\n");
    printf("    if(cdTimer) clearInterval(cdTimer);\n");
    printf("    var tick = function(){\n");
    printf("      var now = new Date();\n");
    printf("      var xmas = new Date(now.getFullYear(), 11, 25);\n");
    printf("      if(now >= new Date(xmas.getTime() + 86400000)) xmas.setFullYear(now.getFullYear() + 1);\n");
    printf("      var diff = Math.max(0, xmas - now);\n");
    printf("      var d = Math.floor(diff/86400000), h = Math.floor(diff/3600000)%%24,\n");
    printf("          m = Math.floor(diff/60000)%%60, s = Math.floor(diff/1000)%%60;\n");
    printf("      elem.innerHTML = '<div class=countdown-boxes>' +\n");
    printf("        box(d,'days') + box(h,'hours') + box(m,'mins') + box(s,'secs') + '</div>';\n");
    printf("      function box(n,l){ return '<div class=box><div class=num>'+n+'</div><div class=lbl>'+l+'</div></div>'; }\n");
    printf("    };\n");
    printf("    tick(); cdTimer = setInterval(tick, 1000);\n");
    printf("  }\n");
    printf("  function snow(){\n");
    printf("    var old = document.getElementById('snowflakes'); if(old) old.remove();\n");
    printf("    var c = document.createElement('canvas'); c.id='snowflakes';\n");
    printf("    c.style.cssText='position:fixed;inset:0;pointer-events:none;z-index:9999';\n");
    printf("    document.body.appendChild(c); var ctx=c.getContext('2d');\n");
    printf("    function rs(){ c.width=innerWidth; c.height=innerHeight; } rs();\n");
    printf("    addEventListener('resize', rs);\n");
    printf("    var f=[]; for(var i=0;i<90;i++) f.push({x:Math.random()*innerWidth,y:Math.random()*innerHeight,r:Math.random()*2+0.6,s:Math.random()*1.2+0.4,d:Math.random()*200});\n");
    printf("    (function loop(t){ ctx.clearRect(0,0,c.width,c.height);\n");
    printf("      ctx.fillStyle='rgba(255,255,255,.85)';\n");
    printf("      f.forEach(function(p){ p.y+=p.s; p.x+=Math.sin((t+p.d)/500)*0.4;\n");
    printf("        if(p.y>c.height+4){p.y=-4;p.x=Math.random()*c.width;}\n");
    printf("        ctx.beginPath(); ctx.arc(p.x,p.y,p.r,0,6.283); ctx.fill(); });\n");
    printf("      requestAnimationFrame(loop); })(0);\n");
    printf("  }\n");
    printf("  function swap(html, url){\n");
    printf("    var doc = new DOMParser().parseFromString(html, 'text/html');\n");
    printf("    var app = doc.getElementById('app');\n");
    printf("    if(!app){ window.location.href = url; return; }\n");
    printf("    document.title = doc.title;\n");
    printf("    document.getElementById('app').innerHTML = app.innerHTML;\n");
    printf("    // Re-run any inline scripts from the swapped-in content (innerHTML does not execute them)\n");
    printf("    app.querySelectorAll('script').forEach(function(s){\n");
    printf("      var n = document.createElement('script');\n");
    printf("      n.textContent = s.textContent;\n");
    printf("      document.body.appendChild(n);\n");
    printf("    });\n");
    printf("    window.scrollTo(0,0); startCountdown();\n");
    printf("  }\n");
    printf("  document.addEventListener('click', function(e){\n");
    printf("    var a = e.target.closest('a'); if(!a) return;\n");
    printf("    var href = a.getAttribute('href');\n");
    printf("    if(!href || href[0] === '#' || href.indexOf('://') > -1 && href.indexOf(location.host) === -1) return;\n");
    printf("    if(href.indexOf('?') === 0 || href === '' || href === '?'){ e.preventDefault();\n");
    printf("      var url = href === '' ? '?' : href;\n");
    printf("      fetch(url).then(function(r){return r.text()}).then(function(h){ swap(h,url); history.pushState({}, '', url); }).catch(function(){ location.href = url; });\n");
    printf("    }\n");
    printf("  });\n");
    printf("  document.addEventListener('submit', function(e){\n");
    printf("    var f = e.target; if(f.tagName !== 'FORM') return;\n");
    printf("    e.preventDefault();\n");
    printf("    var action = f.getAttribute('action') || '?';\n");
    printf("    var url = new URL(action, location.href);\n");
    printf("    new FormData(f).forEach(function(v,k){ url.searchParams.set(k,v); });\n");
    printf("    var rel = url.search;\n");
    printf("    fetch(rel).then(function(r){return r.text()}).then(function(h){ swap(h, rel); history.pushState({}, '', rel); }).catch(function(){ location.href = rel; });\n");
    printf("  });\n");
    printf("  window.addEventListener('popstate', function(){ location.reload(); });\n");
    printf("  window.copyCard = function(){\n");
    printf("    var el = document.querySelector('.holiday-card'); if(!el) return;\n");
    printf("    navigator.clipboard.writeText(el.innerText).then(function(){ alert('🎄 Card copied to clipboard!'); },\n");
    printf("      function(){ alert('Could not copy — please select the text manually.'); });\n");
    printf("  };\n");
    printf("  snow(); startCountdown();\n");
    printf("})();\n");
    printf("</script>\n");
    printf("</body>\n</html>\n");
}

/* ---------------- forms ---------------- */
void html_form_start(const char *action, const char *method) {
    char safe_action[256];
    html_escape(action ? action : "?", safe_action, sizeof(safe_action));
    printf("<form action='%s' method='%s'>\n", safe_action, method ? method : "GET");
}

void html_form_end(void) { printf("</form>\n"); }

void html_text_input(const char *name, const char *placeholder, const char *value) {
    char n[64], p[128], v[256];
    html_escape(name ? name : "", n, sizeof(n));
    html_escape(placeholder ? placeholder : "", p, sizeof(p));
    html_escape(value ? value : "", v, sizeof(v));
    printf("<input type='text' name='%s' placeholder='%s' value='%s'>\n", n, p, v);
}

void html_textarea(const char *name, const char *placeholder, const char *value) {
    char n[64], p[128], v[1024];
    html_escape(name ? name : "", n, sizeof(n));
    html_escape(placeholder ? placeholder : "", p, sizeof(p));
    html_escape(value ? value : "", v, sizeof(v));
    printf("<textarea name='%s' placeholder='%s' rows='4'>%s</textarea>\n", n, p, v);
}

void html_select_start(const char *name) {
    char n[64]; html_escape(name ? name : "", n, sizeof(n));
    printf("<select name='%s'>\n", n);
}

void html_select_option(const char *value, const char *text, int selected) {
    char v[64], t[64];
    html_escape(value ? value : "", v, sizeof(v));
    html_escape(text ? text : "", t, sizeof(t));
    printf("<option value='%s'%s>%s</option>\n", v, selected ? " selected" : "", t);
}

void html_select_end(void) { printf("</select>\n"); }

void html_submit_button(const char *text) {
    char t[64]; html_escape(text ? text : "Submit", t, sizeof(t));
    printf("<button type='submit' name='submit' value='1'>%s</button>\n", t);
}

/* ---------------- color validation ---------------- */
int is_safe_color(const char *c) {
    if (!c) return 0;
    if (c[0] != '#') return 0;
    int len = (int)strlen(c) - 1;
    if (len != 6 && len != 3) return 0;
    for (int i = 1; c[i]; i++) {
        if (!isxdigit((unsigned char)c[i])) return 0;
    }
    return 1;
}

/* ---------------- tree ---------------- */
void html_christmas_tree(int height, const char *ornaments, const char *color) {
    const char *safe = is_safe_color(color) ? color : "#228b22";

    /* collect UTF-8 symbol offsets from ornaments (handles emoji) */
    const char *start[128];
    int count = 0;
    if (ornaments && *ornaments) {
        for (const char *p = ornaments; *p && count < 128; p++) {
            int len = 1;
            if ((*p & 0xE0) == 0xC0) len = 2;
            else if ((*p & 0xF0) == 0xE0) len = 3;
            else if ((*p & 0xF8) == 0xF0) len = 4;
            start[count++] = p;
            p += len - 1;
        }
    }

    printf("<div class='christmas-tree'>\n<h2>🎄 Your Christmas Tree</h2>\n");
    printf("<div style='color:%s;'>\n", safe);

    for (int i = 1; i <= height; i++) {
        printf("<div class='row'>\n");
        for (int j = 0; j < 2 * i - 1; j++) {
            if (count > 0) {
                const char *s = start[j % count];
                int len = 1;
                if ((*s & 0xE0) == 0xC0) len = 2;
                else if ((*s & 0xF0) == 0xE0) len = 3;
                else if ((*s & 0xF8) == 0xF0) len = 4;
                int whole = (len == 1 && (*s == '*' || *s == '!')) ? 1 : 0;
                if (whole && *s == '*') printf("<span>★</span>");
                else if (whole && *s == '!') printf("<span>✨</span>");
                else { char buf[8]; memcpy(buf, s, len); buf[len] = 0; printf("<span>%s</span>", buf); }
            } else {
                printf("<span>★</span>");
            }
        }
        printf("\n</div>\n");
    }
    for (int i = 0; i < 3; i++) {
        printf("<div class='row'><span>║</span><span>║</span><span>║</span></div>\n");
    }
    printf("</div>\n</div>\n");
}

/* ---------------- holiday card ---------------- */
void html_holiday_card(const char *recipient, const char *message, const char *theme) {
    char safe_r[512], safe_m[2048];
    html_escape(recipient ? recipient : "", safe_r, sizeof(safe_r));
    html_escape(message ? message : "", safe_m, sizeof(safe_m));

    const char *cls = "";
    if (theme && strcmp(theme, "modern") == 0) cls = " modern";
    else if (theme && strcmp(theme, "winter") == 0) cls = " winter";

    printf("<div class='holiday-card%s'>\n", cls);
    printf("<h2>🎄 Happy Holidays, %s!</h2>\n", safe_r);
    printf("<div style='display:flex;align-items:center;gap:20px;flex-wrap:wrap;'>\n");
    printf("<div style='font-size:60px;'>🎅</div>\n");
    printf("<div style='flex-grow:1;min-width:220px;'>\n");
    printf("<p class='msg'>%s</p>\n", safe_m);
    printf("<p style='text-align:right;color:#ffe9a8;margin:10px 0 0;'>From Santa 🎅</p>\n");
    printf("</div></div></div>\n");
}

/* ---------------- countdown ---------------- */
void html_countdown_timer(void) {
    printf("<div id='countdown-display' style='margin:25px 0;'></div>\n");
}

/* ---------------- error/success pages ---------------- */
void html_error_page(int status_code, const char *message) {
    char safe_m[512]; html_escape(message ? message : "Unknown error", safe_m, sizeof(safe_m));
    html_header("Error", NULL);
    printf("<div class='error'><h1>❌ Error %d</h1><p>%s</p></div>\n", status_code, safe_m);
    printf("<p style='text-align:center;'><a class='nav-back' href='?'>← Back to Home</a></p>\n");
    html_footer();
}

void html_success_page(const char *title, const char *message) {
    char safe_t[128], safe_m[512];
    html_escape(title ? title : "Success", safe_t, sizeof(safe_t));
    html_escape(message ? message : "Your request has been processed.", safe_m, sizeof(safe_m));
    html_header(title, NULL);
    printf("<div class='success'><h1>✅ %s</h1><p>%s</p></div>\n", safe_t, safe_m);
    printf("<p style='text-align:center;'><a class='nav-back' href='?'>← Create Another</a></p>\n");
    html_footer();
}
