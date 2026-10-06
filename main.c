#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "cgi_utils.h"
#include "html_utils.h"

#define MAX_PARTICIPANTS 20

// Function prototypes
void show_main_menu();
void show_tree_section();
void show_card_section();
void show_santa_section();
void show_countdown_section();
void show_studio_section();
void show_gallery_section();
void show_story_section();
void show_world_section();
void show_about_section();

int main() {
    // Initialize CGI environment
    cgi_init();

    // Get action parameter to determine what to show
    char *action = cgi_get_param("action");

    html_header("🎄 Santa's Christmas Mini Market", NULL);

    // If no action specified, show the main menu
    if (!action) {
        printf("<div class='hero'>\n");
        printf("<span class='hero-emojis'>🎄 🎅 ⛄ 🦌 🎁 ❄️</span>\n");
        printf("<h1>🎄 Santa's Christmas Mini Market</h1>\n");
        printf("</div>\n");
        show_main_menu();
    } else if (strcmp(action, "tree") == 0) {
        show_tree_section();
    } else if (strcmp(action, "card") == 0) {
        show_card_section();
    } else if (strcmp(action, "santa") == 0) {
        show_santa_section();
    } else if (strcmp(action, "countdown") == 0) {
        show_countdown_section();
    } else if (strcmp(action, "studio") == 0) {
        show_studio_section();
    } else if (strcmp(action, "gallery") == 0) {
        show_gallery_section();
    } else if (strcmp(action, "story") == 0) {
        show_story_section();
    } else if (strcmp(action, "world") == 0) {
        show_world_section();
    } else if (strcmp(action, "about") == 0) {
        show_about_section();
    } else {
        // Invalid action, show main menu
        printf("<div class='error'>\n");
        printf("<p>❌ Nothing here — pick a tile below.</p>\n");
        printf("</div>\n");
        show_main_menu();
    }

    html_footer();

    return 0;
}

void show_main_menu() {
    printf("<div class='santa-dance'>🎅</div>\n");

    printf("<div class='grid'>\n");

    printf("<div class='tile tile-green'><div class='tile-ico'>🎄</div>\n");
    printf("<h2>Tree</h2>\n<a class='btn' href='?action=tree'>Open</a>\n</div>\n");

    printf("<div class='tile tile-pink'><div class='tile-ico'>💌</div>\n");
    printf("<h2>Card</h2>\n<a class='btn' href='?action=card'>Open</a>\n</div>\n");

    printf("<div class='tile tile-orange'><div class='tile-ico'>🎁</div>\n");
    printf("<h2>Secret Santa</h2>\n<a class='btn' href='?action=santa'>Open</a>\n</div>\n");

    printf("<div class='tile tile-blue'><div class='tile-ico'>⏰</div>\n");
    printf("<h2>Countdown</h2>\n<a class='btn' href='?action=countdown'>Open</a>\n</div>\n");

    printf("<div class='tile tile-red'><div class='tile-ico'>📸</div>\n");
    printf("<h2>Photo Studio</h2>\n<a class='btn' href='?action=studio'>Open</a>\n</div>\n");

    printf("<div class='tile tile-gold'><div class='tile-ico'>🖼</div>\n");
    printf("<h2>Gallery</h2>\n<a class='btn' href='?action=gallery'>Open</a>\n</div>\n");

    printf("<div class='tile tile-purple'><div class='tile-ico'>📖</div>\n");
    printf("<h2>Stories</h2>\n<a class='btn' href='?action=story'>Open</a>\n</div>\n");

    printf("<div class='tile tile-teal'><div class='tile-ico'>🌍</div>\n");
    printf("<h2>World</h2>\n<a class='btn' href='?action=world'>Open</a>\n</div>\n");

    printf("</div>\n");
}

void show_tree_section() {
    // Get parameters
    char *height_str = cgi_get_param("height");
    char *ornaments = cgi_get_param("ornaments");
    char *color = cgi_get_param("color");
    char *message = cgi_get_param("message");

    // Set defaults
    int height = height_str ? atoi(height_str) : 10;
    if (height < 3 || height > 20) height = 10;
    if (!ornaments) ornaments = "🎄❄️*!";
    if (!color) color = "#ff0000";

    printf("<div class='panel'>\n");
    printf("<h2>🎄 Christmas Tree Generator</h2>\n");

    // Show the generated tree if parameters provided
    if (height_str) {
        html_christmas_tree(height, ornaments, color);

        if (message && strlen(message) > 0) {
            char escaped_message[1024];
            html_escape(message, escaped_message, sizeof(escaped_message));
            printf("<div style='text-align: center; margin: 20px 0; font-size: 18px; color: #ffd700; font-weight: bold;'>\n");
            printf("✨ %s ✨\n", escaped_message);
            printf("</div>\n");
        }
    }

    // Tree customization form
    printf("<h3>Customize Your Tree:</h3>\n");
    html_form_start("?action=tree", "GET");

    printf("<div class='row'>\n");

    printf("<div>\n");
    printf("<label>Tree Height (3-20):</label>\n");
    html_text_input("height", "10", height_str);
    printf("</div>\n");

    printf("<div>\n");
    printf("<label>Ornaments:</label>\n");
    html_text_input("ornaments", "🎄❄️", ornaments);
    printf("</div>\n");

    printf("<div>\n");
    printf("<label>Tree Color:</label>\n");
    html_select_start("color");
    html_select_option("#ff0000", "Red ❤️", strcmp(color, "#ff0000") == 0);
    html_select_option("#228b22", "Green", strcmp(color, "#228b22") == 0);
    html_select_option("#ffd93d", "Gold", strcmp(color, "#ffd93d") == 0);
    html_select_option("#6bcf7f", "Light Green", strcmp(color, "#6bcf7f") == 0);
    html_select_option("#ff69b4", "Pink", strcmp(color, "#ff69b4") == 0);
    html_select_end();
    printf("</div>\n");

    printf("<div>\n");
    printf("<label>Custom Message:</label>\n");
    html_text_input("message", "Merry Christmas!", message);
    printf("</div>\n");

    printf("</div>\n");

    html_submit_button("🎄 Generate Tree");

    html_form_end();

    printf("<p style='text-align: center; margin-top: 20px;'>\n");
    printf("<a href='?' class='nav-back'>← Back to Main Menu</a>\n");
    printf("</p>\n");

    printf("</div>\n");
}

void show_card_section() {
    // Get form data
    char *recipient = cgi_get_param("recipient");
    char *message = cgi_get_param("message");
    char *theme = cgi_get_param("theme");

    printf("<div class='panel'>\n");
    printf("<h2>🎅 Holiday Card Creator</h2>\n");

    // Show card if both recipient and message are provided
    if (recipient && recipient[0] != '\0' && message && message[0] != '\0') {
        // Show the created card
        printf("<h3>Your Holiday Card:</h3>\n");
        html_holiday_card(recipient, message, theme);

        printf("<div style='text-align: center; margin: 20px;'>\n");
        printf("<button onclick='copyCard()' style='background: #4ecdc4; color: white; padding: 10px 20px; border-radius: 25px; border: none; cursor: pointer; margin: 5px;'>📋 Copy Card</button>\n");
        printf("<a href='?action=card' style='background: #ff69b4; color: white; padding: 10px 20px; border-radius: 25px; text-decoration: none; display: inline-block; margin: 5px;'>Create Another Card</a>\n");
        printf("</div>\n");
        
        printf("<div style='background: rgba(100, 200, 255, 0.1); padding: 15px; border-radius: 10px; margin-top: 20px; border-left: 4px solid #4ecdc4;'>\n");
        printf("<p style='margin: 0; color: #333; font-weight: bold;'>💡 How to Send:</p>\n");
        printf("<p style='margin: 5px 0; color: #555;'>1. Click <strong>Copy Card</strong> to copy the card text</p>\n");
        printf("<p style='margin: 5px 0; color: #555;'>2. Paste it in WhatsApp, Email, or any message app</p>\n");
        printf("<p style='margin: 5px 0; color: #555;'>3. Your recipient will see the beautiful formatted card! 🎄</p>\n");
        printf("</div>\n");
        
        printf("<script>\n");
        printf("function copyCard() {\n");
        printf("    const cardContent = document.querySelector('.holiday-card');\n");
        printf("    if (cardContent) {\n");
        printf("        const text = cardContent.innerText;\n");
        printf("        navigator.clipboard.writeText(text).then(() => {\n");
        printf("            alert('✅ Card copied to clipboard!\\n\\nNow paste it to send to your recipient!');\n");
        printf("        }).catch(() => {\n");
        printf("            alert('Failed to copy. Try selecting the card manually.');\n");
        printf("        });\n");
        printf("    }\n");
        printf("}\n");
        printf("</script>\n");
    }

    // Card creation form
    printf("<h3>Create a New Card:</h3>\n");
    html_form_start("?action=card", "GET");

    printf("<div class='row'>\n");

    printf("<div>\n");
    printf("<label>Recipient Name:</label>\n");
    html_text_input("recipient", "Who is this card for?", recipient);
    printf("</div>\n");

    printf("<div>\n");
    printf("<label>Card Theme:</label>\n");
    html_select_start("theme");
    html_select_option("traditional", "Traditional", (!theme || strcmp(theme, "traditional") == 0));
    html_select_option("modern", "Modern", theme && strcmp(theme, "modern") == 0);
    html_select_option("winter", "Winter Wonderland", theme && strcmp(theme, "winter") == 0);
    html_select_end();
    printf("</div>\n");

    printf("</div>\n");

    printf("<label>Your Message:</label>\n");
    html_textarea("message", "Write your holiday message here...", message);

    html_submit_button("🎅 Create Card");

    html_form_end();

    printf("<p style='text-align: center; margin-top: 20px;'>\n");
    printf("<a href='?' class='nav-back'>← Back to Main Menu</a>\n");
    printf("</p>\n");

    printf("</div>\n");
}

/* URL-encode a string for safe use inside href query strings */
static void url_encode(const char *in, char *out, size_t out_len) {
    static const char *hex = "0123456789ABCDEF";
    size_t o = 0;
    for (size_t i = 0; in && in[i] && o + 4 < out_len; i++) {
        unsigned char c = (unsigned char)in[i];
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            out[o++] = (char)c;
        } else {
            out[o++] = '%';
            out[o++] = hex[c >> 4];
            out[o++] = hex[c & 15];
        }
    }
    out[o] = '\0';
}

void show_santa_section() {
    char *participants_str = cgi_get_param("participants");
    char *budget_str = cgi_get_param("budget");
    char safe_budget[32] = "";
    if (budget_str) html_escape(budget_str, safe_budget, sizeof(safe_budget));

    printf("<div class='panel'>\n");
    printf("<h2>🎁 Secret Santa & Gift Exchange</h2>\n");
    printf("<p>Everyone draws <strong>one</strong> name — one gift given, one received. Always even.</p>\n");

    html_form_start("?action=santa", "GET");
    printf("<div class='row'>\n");
    printf("<div><label>Participants (comma-separated)</label>\n");
    html_text_input("participants", "Alice, Bob, Charlie, Diana", participants_str);
    printf("</div>\n");
    printf("<div><label>Budget per gift (optional, any currency)</label>\n");
    printf("<input type='number' name='budget' min='1' max='1000000' placeholder='e.g. 20' value='%s'>\n", safe_budget);
    printf("</div>\n");
    printf("</div>\n");
    html_submit_button("🎄 Draw names & get gift ideas");
    html_form_end();
    printf("</div>\n");

    if (!participants_str || participants_str[0] == '\0') {
        /* gift idea bank so the page is useful even before drawing */
        printf("<div class='panel'>\n");
        printf("<h3>💡 Gift idea bank</h3>\n");
        printf("<div class='row'>\n");
        printf("<div><strong>🧺 Small &amp; sweet</strong><ul><li>🍫 Artisan chocolates</li><li>🧦 Fun socks</li><li>☕ Mug + cocoa mix</li><li>🕯️ Scented candle</li><li>🃏 Funny card + note</li></ul></div>\n");
        printf("<div><strong>🎁 Crowd-pleasers</strong><ul><li>📖 Bestselling book</li><li>🎧 Wireless earbuds</li><li>🧣 Scarf &amp; gloves</li><li>🎲 Board game</li><li>🧴 Self-care set</li></ul></div>\n");
        printf("<div><strong>👑 Big-ticket</strong><ul><li>🎒 Stylish backpack</li><li>⌚ Smart band</li><li>🥂 Celebration set</li><li>💍 Jewelry</li><li>🕹️ Gaming accessory</li></ul></div>\n");
        printf("</div>\n");
        printf("<p style='font-size:14px;color:#b9c2ee;'>Tip: add a budget above and each pairing gets a ready-made gift idea at the right price.</p>\n");
        printf("</div>\n");
        return;
    }

    char *participants[MAX_PARTICIPANTS];
    int count = 0;

    char *copy = strdup(participants_str);
    char *token = strtok(copy, ",");
    while (token && count < MAX_PARTICIPANTS) {
        while (*token == ' ') token++;
        char *end = token + strlen(token) - 1;
        while (end > token && *end == ' ') *end-- = '\0';
        if (strlen(token) > 0) participants[count++] = strdup(token);
        token = strtok(NULL, ",");
    }
    free(copy);

    if (count < 2) {
        printf("<div class='error'>❌ Please enter at least 2 participants!</div>\n");
        printf("<p style='text-align:center;'><a href='?action=santa' class='nav-back'>← Try again</a></p>\n");
        return;
    }

    srand((unsigned int)time(NULL) ^ (unsigned int)getpid());
    int assignments[MAX_PARTICIPANTS];
    int available[MAX_PARTICIPANTS];
    int valid = 0;

    for (int attempt = 0; attempt < 100 && !valid; attempt++) {
        valid = 1;
        for (int i = 0; i < count; i++) { assignments[i] = -1; available[i] = i; }
        for (int i = count - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            int t = available[i]; available[i] = available[j]; available[j] = t;
        }
        for (int i = 0; i < count; i++) {
            int r = -1;
            for (int j = 0; j < count; j++) {
                if (available[j] != -1 && available[j] != i) { r = available[j]; available[j] = -1; break; }
            }
            if (r == -1) { valid = 0; break; }
            assignments[i] = r;
        }
    }

    if (!valid) {
        printf("<div class='error'>❌ Couldn't make a valid draw. Please try again!</div>\n");
        for (int i = 0; i < count; i++) free(participants[i]);
        return;
    }

    int budget = budget_str ? atoi(budget_str) : 0;

    /* gift idea pools per budget tier */
    static const char *LOW[] = {
        "Box of artisan chocolates", "Fun statement socks", "Mug + hot cocoa mix",
        "Scented candle", "Movie-night snack pack", "Funny card + handwritten note"
    };
    static const char *MID[] = {
        "Bestselling novel", "Wireless earbuds", "Scarf & gloves set",
        "Board game night kit", "Gourmet treat basket", "Self-care / spa set",
        "Coffee-lover bundle"
    };
    static const char *HIGH[] = {
        "Stylish backpack", "Gaming accessory", "Celebration gift set",
        "Smart fitness band", "Jewelry piece", "Vinyl or music gift card"
    };
    static const char *TIPS[] = {
        "Wrap it yourself — presentation counts!",
        "Add a handwritten note, it's the best part.",
        "Tuck the receipt quietly inside.",
        "Experiences work too: cinema, coffee, escape room.",
        "Pair it with homemade treats for extra charm."
    };
    const char **pool;
    int pool_len;
    const char *tier_name;
    if (budget > 0 && budget < 10)        { pool = LOW;  pool_len = (int)(sizeof(LOW)  / sizeof(LOW[0]));  tier_name = "small & sweet"; }
    else if (budget >= 10 && budget < 30) { pool = MID;  pool_len = (int)(sizeof(MID)  / sizeof(MID[0]));  tier_name = "just right"; }
    else if (budget >= 30)                { pool = HIGH; pool_len = (int)(sizeof(HIGH) / sizeof(HIGH[0])); tier_name = "big-ticket"; }
    else                                  { pool = MID;  pool_len = (int)(sizeof(MID)  / sizeof(MID[0]));  tier_name = "all-round"; }

    printf("<div class='panel'>\n");
    printf("<h3>🎄 Your draw</h3>\n");
    printf("<div id='pairings'>\n");
    int used[64] = {0};
    for (int i = 0; i < count; i++) {
        char safe_a[128], safe_b[128];
        html_escape(participants[i], safe_a, sizeof(safe_a));
        html_escape(participants[assignments[i]], safe_b, sizeof(safe_b));

        /* pick an unused gift idea from the tier pool (reuse only when everyone got one) */
        int idx = rand() % pool_len;
        for (int tries = 0; tries < pool_len && used[idx]; tries++) idx = (idx + 1) % pool_len;
        if (!used[idx]) used[idx] = 1;
        const char *idea = pool[idx];

        char safe_idea[256];
        html_escape(idea, safe_idea, sizeof(safe_idea));

        printf("<div class='assignment' data-giver='%s' data-receiver='%s' data-idea='%s'>\n",
               safe_a, safe_b, safe_idea);
        printf("<div>🧑 <strong>%s</strong><br><span style='font-size:13px;color:#b9c2ee;'>buys for</span></div>\n", safe_a);
        printf("<div>🎁 <strong>%s</strong></div>\n", safe_b);
        printf("<div class='gift-idea'>💡 %s</div>\n", safe_idea);
        printf("</div>\n");
    }
    printf("</div>\n");

    const char *tip = TIPS[rand() % (int)(sizeof(TIPS) / sizeof(TIPS[0]))];
    char safe_tip[256]; html_escape(tip, safe_tip, sizeof(safe_tip));
    printf("<p style='font-size:14px;color:#9ff5ec;'>🎄 %s</p>\n", safe_tip);

    if (budget > 0) {
        printf("<p style='font-size:14px;color:#c9d2ff;'>Budget: <strong>%d</strong> per gift (%s tier) · "
               "group total ≈ <strong>%d</strong> · every gift in the draw sits at the same price band — "
               "no one gets the cheap one. 🎯</p>\n", budget, tier_name, budget * count);
    } else {
        printf("<p style='font-size:14px;color:#c9d2ff;'>💡 Set a budget to get price-matched gift ideas — "
               "so every gift feels <em>even</em>.</p>\n");
    }
    printf("<p style='font-size:13px;color:#b9c2ee;'>Even-gift rules: each person gives <strong>1</strong> and receives <strong>1</strong> · no self-gifting · the draw is fresh on every load.</p>\n");

    char enc_part[1024];
    url_encode(participants_str, enc_part, sizeof(enc_part));
    char enc_budget[64];
    url_encode(safe_budget, enc_budget, sizeof(enc_budget));

    printf("<div style='text-align:center;margin:16px 0;display:flex;gap:10px;flex-wrap:wrap;justify-content:center;'>\n");
    printf("<a class='btn alt' id='reroll' href='?action=santa&participants=%s&budget=%s'>🔄 Re-roll the draw</a>\n",
           enc_part, enc_budget);
    printf("<button class='btn' type='button' id='copyDraw'>📋 Copy draw</button>\n");
    printf("<button class='btn' type='button' id='dlDraw'>⬇ Download as text</button>\n");
    printf("</div>\n");
    printf("</div>\n");

    printf("<script>\n");
    printf("(function(){\n");
    printf("var txt=function(){return Array.prototype.map.call(document.querySelectorAll('#pairings .assignment'),function(el){\n");
    printf("  return el.dataset.giver+'  →  '+el.dataset.receiver+'   |   gift idea: '+el.dataset.idea;}).join('\\n');};\n");
    printf("var c=document.getElementById('copyDraw');if(c)c.addEventListener('click',function(){\n");
    printf("  if(navigator.clipboard&&navigator.clipboard.writeText){navigator.clipboard.writeText('Secret Santa draw\\n\\n'+txt()).catch(function(){});alert('📋 Draw copied to clipboard!');}\n");
    printf("  else{prompt('Copy your draw:', txt());}});\n");
    printf("var d=document.getElementById('dlDraw');if(d)d.addEventListener('click',function(){\n");
    printf("  var b=new Blob(['Secret Santa draw\\n\\n'+txt()+'\\n'],{type:'text/plain'});\n");
    printf("  var a=document.createElement('a');a.href=URL.createObjectURL(b);a.download='secret-santa-draw.txt';a.click();\n");
    printf("  setTimeout(function(){URL.revokeObjectURL(a.href);},4000);});\n");
    printf("var r=document.getElementById('reroll');if(r){var base=r.getAttribute('href');r.addEventListener('click',function(){r.href=base+'&_='+Date.now();});}\n");
    printf("})();\n");
    printf("</script>\n");

    for (int i = 0; i < count; i++) free(participants[i]);
    printf("<p style='text-align:center;'><a href='?action=santa' class='nav-back'>← Start over</a></p>\n");
}

void show_countdown_section() {
    printf("<div class='panel'>\n");
    printf("<h2>⏰ Christmas Countdown</h2>\n");

    html_countdown_timer();

    printf("<div style='text-align: center; margin: 30px 0;'>\n");
    printf("<h3>🎄 Merry Christmas!</h3>\n");
    printf("<p>Joy, love &amp; celebration — see you on the 25th! 🎅</p>\n");
    printf("</div>\n");

    // Simple Christmas tree
    printf("<div style='text-align: center;'>\n");
    printf("<pre style='color: #228b22; display: inline-block; text-align: left;'>\n");
    printf("         *\n");
    printf("        ***\n");
    printf("       *****\n");
    printf("      *******\n");
    printf("     *********\n");
    printf("    ***********\n");
    printf("   *************\n");
    printf("  ***************\n");
    printf("        |||\n");
    printf("        |||\n");
    printf("</pre>\n");
    printf("</div>\n");

    printf("<p style='text-align: center; margin-top: 20px;'>\n");
    printf("<a href='?' class='nav-back'>← Back to Main Menu</a>\n");
    printf("</p>\n");

    printf("</div>\n");
}

void show_studio_section() {
    printf("<div class='panel'>\n");
    printf("<h2>📸 Christmas Photo Studio</h2>\n");
    printf("<p>Snap or pick a photo, make it merry, then download, share, or save it to the server gallery.</p>\n");

    printf("<div class='row'>\n");
    printf("<div><label>1. Choose a photo</label><input type='file' id='photo' accept='image/*'></div>\n");
    printf("<div><label>or use your camera</label><button class='btn alt' type='button' id='camBtn'>📷 Use Webcam</button></div>\n");
    printf("</div>\n");
    printf("<div id='camWrap' style='display:none;text-align:center;margin:12px 0;'>\n");
    printf("<video id='cam' autoplay playsinline style='max-width:100%%;border-radius:12px;background:#000;'></video><br>\n");
    printf("<button class='btn' type='button' id='snapBtn'>🎬 Capture Photo</button>\n");
    printf("</div>\n");

    printf("<h3>2. Christmas Edits</h3>\n");
    printf("<div class='row'>\n");
    printf("<div><label>Frame</label><select id='frame'>"
           "<option value='none'>No frame</option>"
           "<option value='gold' selected>Golden</option>"
           "<option value='tinsel'>Tinsel</option>"
           "<option value='holly'>Holly &amp; berries</option>"
           "<option value='custom'>Custom color</option></select></div>\n");
    printf("<div><label>Color effect</label><select id='fx'>"
           "<option value='none'>None</option>"
           "<option value='warm' selected>Cozy warm</option>"
           "<option value='frost'>Frosty blue</option>"
           "<option value='vintage'>Vintage</option>"
           "<option value='night'>Silent night</option></select></div>\n");
    printf("<div><label>Snow ❄</label><input type='range' id='snow' min='0' max='300' value='120'></div>\n");
    printf("<div><label>Frame color</label><input type='color' id='frameColor' value='#d4af37'></div>\n");
    printf("<div><label>Your greeting</label><input type='text' id='greet' maxlength='60' value='Merry Christmas!'></div>\n");
    printf("</div>\n");

    printf("<p style='text-align:center;'><label><input type='checkbox' id='hat' checked> 🎅 Santa hat</label> &nbsp; "
           "<label><input type='checkbox' id='sparkle' checked> ✨ Sparkles</label></p>\n");

    printf("<p id='studioMsg' class='error' style='display:none;'></p>\n");
    printf("<canvas id='stage' style='max-width:100%%;width:100%%;border-radius:12px;background:#12162e;'></canvas>\n");

    printf("<div style='text-align:center;margin:16px 0;display:flex;gap:10px;flex-wrap:wrap;justify-content:center;'>\n");
    printf("<button class='btn' type='button' id='dlBtn'>⬇ Download</button>\n");
    printf("<button class='btn alt' type='button' id='shBtn'>📤 Share</button>\n");
    printf("<button class='btn' type='button' id='cpBtn'>📋 Copy</button>\n");
    printf("<button class='btn alt' type='button' id='saveBtn'>💾 Save to Gallery</button>\n");
    printf("</div>\n");

    printf("<script>\n");
    printf("(function(){\n");
    printf("var cvs=document.getElementById('stage'),ctx=cvs.getContext('2d'),img=null,stream=null;\n");
    printf("function G(id){return document.getElementById(id);}\n");
    printf("function msg(t){var m=G('studioMsg');if(!t){m.style.display='none';return;}m.style.display='block';m.textContent=t;}\n");
    printf("function placeholder(){cvs.width=800;cvs.height=560;ctx.fillStyle='#12162e';ctx.fillRect(0,0,800,560);ctx.textAlign='center';ctx.font='90px serif';ctx.fillText('🎄',400,250);ctx.fillStyle='#ffd700';ctx.font='24px sans-serif';ctx.fillText('Pick a photo to begin',400,380);}\n");
    printf("function loadFile(f){\n");
    printf("  if(!f)return; if(!f.type||f.type.indexOf('image/')!==0){msg('That file is not an image.');return;}\n");
    printf("  if(f.size>20*1024*1024){msg('Image too large (max 20MB).');return;}\n");
    printf("  msg('');var r=new FileReader();\n");
    printf("  r.onload=function(e){img=new Image();img.onload=function(){msg('');draw();};img.onerror=function(){msg('Could not decode that image.');};img.src=e.target.result;};\n");
    printf("  r.onerror=function(){msg('Could not read that file.');};r.readAsDataURL(f);\n");
    printf("}\n");
    printf("G('photo').addEventListener('change',function(){loadFile(this.files[0]);});\n");

    printf("function toggleCam(){\n");
    printf("  var wrap=G('camWrap'),v=G('cam');\n");
    printf("  if(stream){stream.getTracks().forEach(function(t){t.stop();});stream=null;wrap.style.display='none';G('camBtn').textContent='📷 Use Webcam';return;}\n");
    printf("  if(!navigator.mediaDevices||!navigator.mediaDevices.getUserMedia){msg('Webcam not supported in this browser.');return;}\n");
    printf("  navigator.mediaDevices.getUserMedia({video:{facingMode:'user'}}).then(function(s){stream=s;v.srcObject=s;wrap.style.display='block';G('camBtn').textContent='⏹ Stop Camera';msg('');})\n");
    printf("    .catch(function(){msg('Camera permission denied or unavailable.');});\n");
    printf("}\n");
    printf("G('camBtn').addEventListener('click',toggleCam);\n");
    printf("G('snapBtn').addEventListener('click',function(){\n");
    printf("  var v=G('cam'); if(!v||!v.videoWidth){msg('Camera not ready yet.');return;}\n");
    printf("  var t=document.createElement('canvas');t.width=v.videoWidth;t.height=v.videoHeight;\n");
    printf("  t.getContext('2d').drawImage(v,0,0);\n");
    printf("  img=new Image();img.onload=function(){msg('');draw();};img.src=t.toDataURL('image/jpeg',0.92);\n");
    printf("});\n");

    printf("function trees(w,h){var s=Math.round(w*0.07);ctx.font=s+'px serif';ctx.textAlign='center';ctx.fillText('🎄',s*1.1,s*1.1);ctx.fillText('🎄',w-s*1.1,s*1.1);ctx.fillText('🎄',s*1.1,h-s*0.4);ctx.fillText('🎄',w-s*1.1,h-s*0.4);}\n");
    printf("function tinsel(w,h){var step=Math.max(14,Math.round(w/50)),cols=['#e74c3c','#ffd700','#2ecc71','#ffffff'];var i=0;ctx.save();ctx.lineWidth=Math.max(6,w*0.012);\n");
    printf("  for(var x=0;x<w;x+=step){dot(x,ctx.lineWidth,dotSize(),cols[i++%%4]);dot(x,h-ctx.lineWidth/2,dotSize(),cols[i++%%4]);}\n");
    printf("  for(var y=0;y<h;y+=step){dot(ctx.lineWidth/2,y,dotSize(),cols[i++%%4]);dot(w-ctx.lineWidth/2,y,dotSize(),cols[i++%%4]);}\n");
    printf("  function dot(x,y,r,c){ctx.fillStyle=c;ctx.beginPath();ctx.arc(x,y,r,0,6.283);ctx.fill();}\n");
    printf("  function dotSize(){return Math.max(5,Math.round(step*0.42));}ctx.restore();}\n");
    printf("function holly(w,h){var step=Math.max(40,Math.round(w/16));ctx.save();\n");
    printf("  for(var x=step/2;x<w;x+=step){berry(x,step*0.3);berry(x,h-step*0.3);}\n");
    printf("  for(var y=step/2;y<h;y+=step){berry(step*0.3,y);berry(w-step*0.3,y);}\n");
    printf("  function berry(x,y){var r=Math.max(4,step*0.16);ctx.fillStyle='#1e7d32';ctx.beginPath();ctx.arc(x-r,y,r,0,6.283);ctx.fill();ctx.beginPath();ctx.arc(x+r,y,r,0,6.283);ctx.fill();ctx.fillStyle='#e63946';ctx.beginPath();ctx.arc(x,y,r*0.9,0,6.283);ctx.fill();}ctx.restore();}\n");
    printf("function hat(w,h){var bw=w*0.36,bx=w*0.08,by=h*0.02,hh=Math.max(40,h*0.16);ctx.save();\n");
    printf("  ctx.fillStyle='#c0392b';ctx.beginPath();ctx.moveTo(bx,by+hh);ctx.quadraticCurveTo(bx+bw*0.45,by-hh*0.35,bx+bw,by+hh*0.45);ctx.lineTo(bx+bw*0.9,by+hh*1.05);ctx.quadraticCurveTo(bx+bw*0.5,by+hh*0.7,bx+bw*0.1,by+hh*1.25);ctx.closePath();ctx.fill();\n");
    printf("  ctx.fillStyle='#fff';ctx.beginPath();ctx.ellipse(bx+bw*0.45,by+hh*1.1,bw*0.5,hh*0.24,0,0,6.283);ctx.fill();\n");
    printf("  ctx.beginPath();ctx.arc(bx+bw*0.97,by+hh*0.42,hh*0.26,0,6.283);ctx.fill();ctx.restore();}\n");
    printf("function snow(w,h,n){ctx.save();for(var i=0;i<n;i++){var r=Math.random()*3+1;ctx.globalAlpha=0.35+Math.random()*0.6;ctx.fillStyle='#fff';ctx.beginPath();ctx.arc(Math.random()*w,Math.random()*h,r,0,6.283);ctx.fill();}ctx.restore();}\n");
    printf("function sparkles(w,h){ctx.save();ctx.textAlign='center';ctx.font=Math.round(w/16)+'px serif';for(var i=0;i<14;i++){ctx.globalAlpha=0.5+Math.random()*0.5;ctx.fillText('✨',Math.random()*w,Math.random()*h);}ctx.restore();}\n");
    printf("function greeting(w,h){var t=G('greet').value.trim();if(!t)return;var size=Math.max(22,Math.round(w/16));ctx.save();ctx.textAlign='center';ctx.font='bold '+size+'px Georgia, serif';ctx.lineWidth=Math.max(3,size/9);ctx.strokeStyle='rgba(80,0,0,.85)';ctx.strokeText(t,w/2,h-size*0.6);ctx.fillStyle='#ffd700';ctx.fillText(t,w/2,h-size*0.6);ctx.restore();}\n");

    printf("function draw(){\n");
    printf("  if(!img){placeholder();return;}\n");
    printf("  var scale=Math.min(1,1400/img.width);cvs.width=Math.round(img.width*scale);cvs.height=Math.round(img.height*scale);\n");
    printf("  var w=cvs.width,h=cvs.height;\n");
    printf("  var fx={none:'',warm:'sepia(.3) saturate(1.4) contrast(1.05)',frost:'brightness(1.1) saturate(1.2) hue-rotate(-15deg)',vintage:'sepia(.65) contrast(.85) brightness(1.05)',night:'brightness(.68) saturate(1.3) hue-rotate(5deg)'};\n");
    printf("  ctx.filter=fx[G('fx').value]||'none';ctx.drawImage(img,0,0,w,h);ctx.filter='none';\n");
    printf("  var fr=G('frame').value;\n");
    printf("  if(fr==='gold'){ctx.strokeStyle=G('frameColor').value;ctx.lineWidth=Math.max(14,Math.round(w*0.03));ctx.strokeRect(ctx.lineWidth/2,ctx.lineWidth/2,w-ctx.lineWidth,h-ctx.lineWidth);trees(w,h);}\n");
    printf("  else if(fr==='tinsel'){tinsel(w,h);}else if(fr==='holly'){holly(w,h);}\n");
    printf("  else if(fr==='custom'){ctx.strokeStyle=G('frameColor').value;ctx.lineWidth=Math.max(16,Math.round(w*0.035));ctx.strokeRect(ctx.lineWidth/2,ctx.lineWidth/2,w-ctx.lineWidth,h-ctx.lineWidth);}\n");
    printf("  if(G('hat').checked){hat(w,h);}\n");
    printf("  snow(w,h,parseInt(G('snow').value,10)||0);\n");
    printf("  if(G('sparkle').checked){sparkles(w,h);}\n");
    printf("  greeting(w,h);\n");
    printf("}\n");

    printf("['frame','fx','snow','frameColor','greet','hat','sparkle'].forEach(function(id){\n");
    printf("  G(id).addEventListener('input',draw);G(id).addEventListener('change',draw);});\n");

    printf("function blob(cb){cvs.toBlob(function(b){if(!b){msg('Could not render image.');return;}cb(b);},'image/png');}\n");
    printf("G('dlBtn').addEventListener('click',function(){blob(function(b){var a=document.createElement('a');a.href=URL.createObjectURL(b);a.download='merry-christmas.png';a.click();setTimeout(function(){URL.revokeObjectURL(a.href);},4000);});});\n");
    printf("G('shBtn').addEventListener('click',function(){blob(function(b){var f=new File([b],'merry-christmas.png',{type:'image/png'});\n");
    printf("  if(navigator.canShare&&navigator.canShare({files:[f]})){navigator.share({files:[f],title:'🎄 Merry Christmas!'}).catch(function(){});}\n");
    printf("  else{msg('Sharing not supported here — use Download instead.');}});});\n");
    printf("G('cpBtn').addEventListener('click',function(){blob(function(b){\n");
    printf("  if(!window.ClipboardItem||!navigator.clipboard||!navigator.clipboard.write){msg('Copy not supported in this browser — use Download.');return;}\n");
    printf("  navigator.clipboard.write([new ClipboardItem({'image/png':b})]).then(function(){msg('');alert('📋 Image copied to clipboard!');},function(){msg('Copy blocked by browser — use Download.');});});});\n");

    /* Save the finished festive picture to the server gallery (SQLite + ./pictures) */
    printf("var BASE=(location.pathname==='/'||location.pathname==='')?'':location.pathname.replace(/\\/$/,'');\n");
    printf("G('saveBtn').addEventListener('click',function(){\n");
    printf("  if(!img){msg('Pick a photo first.');return;}\n");
    printf("  var b=this;b.disabled=true;msg('Saving to the gallery…');\n");
    printf("  var data=cvs.toDataURL('image/png');\n");
    printf("  fetch(BASE+'/api/pictures',{method:'POST',headers:{'Content-Type':'application/json'},\n");
    printf("    body:JSON.stringify({caption:G('greet').value.trim(),data:data})})\n");
    printf("    .then(function(r){return r.json();})\n");
    printf("    .then(function(d){b.disabled=false;if(!d.ok)throw new Error(d.error||'save failed');\n");
    printf("      msg('');location.href=BASE+'/?'+'action=gallery';})\n");
    printf("    .catch(function(e){b.disabled=false;msg('Could not save: '+(e&&e.message||'server error'));});\n");
    printf("});\n");

    printf("placeholder();\n");
    printf("})();\n");
    printf("</script>\n");
    printf("</div>\n");
}

void show_gallery_section() {
    printf("<div class='panel'>\n");
    printf("<h2>🖼️ Christmas Photo Gallery</h2>\n");
    printf("<p>Every festive picture saved from the <a href='?action=studio'>Photo Studio</a> — "
           "the image files live on this server under <code>pictures/</code> and are indexed in "
           "<code>data/christmas.db</code>.</p>\n");
    printf("<p id='galStatus' style='text-align:center;color:#c8d2ff;min-height:20px;'>Loading the gallery…</p>\n");
    printf("<div id='gal' style='display:grid;grid-template-columns:repeat(auto-fill,minmax(220px,1fr));gap:16px;'></div>\n");
    printf("<p style='text-align:center;margin-top:20px;display:flex;gap:10px;flex-wrap:wrap;justify-content:center;'>\n");
    printf("<a class='nav-back' href='?action=studio'>📸 Make a new picture</a>\n");
    printf("<a class='nav-back' href='?'>← Back to Main Menu</a>\n");
    printf("</p>\n");
    printf("</div>\n");

    printf("<script>\n");
    printf("(function(){\n");
    printf("var g=document.getElementById('gal'),st=document.getElementById('galStatus');\n");
    printf("if(!g)return;\n");
    printf("var BASE=(location.pathname==='/'||location.pathname==='')?'':location.pathname.replace(/\\/$/,'');\n");
    printf("function card(p){\n");
    printf("  var d=document.createElement('div');\n");
    printf("  d.style.cssText='background:rgba(255,255,255,.06);border:1px solid rgba(255,255,255,.1);border-radius:12px;overflow:hidden;';\n");
    printf("  var a=document.createElement('a');\n");
    printf("  a.href=BASE+'/pictures/'+encodeURIComponent(p.file);a.target='_blank';a.rel='noopener';\n");
    printf("  var img=document.createElement('img');\n");
    printf("  img.src=a.href;img.alt=p.caption||'Saved picture';img.loading='lazy';\n");
    printf("  img.style.cssText='width:100%%;height:180px;object-fit:cover;display:block;background:#12162e;';\n");
    printf("  a.appendChild(img);d.appendChild(a);\n");
    printf("  var meta=document.createElement('div');meta.style.cssText='padding:10px 12px;font-size:13px;color:#c8d2ff;';\n");
    printf("  var cap=document.createElement('div');cap.textContent=p.caption||'Untitled';\n");
    printf("  cap.style.cssText='color:#ffd700;font-weight:600;margin-bottom:4px;overflow:hidden;text-overflow:ellipsis;white-space:nowrap;';\n");
    printf("  var when=document.createElement('div');\n");
    printf("  when.textContent=(p.created_at||'')+' · '+Math.max(1,Math.round((p.bytes||0)/1024))+' KB';\n");
    printf("  when.style.opacity='.75';\n");
    printf("  meta.appendChild(cap);meta.appendChild(when);d.appendChild(meta);\n");
    printf("  var del=document.createElement('button');del.type='button';del.className='btn alt';del.textContent='🗑 Delete';\n");
    printf("  del.style.cssText='margin:0 12px 12px;padding:6px 14px;font-size:13px;';\n");
    printf("  del.addEventListener('click',function(){\n");
    printf("    if(!confirm('Delete this picture from the server?'))return;\n");
    printf("    del.disabled=true;\n");
    printf("    fetch(BASE+'/api/pictures/'+p.id,{method:'DELETE'}).then(function(r){return r.json();})\n");
    printf("      .then(function(){load();}).catch(function(){del.disabled=false;st.textContent='⚠ Could not delete that picture.';});\n");
    printf("  });\n");
    printf("  d.appendChild(del);\n");
    printf("  return d;\n");
    printf("}\n");
    printf("function load(){\n");
    printf("  fetch(BASE+'/api/pictures').then(function(r){return r.json();})\n");
    printf("    .then(function(d){\n");
    printf("      var list=d.pictures||[];g.innerHTML='';\n");
    printf("      if(!list.length){st.textContent='No pictures yet — make one in the studio. 📸';return;}\n");
    printf("      st.textContent=list.length+(list.length===1?' picture':' pictures')+' saved on this server';\n");
    printf("      list.forEach(function(p){g.appendChild(card(p));});\n");
    printf("    })\n");
    printf("    .catch(function(){st.textContent='⚠ Could not reach the picture database.';});\n");
    printf("}\n");
    printf("load();\n");
    printf("})();\n");
    printf("</script>\n");
}

void show_world_section() {
    printf("<div class='panel'>\n");

    printf("<div id='wmBox'>\n");
    printf("<div id='wmLang'>🇬🇧 English</div>\n");
    printf("<div id='wmText'>Merry Christmas!</div>\n");
    printf("<div id='wmCount'>1 / 1</div>\n");
    printf("</div>\n");

    printf("<div class='studio-row' style='justify-content:center;margin-top:14px;'>\n");
    printf("<button class='btn alt' type='button' id='wmPause' title='Pause / play'>⏸</button>\n");
    printf("</div>\n");
    printf("<p id='wmStatus' style='text-align:center;font-size:14px;color:#c8d2ff;min-height:20px;'>Loading messages…</p>\n");

    printf("<div class='studio-row' style='justify-content:center;'>\n");
    printf("<input id='wmInput' type='text' maxlength='140' placeholder='Add a message…' "
           "style='flex:1 1 320px;max-width:440px;padding:11px 14px;border-radius:10px;border:1px solid rgba(255,255,255,.25);background:rgba(255,255,255,.92);color:#223;'\n");
    printf(">\n");
    printf("<button class='btn' type='button' id='wmAdd'>➕ Add</button>\n");
    printf("</div>\n");
    printf("</div>\n");

    printf("<script>\n");
    printf("(function(){\n");
    printf("if(window.__worldStop){window.__worldStop();}\n");
    printf("var baseEl=document.getElementById('wmText'),langEl=document.getElementById('wmLang'),cntEl=document.getElementById('wmCount'),stEl=document.getElementById('wmStatus');\n");
    printf("if(!baseEl)return;\n");
    printf("var BASE=(location.pathname==='/'||location.pathname==='')?'':location.pathname.replace(/\\/$/,'');\n");

    /* Built-in translations (instant, offline fallback) */
    printf("var BUILTIN=[\n");
    printf("{base:'Merry Christmas!',lines:[\n");
    printf("['🇬🇧','English','Merry Christmas!'],['🇫🇷','French','Joyeux Noël !'],['🇪🇸','Spanish','¡Feliz Navidad!'],\n");
    printf("['🇩🇪','German','Frohe Weihnachten!'],['🇮🇹','Italian','Buon Natale!'],['🇵🇹','Portuguese','Feliz Natal!'],\n");
    printf("['🇳🇱','Dutch','Fijne Kerst!'],['🇸🇦','Arabic','عيد ميلاد مجيد!'],['🇷🇺','Russian','С Рождеством!'],\n");
    printf("['🇨🇳','Chinese','圣诞快乐！'],['🇯🇵','Japanese','メリークリスマス！'],['🇰🇷','Korean','메리 크리스마스!'],\n");
    printf("['🇮🇳','Hindi','मेरी क्रिसमस!'],['🇰🇪','Swahili','Krismasi njema!']]},\n");
    printf("{base:'Wishing you love, joy and peace.',lines:[\n");
    printf("['🇬🇧','English','Wishing you love, joy and peace.'],['🇫🇷','French','Je vous souhaite de l’amour, de la joie et la paix.'],\n");
    printf("['🇪🇸','Spanish','Les deseo amor, alegría y paz.'],['🇩🇪','German','Ich wünsche euch Liebe, Freude und Frieden.'],\n");
    printf("['🇮🇹','Italian','Vi auguro amore, gioia e pace.'],['🇵🇹','Portuguese','Desejo-lhe amor, alegria e paz.'],\n");
    printf("['🇳🇱','Dutch','Ik wens je liefde, vreugde en vrede.'],['🇸🇦','Arabic','أتمنى لكم المحبة والفرح والسلام.'],\n");
    printf("['🇷🇺','Russian','Желаю любви, радости и мира.'],['🇨🇳','Chinese','祝你爱、欢乐与平安。'],\n");
    printf("['🇯🇵','Japanese','愛と喜び、そして安らぎを。'],['🇰🇷','Korean','사랑과 기쁨, 평화를 기원합니다.'],\n");
    printf("['🇮🇳','Hindi','आपको प्यार, खुशी और शांति की शुभकामनाएँ।'],['🇰🇪','Swahili','Nakutakia upendo, furaha na amani.']]},\n");
    printf("{base:'Let it snow, sparkle and glow!',lines:[\n");
    printf("['🇬🇧','English','Let it snow, sparkle and glow!'],['🇫🇷','French','Que la neige scintille et brille !'],\n");
    printf("['🇪🇸','Spanish','¡Que nieve, que brille y que resplandezca!'],['🇩🇪','German','Es schneie, es funkele, es leuchte!'],\n");
    printf("['🇮🇹','Italian','Che nevi, che scintilli e che brilli!'],['🇵🇹','Portuguese','Que neve, que brilhe e que refulja!'],\n");
    printf("['🇳🇱','Dutch','Laat het sneeuwen, fonkelen en gloren!'],['🇸🇦','Arabic','ليتساقط الثلج ويلمع ويتوهج!'],\n");
    printf("['🇷🇺','Russian','Пусть снег искрится и сияет!'],['🇨🇳','Chinese','飘雪吧，闪耀吧，发光吧！'],\n");
    printf("['🇯🇵','Japanese','雪が降って、きらめいて、光れ！'],['🇰🇷','Korean','눈이 내리고, 반짝이고, 빛나자!'],\n");
    printf("['🇮🇳','Hindi','बर्फ गिरे, चमके और रोशन हो!'],['🇰🇪','Swahili','Mvua ya theluji, angaze na angaze!']]}\n");
    printf("];\n");

    printf("var FLAGMAP={'english':'🇬🇧','french':'🇫🇷','spanish':'🇪🇸','german':'🇩🇪','italian':'🇮🇹','portuguese':'🇵🇹','dutch':'🇳🇱','swahili':'🇰🇪','hindi':'🇮🇳','japanese':'🇯🇵','korean':'🇰🇷','arabic':'🇸🇦','russian':'🇷🇺','chinese':'🇨🇳'};\n");
    printf("var LANGMAP={'english':'English','french':'French','spanish':'Spanish','german':'German','italian':'Italian','portuguese':'Portuguese','dutch':'Dutch','swahili':'Swahili','hindi':'Hindi','japanese':'Japanese','korean':'Korean','arabic':'Arabic','russian':'Russian','chinese':'Chinese'};\n");

    printf("var msgs=BUILTIN.slice(),items=[],idx=0,paused=false,tick=0,poll=0;\n");

    printf("function rebuild(){\n");
    printf("  items=[];\n");
    printf("  msgs.forEach(function(m){\n");
    printf("    if(!m.custom){m.lines.forEach(function(l){items.push({flag:l[0],lang:l[1],text:l[2]});});return;}\n");
    printf("    if(!m.done||!m.tr||Object.keys(m.tr).length===0){items.push({flag:'⌛',lang:'Translating with the llama…',text:m.base,pending:true});return;}\n");
    printf("    items.push({flag:'🇬🇧',lang:'English',text:m.base});\n");
    printf("    Object.keys(m.tr).forEach(function(k){items.push({flag:FLAGMAP[k]||'🌍',lang:(LANGMAP[k]||k),text:m.tr[k]});});\n");
    printf("  });\n");
    printf("  if(idx>=items.length)idx=0;\n");
    printf("  render();\n");
    printf("}\n");

    printf("function render(){\n");
    printf("  var it=items[idx]||{flag:'🎄',lang:'…',text:'Merry Christmas!'};\n");
    printf("  baseEl.textContent=it.text;\n");
    printf("  langEl.textContent=it.flag+'   '+it.lang;\n");
    printf("  cntEl.textContent=(idx+1)+' / '+items.length;\n");
    printf("  baseEl.classList.remove('pop');void baseEl.offsetWidth;baseEl.classList.add('pop');\n");
    printf("}\n");

    printf("function step(){if(!paused&&items.length){idx=(idx+1)%%items.length;render();}}\n");
    printf("tick=setInterval(step,2800);\n");

    printf("function hasPending(){return msgs.some(function(m){return m.custom&&!m.done;});}\n");
    printf("function load(){\n");
    printf("  fetch(BASE+'/api/phrases').then(function(r){return r.json();}).then(function(d){\n");
    printf("    msgs=BUILTIN.slice();\n");
    printf("    (d.phrases||[]).forEach(function(p){msgs.push({base:p.text,custom:true,tr:p.translations||{},done:!!p.done});});\n");
    printf("    rebuild();\n");
    printf("    if(hasPending())stEl.textContent=\"🎅 Santa's elves are still translating your message…\";\n");
    printf("    else if(msgs.some(function(m){return m.custom;}))stEl.textContent='✅ Every message is translated and looping!';\n");
    printf("    else stEl.textContent='Tip: add your own message below — the local llama will translate it into '+((d.langs||[]).length)+' languages.';\n");
    printf("  }).catch(function(){stEl.textContent='⚠ Could not reach the message server.';});\n");
    printf("}\n");
    printf("poll=setInterval(function(){if(hasPending())load();},6000);\n");
    printf("load();\n");

    printf("var btn=document.getElementById('wmAdd'),inp=document.getElementById('wmInput');\n");
    printf("function add(){\n");
    printf("  var v=(inp.value||'').trim();\n");
    printf("  if(!v){stEl.textContent='Type a message first 🎁';return;}\n");
    printf("  if(v.length>140)v=v.slice(0,140);\n");
    printf("  btn.disabled=true;stEl.textContent='🎄 Sending it to the llama…';\n");
    printf("  fetch(BASE+'/api/phrases',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({text:v})})\n");
    printf("    .then(function(r){return r.json();})\n");
    printf("    .then(function(){inp.value='';load();})\n");
    printf("    .catch(function(){stEl.textContent='⚠ Could not add the message.';})\n");
    printf("    .then(function(){btn.disabled=false;});\n");
    printf("}\n");
    printf("btn.addEventListener('click',add);\n");
    printf("inp.addEventListener('keydown',function(e){if(e.key==='Enter')add();});\n");

    printf("var pb=document.getElementById('wmPause');\n");
    printf("pb.addEventListener('click',function(){paused=!paused;pb.textContent=paused?'▶':'⏸';});\n");
    printf("window.__worldStop=function(){clearInterval(tick);clearInterval(poll);};\n");
    printf("})();\n");
    printf("</script>\n");
}
void show_story_section() {
    printf("<div class='panel story-panel'>\n");
    printf("<div class='story-stage' id='storyStage'>\n");
    printf("<canvas id='scene' width='1280' height='720'></canvas>\n");
    printf("<div class='story-bar'>\n");
    printf("<button class='btn alt sc' type='button' data-sc='village' title='Village Night'>🏘</button>\n");
    printf("<button class='btn alt sc' type='button' data-sc='aurora' title='Aurora Forest'>🌌</button>\n");
    printf("<button class='btn alt sc' type='button' data-sc='fire' title='Fireside'>🔥</button>\n");
    printf("<button class='btn alt sc' type='button' data-sc='sky' title='Night Sky'>🌙</button>\n");
    printf("<button class='btn alt' type='button' id='pauseBtn' title='Pause / play'>⏸</button>\n");
    printf("<button class='btn alt' type='button' id='tourBtn' title='Auto tour'>🎬</button>\n");
    printf("<button class='btn alt' type='button' id='fsBtn' title='Fullscreen'>⛶</button>\n");
    printf("</div>\n");
    printf("<div id='cap'></div>\n");
    printf("</div>\n");
    printf("</div>\n");

    printf("<script>\n");
    printf("(function(){\n");
    printf("if(window.__storyStop){window.__storyStop();}\n");
    printf("var cvs=document.getElementById('scene');if(!cvs)return;\n");
    printf("var ctx=cvs.getContext('2d'),W=1280,H=720,T=0,last=performance.now(),raf=0;\n");
    printf("var scene='village',playing=true,auto=false,sceneAge=0,capTimer=0,spawnT=0,emberT=0,santaT=0,santaOn=false;\n");

    printf("var stars=[],snowflakes=[],pines=[[],[],[]],smoke=[],embers=[],vill=[],capEl=document.getElementById('cap');\n");
    printf("function R(a,b){return a+Math.random()*(b-a);}\n");
    printf("for(var i=0;i<180;i++)stars.push({x:R(0,W),y:R(0,H*0.72),r:R(0.4,1.8),ph:R(0,6.28),sp:R(0.6,2.4)});\n");
    printf("for(i=0;i<240;i++)snowflakes.push({x:R(0,W),y:R(0,H),r:R(0.5,2.5),sp:R(22,70),ph:R(0,6.28),dr:R(6,26),a:R(0.3,0.95)});\n");
    printf("function mkPines(li,n,yb,hmin,hmax){var a=pines[li];for(var j=0;j<n;j++)a.push({x:R(-60,W+60),y:yb+R(-14,14),h:R(hmin,hmax)});}\n");
    printf("mkPines(0,30,470,50,110);mkPines(1,22,545,90,175);mkPines(2,16,625,130,250);\n");
    printf("for(i=0;i<12;i++)smoke.push({x:0,y:0,life:0,r:6,vx:0});\n");
    printf("for(i=0;i<26;i++)embers.push({x:0,y:0,life:0,vx:0,vy:0});\n");
    printf("for(i=0;i<26;i++)vill.push({x:R(60,W-60),y:R(640,700),ph:R(0,6.28)});\n");

    printf("var caps=['Snow hushed the village, and every window glowed like a small amber moon.',"
           "'The old fir kept its secrets under a blanket of white, waiting for midnight.',"
           "'Somewhere a kettle sang, and boot-stamps circled the gate like a wooden crown.',"
           "'Letters to Santa lay folded on the sill, ink frozen into loops of hope.',"
           "'The fire whispered in the dark: rest now, the year has been long enough.',"
           "'By dawn the world was sugar, and footprints led away toward the hills.'];\n");
    printf("var capI=0;capEl.textContent=caps[0];capEl.style.opacity=1;\n");
    printf("function capFade(){capEl.style.opacity=0;setTimeout(function(){capI=(capI+1)%%caps.length;capEl.textContent=caps[capI];capEl.style.opacity=1;},1500);}\n");
    printf("capTimer=setInterval(capFade,17000);\n");

    printf("function starsDraw(){for(var k=0;k<stars.length;k++){var s=stars[k];var a=0.35+0.65*Math.abs(Math.sin(T*s.sp+s.ph));ctx.fillStyle='rgba(255,255,255,'+a.toFixed(2)+')';ctx.beginPath();ctx.arc(s.x,s.y,s.r,0,6.283);ctx.fill();}}\n");
    printf("function moonDraw(mx,my,mr){ctx.save();var g=ctx.createRadialGradient(mx,my,mr*0.2,mx,my,mr*3);g.addColorStop(0,'rgba(255,244,214,0.55)');g.addColorStop(1,'rgba(255,244,214,0)');ctx.fillStyle=g;ctx.beginPath();ctx.arc(mx,my,mr*3,0,6.283);ctx.fill();ctx.fillStyle='#fff6d8';ctx.beginPath();ctx.arc(mx,my,mr,0,6.283);ctx.fill();ctx.fillStyle='rgba(214,200,160,0.5)';ctx.beginPath();ctx.arc(mx-mr*0.3,my-mr*0.15,mr*0.22,0,6.283);ctx.arc(mx+mr*0.25,my+mr*0.3,mr*0.16,0,6.283);ctx.arc(mx+mr*0.1,my-mr*0.45,mr*0.12,0,6.283);ctx.fill();ctx.restore();}\n");
    printf("function snowDraw(dt,slow){for(var k=0;k<snowflakes.length;k++){var f=snowflakes[k];f.y+=f.sp*dt*(slow?0.55:1);f.x+=Math.sin(T*1.1+f.ph)*f.dr*dt;if(f.y>H+6){f.y=-6;f.x=R(0,W);}if(f.x<-8)f.x=W+8;if(f.x>W+8)f.x=-8;ctx.globalAlpha=f.a;ctx.fillStyle='#fff';ctx.beginPath();ctx.arc(f.x,f.y,f.r,0,6.283);ctx.fill();}ctx.globalAlpha=1;}\n");
    printf("function pineDraw(x,y,h,c){ctx.fillStyle=c;ctx.beginPath();ctx.moveTo(x,y-h);ctx.lineTo(x-h*0.44,y);ctx.lineTo(x+h*0.44,y);ctx.closePath();ctx.fill();ctx.fillRect(x-h*0.05,y-1,h*0.10,h*0.13);ctx.strokeStyle='rgba(255,255,255,0.45)';ctx.lineWidth=Math.max(1,h*0.028);ctx.beginPath();ctx.moveTo(x-h*0.26,y-h*0.20);ctx.lineTo(x-h*0.04,y-h*0.27);ctx.stroke();}\n");
    printf("function driftPines(dt,sp){for(var li=0;li<3;li++){var a=pines[li];for(var k=0;k<a.length;k++){a[k].x-=sp*(0.5+li*0.5)*dt;if(a[k].x<-140)a[k].x=W+140;}}}\n");

    printf("function santaDraw(x,y,sc){ctx.save();ctx.translate(x,y);ctx.scale(sc,sc);ctx.fillStyle='rgba(255,90,90,0.95)';ctx.beginPath();ctx.arc(112,0,5,0,6.283);ctx.fill();ctx.fillStyle='#10121e';ctx.strokeStyle='#10121e';ctx.lineWidth=2;ctx.beginPath();ctx.moveTo(-20,-1);ctx.lineTo(52,-1);ctx.stroke();for(var k=0;k<3;k++){var rx=k*34;ctx.fillRect(rx-13,-6,26,9);ctx.fillRect(rx-11,3,4,15);ctx.fillRect(rx+7,3,4,15);ctx.fillRect(rx+9,-15,7,11);ctx.beginPath();ctx.moveTo(rx+12,-19);ctx.lineTo(rx+9,-26);ctx.moveTo(rx+12,-19);ctx.lineTo(rx+15,-26);ctx.stroke();}ctx.beginPath();ctx.moveTo(-74,2);ctx.lineTo(-18,2);ctx.lineTo(-24,16);ctx.lineTo(-66,16);ctx.closePath();ctx.fill();ctx.beginPath();ctx.moveTo(-66,18);ctx.lineTo(-86,18);ctx.quadraticCurveTo(-90,2,-74,2);ctx.stroke();ctx.beginPath();ctx.arc(-44,-9,8,0,6.283);ctx.fill();ctx.beginPath();ctx.moveTo(-44,-17);ctx.lineTo(-57,-24);ctx.lineTo(-31,-24);ctx.closePath();ctx.fill();ctx.restore();}\n");

    printf("function drawVillage(dt){\n");
    printf("  var g=ctx.createLinearGradient(0,0,0,H);g.addColorStop(0,'#04081f');g.addColorStop(0.55,'#101a4a');g.addColorStop(1,'#2b3775');ctx.fillStyle=g;ctx.fillRect(0,0,W,H);\n");
    printf("  starsDraw();moonDraw(1040,120,46);\n");
    printf("  ctx.fillStyle='#0a1030';ctx.beginPath();ctx.moveTo(0,505);for(var x=0;x<=W;x+=36)ctx.lineTo(x,505-26*Math.sin(x*0.006+1.7)-15*Math.sin(x*0.014+0.4));ctx.lineTo(W,H);ctx.lineTo(0,H);ctx.fill();\n");
    printf("  driftPines(dt,7);\n");
    printf("  var cols=['#0a1c12','#0d2417','#10301d'];\n");
    printf("  for(var li=0;li<3;li++){var a=pines[li];for(var k=0;k<a.length;k++)pineDraw(a[k].x,a[k].y,a[k].h,cols[li]);}\n");
    printf("  ctx.fillStyle='#dfe7f5';ctx.beginPath();ctx.moveTo(0,640);for(x=0;x<=W;x+=40)ctx.lineTo(x,640+9*Math.sin(x*0.004));ctx.lineTo(W,H);ctx.lineTo(0,H);ctx.fill();\n");
    printf("  var bx=210,by=470,bw=250,bh=150;\n");
    printf("  ctx.fillStyle='#5d3d26';ctx.fillRect(bx,by,bw,bh);ctx.fillStyle='rgba(0,0,0,0.14)';ctx.fillRect(bx,by+bh*0.55,bw,bh*0.45);\n");
    printf("  ctx.fillStyle='#3a2416';ctx.beginPath();ctx.moveTo(bx-34,by);ctx.lineTo(bx+bw/2,by-105);ctx.lineTo(bx+bw+34,by);ctx.closePath();ctx.fill();\n");
    printf("  ctx.fillStyle='#eef4ff';ctx.beginPath();ctx.moveTo(bx-34,by);ctx.lineTo(bx+bw/2,by-105);ctx.lineTo(bx+bw+34,by);ctx.lineTo(bx+bw+34,by+12);ctx.lineTo(bx+bw/2,by-84);ctx.lineTo(bx-34,by+12);ctx.closePath();ctx.fill();\n");
    printf("  ctx.fillStyle='#4a4a55';ctx.fillRect(bx+bw-74,by-136,36,64);\n");
    printf("  var fl=(0.72+0.2*Math.sin(T*2.4)+0.08*Math.sin(T*9.7)).toFixed(2);\n");
    printf("  ctx.fillStyle='rgba(255,198,84,'+fl+')';ctx.fillRect(bx+28,by+40,58,54);ctx.fillRect(bx+bw-86,by+40,58,54);\n");
    printf("  ctx.fillStyle='rgba(0,0,0,0.35)';ctx.fillRect(bx+55,by+40,4,54);ctx.fillRect(bx+28,by+64,58,4);ctx.fillRect(bx+bw-59,by+40,4,54);ctx.fillRect(bx+bw-86,by+64,58,4);\n");
    printf("  ctx.fillStyle='#3a2416';ctx.fillRect(bx+bw/2-27,by+76,54,74);ctx.fillStyle='#ffd700';ctx.beginPath();ctx.arc(bx+bw/2+14,by+112,4,0,6.283);ctx.fill();\n");
    printf("  ctx.fillStyle='#f4f7ff';ctx.fillRect(bx-30,by+bh,90,12);ctx.fillRect(bx+bw-10,by+bh,70,12);\n");
    printf("  spawnT+=dt;if(spawnT>0.5){spawnT=0;for(var s=0;s<smoke.length;s++){if(smoke[s].life<=0){smoke[s].x=bx+bw-56+R(-6,6);smoke[s].y=by-138;smoke[s].life=R(3,4.5);smoke[s].r=R(6,10);smoke[s].vx=R(9,20);break;}}}\n");
    printf("  for(s=0;s<smoke.length;s++){var p=smoke[s];if(p.life<=0)continue;p.life-=dt;p.y-=26*dt;p.x+=p.vx*dt+Math.sin(T*2+s)*7*dt;p.r+=9*dt;ctx.globalAlpha=Math.max(0,p.life/4.5*0.4);ctx.fillStyle='#cfd6e8';ctx.beginPath();ctx.arc(p.x,p.y,p.r,0,6.283);ctx.fill();}\n");
    printf("  ctx.globalAlpha=1;\n");
    printf("  santaT+=dt;if(santaT>42&&!santaOn){santaOn=true;santaT=0;}if(santaOn){var pr=(sceneAge%%14)/14;var sx=-160+pr*(W+320);var sy=110+Math.sin(pr*6.28)*36;santaDraw(sx,sy,1.1);if(pr>=1){santaOn=false;}}\n");
    printf("  snowDraw(dt,false);\n");
    printf("}\n");

    printf("function drawAurora(dt){\n");
    printf("  var g=ctx.createLinearGradient(0,0,0,H);g.addColorStop(0,'#02040f');g.addColorStop(1,'#071627');ctx.fillStyle=g;ctx.fillRect(0,0,W,H);\n");
    printf("  starsDraw();\n");
    printf("  ctx.globalCompositeOperation='lighter';\n");
    printf("  for(var k=0;k<4;k++){ctx.beginPath();for(var x=0;x<=W;x+=18){var y=150+k*52+Math.sin(x*0.007+T*0.32+k*1.7)*58+Math.sin(x*0.017-T*0.21+k)*26;if(x===0)ctx.moveTo(x,y);else ctx.lineTo(x,y);}var hue=Math.round(125+k*26+Math.sin(T*0.16+k)*32);ctx.strokeStyle='hsla('+hue+',85%%,60%%,0.18)';ctx.lineWidth=54-k*7;ctx.stroke();}\n");
    printf("  ctx.globalCompositeOperation='source-over';\n");
    printf("  ctx.fillStyle='#04120c';ctx.beginPath();ctx.moveTo(0,560);for(x=0;x<=W;x+=40)ctx.lineTo(x,560-40*Math.abs(Math.sin(x*0.004+2.1)));ctx.lineTo(W,H);ctx.lineTo(0,H);ctx.fill();\n");
    printf("  driftPines(dt,4);\n");
    printf("  for(var li=0;li<3;li++){var a=pines[li];for(var k2=0;k2<a.length;k2++)pineDraw(a[k2].x,a[k2].y,a[k2].h,'#04140d');}\n");
    printf("  snowDraw(dt,true);\n");
    printf("}\n");

    printf("function drawFire(dt){\n");
    printf("  var g=ctx.createLinearGradient(0,0,0,H);g.addColorStop(0,'#2c1a14');g.addColorStop(1,'#160c09');ctx.fillStyle=g;ctx.fillRect(0,0,W,H);\n");
    printf("  ctx.fillStyle='#3b2418';ctx.fillRect(0,H-150,W,150);\n");
    printf("  ctx.fillStyle='rgba(255,255,255,0.05)';for(var b=0;b<8;b++)ctx.fillRect(0,H-150+b*19,W,2);\n");
    printf("  var wx=70,wy=170,ww=210,wh=250;\n");
    printf("  ctx.fillStyle='#1b2440';ctx.fillRect(wx,wy,ww,wh);ctx.fillStyle='#dfe7f5';ctx.fillRect(wx-10,wy-10,ww+20,wh+20);ctx.fillStyle='#1b2440';ctx.fillRect(wx,wy,ww,wh);\n");
    printf("  ctx.fillStyle='#0a1030';ctx.fillRect(wx+6,wy+6,ww-12,wh-12);\n");
    printf("  for(var s=0;s<40;s++){var sx=wx+8+((s*97+T*26)%%(ww-16));var sy=wy+8+((s*61+T*40)%%(wh-16));ctx.globalAlpha=0.7;ctx.fillStyle='#fff';ctx.fillRect(sx,sy,2,2);}ctx.globalAlpha=1;\n");
    printf("  ctx.strokeStyle='#dfe7f5';ctx.lineWidth=10;ctx.beginPath();ctx.moveTo(wx+ww/2,wy);ctx.lineTo(wx+ww/2,wy+wh);ctx.moveTo(wx,wy+wh/2);ctx.lineTo(wx+ww,wy+wh/2);ctx.stroke();\n");
    printf("  var fx=W/2-210,fy=H-380,fw=420,fh=380;\n");
    printf("  ctx.fillStyle='#57534e';ctx.fillRect(fx,fy,fw,fh);\n");
    printf("  ctx.fillStyle='rgba(0,0,0,0.25)';for(var r=0;r<5;r++)for(var c2=0;c2<6;c2++)ctx.strokeRect(fx+c2*(fw/6),fy+r*(fh/5),fw/6,fh/5);\n");
    printf("  ctx.fillStyle='#150b07';ctx.fillRect(fx+50,fy+70,fw-100,fh-70);\n");
    printf("  ctx.fillStyle='#4a3226';ctx.fillRect(fx+70,fy+fh-64,fw-140,34);\n");
    printf("  ctx.fillStyle='#6b4a2f';ctx.fillRect(fx-44,fy-52,fw+88,52);\n");
    printf("  var cx=fx+fw/2,base=fy+fh-70;\n");
    printf("  ctx.fillStyle='#3a2416';for(var l=0;l<3;l++){var lx=cx-70+l*70;ctx.save();ctx.translate(lx,base+10);ctx.rotate(l%%2===0?0.18:-0.18);ctx.fillRect(-52,-13,104,26);ctx.restore();}\n");
    printf("  ctx.globalCompositeOperation='lighter';\n");
    printf("  for(var f=0;f<3;f++){var fh2=(120-f*32)*(1+Math.sin(T*7+f*2)*0.12);var fw2=76-f*17;var sway=Math.sin(T*5+f)*12;var cols=['rgba(255,110,20,0.5)','rgba(255,186,44,0.55)','rgba(255,240,150,0.65)'];ctx.fillStyle=cols[f];ctx.beginPath();ctx.moveTo(cx-fw2/2,base);ctx.quadraticCurveTo(cx-fw2*0.5,base-fh2*0.55,cx+sway,base-fh2);ctx.quadraticCurveTo(cx+fw2*0.5,base-fh2*0.55,cx+fw2/2,base);ctx.closePath();ctx.fill();}\n");
    printf("  var rg=ctx.createRadialGradient(cx,base-60,12,cx,base-60,420);rg.addColorStop(0,'rgba(255,160,44,0.30)');rg.addColorStop(1,'rgba(255,120,0,0)');ctx.fillStyle=rg;ctx.fillRect(0,0,W,H);\n");
    printf("  ctx.globalCompositeOperation='source-over';\n");
    printf("  emberT+=dt;if(emberT>0.28){emberT=0;for(var e=0;e<embers.length;e++){if(embers[e].life<=0){embers[e].x=cx+R(-40,40);embers[e].y=base-30;embers[e].life=R(1,2.2);embers[e].vx=R(-14,14);embers[e].vy=R(-70,-30);break;}}}\n");
    printf("  for(e=0;e<embers.length;e++){var m=embers[e];if(m.life<=0)continue;m.life-=dt;m.x+=m.vx*dt+Math.sin(T*3+e)*10*dt;m.y+=m.vy*dt;ctx.globalAlpha=Math.max(0,m.life/2.2);ctx.fillStyle='rgba(255,170,60,1)';ctx.beginPath();ctx.arc(m.x,m.y,2.4,0,6.283);ctx.fill();}ctx.globalAlpha=1;\n");
    printf("  ctx.fillStyle='#8b0000';for(var st=0;st<3;st++){var stx=fx+60+st*130;ctx.fillRect(stx,fy-6,44,90);ctx.fillStyle='#f4f7ff';ctx.fillRect(stx,fy+70,44,14);ctx.fillStyle='#f4f7ff';ctx.fillRect(stx+8,fy-6,28,16);ctx.fillStyle='#8b0000';}\n");
    printf("  ctx.fillStyle='#12331f';ctx.fillRect(fx-44,fy-72,fw+88,20);\n");
    printf("  var lc=['#ff5252','#4ecdc4','#ffd700','#9b59b6'];for(var q=0;q<22;q++){var qx=fx-40+q*((fw+80)/21);var lit=0.55+0.45*Math.sin(T*2.2+q*1.3);ctx.globalAlpha=Math.max(0.25,lit);ctx.fillStyle=lc[q%%4];ctx.beginPath();ctx.arc(qx,fy-64,6,0,6.283);ctx.fill();}ctx.globalAlpha=1;\n");
    printf("  snowDraw(dt,true);\n");
    printf("}\n");

    printf("function drawSky(dt){\n");
    printf("  var g=ctx.createLinearGradient(0,0,0,H);g.addColorStop(0,'#020418');g.addColorStop(0.6,'#0b1440');g.addColorStop(1,'#1d2a66');ctx.fillStyle=g;ctx.fillRect(0,0,W,H);\n");
    printf("  starsDraw();moonDraw(320,170,64);\n");
    printf("  ctx.fillStyle='rgba(140,150,200,0.10)';for(var c3=0;c3<5;c3++){var cx2=((c3*310+T*9)%%(W+400))-200;ctx.beginPath();ctx.ellipse(cx2,210+c3*60,150,34,0,0,6.283);ctx.fill();}\n");
    printf("  ctx.fillStyle='#060a1c';ctx.beginPath();ctx.moveTo(0,600);for(var x=0;x<=W;x+=44)ctx.lineTo(x,600-34*Math.sin(x*0.005+0.8)-18*Math.sin(x*0.011));ctx.lineTo(W,H);ctx.lineTo(0,H);ctx.fill();\n");
    printf("  for(var v=0;v<vill.length;v++){var h=vill[v];var tw=0.5+0.5*Math.sin(T*1.6+h.ph);ctx.globalAlpha=0.35+tw*0.65;ctx.fillStyle='#ffc454';ctx.fillRect(h.x,h.y,7,9);}ctx.globalAlpha=1;\n");
    printf("  driftPines(dt,5);\n");
    printf("  for(var li=0;li<3;li++){var a=pines[li];for(var k=0;k<a.length;k++)pineDraw(a[k].x,a[k].y,a[k].h,'#050b16');}\n");
    printf("  santaT+=dt;if(santaT>18&&!santaOn){santaOn=true;santaT=0;}if(santaOn){var pr=(sceneAge%%11)/11;var sx=-160+pr*(W+320);var sy=250+Math.sin(pr*6.28)*60;santaDraw(sx,sy,1.3);if(pr>=1){santaOn=false;}}\n");
    printf("  snowDraw(dt,false);\n");
    printf("}\n");

    printf("var scenes={village:drawVillage,aurora:drawAurora,fire:drawFire,sky:drawSky};\n");
    printf("function syncBtns(){var bs=document.querySelectorAll('.sc');for(var k=0;k<bs.length;k++){var on=bs[k].getAttribute('data-sc')===scene;bs[k].className=on?'btn sc':'btn alt sc';}}\n");
    printf("function loop(now){var dt=Math.min(0.05,(now-last)/1000);last=now;if(playing){T+=dt;sceneAge+=dt;}scenes[scene](playing?dt:0);if(auto&&sceneAge>120){sceneAge=0;var ks=Object.keys(scenes);scene=ks[(ks.indexOf(scene)+1)%%ks.length];syncBtns();}raf=requestAnimationFrame(loop);}\n");
    printf("var bs=document.querySelectorAll('.sc');for(var k=0;k<bs.length;k++){bs[k].addEventListener('click',function(){scene=this.getAttribute('data-sc');sceneAge=0;santaT=0;syncBtns();});}\n");
    printf("var pb=document.getElementById('pauseBtn');pb.addEventListener('click',function(){playing=!playing;pb.textContent=playing?'⏸':'▶';});\n");
    printf("var tb=document.getElementById('tourBtn');tb.addEventListener('click',function(){auto=!auto;tb.classList.toggle('on',auto);});\n");
    printf("var fsb=document.getElementById('fsBtn');var stg=document.getElementById('storyStage');\n");
    printf("fsb.addEventListener('click',function(){if(document.fullscreenElement){document.exitFullscreen();}else if(stg.requestFullscreen){stg.requestFullscreen();}});\n");
    printf("syncBtns();raf=requestAnimationFrame(loop);\n");
    printf("window.__storyStop=function(){cancelAnimationFrame(raf);clearInterval(capTimer);};\n");
    printf("})();\n");
    printf("</script>\n");
}
void show_about_section() {
    printf("<div class='panel'>\n");
    printf("<h2>ℹ️ About Santa's Christmas Mini Market</h2>\n");

    printf("<div style='background: rgba(255,255,255,0.9); color: #333; padding: 20px; border-radius: 10px; margin: 20px 0;'>\n");
    printf("<h3>🎄 What is this?</h3>\n");
    printf("<p>This is a festive web application built entirely in pure C using CGI (Common Gateway Interface). No frameworks, no JavaScript libraries - just clean C code generating beautiful HTML, CSS, and JavaScript!</p>\n");

    printf("<h3>🛠️ Technical Details</h3>\n");
    printf("<ul>\n");
    printf("<li><strong>Language:</strong> Pure C (C99 standard)</li>\n");
    printf("<li><strong>Web Protocol:</strong> CGI for server-side processing</li>\n");
    printf("<li><strong>Frontend:</strong> HTML5 + CSS3 + vanilla JavaScript</li>\n");
    printf("<li><strong>Server:</strong> Any CGI-capable web server</li>\n");
    printf("<li><strong>Dependencies:</strong> None (stdlib only)</li>\n");
    printf("</ul>\n");

    printf("<h3>🎯 Features</h3>\n");
    printf("<ul>\n");
    printf("<li>🎄 <strong>Christmas Tree Generator:</strong> Custom ASCII trees with ornaments</li>\n");
    printf("<li>💌 <strong>Holiday Card Creator:</strong> Personalized greeting cards</li>\n");
    printf("<li>🎅 <strong>Secret Santa Organizer:</strong> Random gift assignments</li>\n");
    printf("<li>⏰ <strong>Live Countdown:</strong> Real-time Christmas countdown</li>\n");
    printf("</ul>\n");

    printf("<h3>📚 Learning Project</h3>\n");
    printf("<p>This application demonstrates advanced C programming concepts:</p>\n");
    printf("<ul>\n");
    printf("<li>CGI web development</li>\n");
    printf("<li>HTML/CSS generation from C</li>\n");
    printf("<li>Form processing and validation</li>\n");
    printf("<li>URL encoding/decoding</li>\n");
    printf("<li>Memory management</li>\n");
    printf("<li>Algorithm implementation</li>\n");
    printf("</ul>\n");

    printf("<h3>🎅 About the Developer</h3>\n");
    printf("<p>Created as part of a C programming learning journey. The goal was to build something fun and shareable while exploring web development with pure C.</p>\n");

    printf("<div style='text-align: center; margin-top: 30px; padding: 20px; background: linear-gradient(135deg, #667eea, #764ba2); color: white; border-radius: 10px;'>\n");
    printf("<h4>🎄 Merry Christmas! 🎅</h4>\n");
    printf("<p>Built with ❤️</p>\n");
    printf("<p style='font-size: 12px; margin-top: 10px;'>December 2025</p>\n");
    printf("</div>\n");

    printf("</div>\n");

    printf("<p style='text-align: center; margin-top: 20px;'>\n");
    printf("<a href='?' class='nav-back'>← Back to Main Menu</a>\n");
    printf("</p>\n");

    printf("</div>\n");
}