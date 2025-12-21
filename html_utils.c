#include "html_utils.h"

// Generate HTML document header
void html_header(const char *title, const char *css) {
    printf("Content-Type: text/html\n\n");
    printf("<!DOCTYPE html>\n");
    printf("<html lang='en'>\n");
    printf("<head>\n");
    printf("    <meta charset='UTF-8'>\n");
    printf("    <meta name='viewport' content='width=device-width, initial-scale=1.0'>\n");
    printf("    <title>%s</title>\n", title);
    if (css) {
        printf("    <style>%s</style>\n", css);
    } else {
        // Default festive CSS
        printf("    <style>\n");
        printf("        body { font-family: Arial, sans-serif; background: linear-gradient(135deg, #667eea 0%%, #764ba2 100%%); color: white; margin: 0; padding: 20px; min-height: 100vh; }\n");
        printf("        .container { max-width: 800px; margin: 0 auto; background: rgba(255,255,255,0.1); padding: 30px; border-radius: 15px; box-shadow: 0 8px 32px rgba(0,0,0,0.3); }\n");
        printf("        h1 { text-align: center; color: #ffd700; text-shadow: 2px 2px 4px rgba(0,0,0,0.5); }\n");
        printf("        form { margin: 20px 0; }\n");
        printf("        input, textarea, select { width: 100%%; padding: 10px; margin: 10px 0; border: none; border-radius: 5px; background: rgba(255,255,255,0.9); color: #333; }\n");
        printf("        button { background: #ff6b6b; color: white; padding: 12px 30px; border: none; border-radius: 25px; cursor: pointer; font-size: 16px; transition: all 0.3s; }\n");
        printf("        button:hover { background: #ff5252; transform: translateY(-2px); }\n");
        printf("        pre { background: rgba(0,0,0,0.3); padding: 20px; border-radius: 10px; overflow-x: auto; }\n");
        printf("        .christmas-tree { text-align: center; font-family: monospace; color: #228b22; }\n");
        printf("        .error { color: #ff6b6b; text-align: center; }\n");
        printf("        .success { color: #4ecdc4; text-align: center; }\n");
        printf("    </style>\n");
    }
    printf("</head>\n");
    printf("<body>\n");
    printf("    <div class='container'>\n");
}

// Generate HTML document footer
void html_footer(void) {
    printf("    </div>\n");
    printf("    <footer style='text-align: center; margin-top: 40px; padding: 20px; border-top: 2px solid rgba(255,215,0,0.3);'>\n");
    printf("        <p style='color: #ffd700; font-size: 14px; line-height: 1.6;'>\n");
    printf("            🎄 Hey guys, so this festive season you can enjoy something I made for everyone! 🎄<br>\n");
    printf("            A little holiday magic ✨<br>\n");
    printf("            © 2025 Santa's Mini Market - Spreading Holiday Cheer! 🎅\n");
    printf("        </p>\n");
    printf("    </footer>\n");
    printf("    <script>\n");
    printf("        // Copy card to clipboard\n");
    printf("        function copyCard() {\n");
    printf("            const cardContent = document.querySelector('.holiday-card');\n");
    printf("            if (cardContent) {\n");
    printf("                const text = cardContent.innerText;\n");
    printf("                navigator.clipboard.writeText(text).then(() => {\n");
    printf("                    alert('🎄 Card copied to clipboard!');\n");
    printf("                }).catch(() => {\n");
    printf("                    alert('Failed to copy card');\n");
    printf("                });\n");
    printf("            }\n");
    printf("        }\n");
    printf("        \n");
    printf("        // Share card via Web Share API or generate shareable link\n");
    printf("        function shareCard() {\n");
    printf("            const cardContent = document.querySelector('.holiday-card');\n");
    printf("            if (cardContent) {\n");
    printf("                const text = cardContent.innerText;\n");
    printf("                const currentUrl = window.location.href;\n");
    printf("                \n");
    printf("                if (navigator.share) {\n");
    printf("                    navigator.share({\n");
    printf("                        title: '🎄 Holiday Card',\n");
    printf("                        text: text,\n");
    printf("                        url: currentUrl\n");
    printf("                    }).catch(err => console.log('Share error:', err));\n");
    printf("                } else {\n");
    printf("                    // Fallback: copy link and show message\n");
    printf("                    navigator.clipboard.writeText(currentUrl).then(() => {\n");
    printf("                        alert('🔗 Card link copied to clipboard!\\n\\nShare this URL with friends to show them your card!');\n");
    printf("                    }).catch(() => {\n");
    printf("                        alert('Current URL: ' + currentUrl);\n");
    printf("                    });\n");
    printf("                }\n");
    printf("            }\n");
    printf("        }\n");
    printf("        \n");
    printf("        // Handle form submission with AJAX to prevent page reload\n");
    printf("        function handleFormSubmit(event, action) {\n");
    printf("            event.preventDefault();\n");
    printf("            const formData = new FormData(event.target);\n");
    printf("            const params = new URLSearchParams(formData);\n");
    printf("            const url = action + '&' + params.toString();\n");
    printf("            \n");
    printf("            fetch(url)\n");
    printf("                .then(response => response.text())\n");
    printf("                .then(html => {\n");
    printf("                    // Replace page content\n");
    printf("                    document.documentElement.innerHTML = html;\n");
    printf("                })\n");
    printf("                .catch(error => {\n");
    printf("                    console.error('Error:', error);\n");
    printf("                    alert('Error submitting form');\n");
    printf("                });\n");
    printf("        }\n");
    printf("        \n");
    printf("        // Add some festive JavaScript\n");
    printf("        document.addEventListener('DOMContentLoaded', function() {\n");
    printf("            // Intercept all navigation links to load content via AJAX\n");
    printf("            document.querySelectorAll('a[href*=\"action=\"]').forEach(link => {\n");
    printf("                link.addEventListener('click', function(e) {\n");
    printf("                    e.preventDefault();\n");
    printf("                    const url = this.getAttribute('href');\n");
    printf("                    fetch(url)\n");
    printf("                        .then(response => response.text())\n");
    printf("                        .then(html => {\n");
    printf("                            document.documentElement.innerHTML = html;\n");
    printf("                        })\n");
    printf("                        .catch(error => {\n");
    printf("                            console.error('Error:', error);\n");
    printf("                            window.location.href = url;\n");
    printf("                        });\n");
    printf("                });\n");
    printf("            });\n");
    printf("            \n");
    printf("            // Also intercept back-to-menu links\n");
    printf("            document.addEventListener('click', function(e) {\n");
    printf("                if (e.target.tagName === 'A' && (e.target.getAttribute('href') === '?' || e.target.getAttribute('href') === '')) {\n");
    printf("                    e.preventDefault();\n");
    printf("                    fetch('/')\n");
    printf("                        .then(response => response.text())\n");
    printf("                        .then(html => {\n");
    printf("                            document.documentElement.innerHTML = html;\n");
    printf("                        })\n");
    printf("                        .catch(error => {\n");
    printf("                            console.error('Error:', error);\n");
    printf("                            window.location.href = '/';\n");
    printf("                        });\n");
    printf("                }\n");
    printf("            });\n");
    printf("            \n");
    printf("            // Add snow effect or other animations here\n");
    printf("            console.log('🎄 Merry Christmas from CGI made by RKB!');\n");
    printf("            \n");
    printf("            // Add snowfall effect\n");
    printf("            const snowContainer = document.createElement('div');\n");
    printf("            snowContainer.style.position = 'fixed';\n");
    printf("            snowContainer.style.top = '0';\n");
    printf("            snowContainer.style.left = '0';\n");
    printf("            snowContainer.style.width = '100%%';\n");
    printf("            snowContainer.style.height = '100%%';\n");
    printf("            snowContainer.style.pointerEvents = 'none';\n");
    printf("            snowContainer.style.zIndex = '9999';\n");
    printf("            document.body.appendChild(snowContainer);\n");
    printf("            \n");
    printf("            // Create snowflakes\n");
    printf("            for (let i = 0; i < 50; i++) {\n");
    printf("                const snowflake = document.createElement('div');\n");
    printf("                snowflake.innerHTML = '❄';\n");
    printf("                snowflake.style.position = 'absolute';\n");
    printf("                snowflake.style.color = 'white';\n");
    printf("                snowflake.style.fontSize = Math.random() * 20 + 10 + 'px';\n");
    printf("                snowflake.style.left = Math.random() * 100 + '%%';\n");
    printf("                snowflake.style.animation = 'fall ' + (Math.random() * 6 + 8) + 's linear infinite';\n");
    printf("                snowflake.style.animationDelay = Math.random() * 2 + 's';\n");
    printf("                snowContainer.appendChild(snowflake);\n");
    printf("            }\n");
    printf("            \n");
    printf("            // Add keyframes for falling animation\n");
    printf("            const style = document.createElement('style');\n");
    printf("            style.textContent = '@keyframes fall { to { transform: translateY(100vh); } }';\n");
    printf("            document.head.appendChild(style);\n");
    printf("        });\n");
    printf("    </script>\n");
    printf("</body>\n");
    printf("</html>\n");
}

// Generate a simple form
void html_form_start(const char *action, const char *method) {
    printf("<form action='%s' method='%s' onsubmit='handleFormSubmit(event, \"%s\")'>\n", 
           action, method ? method : "GET", action);
}

void html_form_end(void) {
    printf("</form>\n");
}

// Generate form inputs
void html_text_input(const char *name, const char *placeholder, const char *value) {
    printf("<input type='text' name='%s' placeholder='%s' value='%s'>\n",
           name, placeholder ? placeholder : "", value ? value : "");
}

void html_textarea(const char *name, const char *placeholder, const char *value) {
    printf("<textarea name='%s' placeholder='%s' rows='4'>%s</textarea>\n",
           name, placeholder ? placeholder : "", value ? value : "");
}

void html_select_start(const char *name) {
    printf("<select name='%s'>\n", name);
}

void html_select_option(const char *value, const char *text, int selected) {
    printf("<option value='%s'%s>%s</option>\n",
           value, selected ? " selected" : "", text);
}

void html_select_end(void) {
    printf("</select>\n");
}

void html_submit_button(const char *text) {
    printf("<button type='submit' name='submit' value='1'>%s</button>\n", text ? text : "Submit");
}

// Generate festive HTML elements
void html_christmas_tree(int height, const char *ornaments, const char *color) {
    printf("<div class='christmas-tree'>\n");
    printf("<h2>🎄 Your Christmas Tree</h2>\n");
    printf("<pre style='color: %s;'>\n", color ? color : "#228b22");

    // Generate tree
    for(int i = 1; i <= height; i++) {
        // Add spaces for centering
        for(int spaces = 0; spaces < height - i; spaces++) printf(" ");
        // Add ornaments
        for(int stars = 0; stars < 2 * i - 1; stars++) {
            printf("%c", ornaments ? ornaments[stars % strlen(ornaments)] : '*');
        }
        printf("\n");
    }

    // Tree trunk
    for(int i = 0; i < 3; i++) {
        for(int spaces = 0; spaces < height - 1; spaces++) printf(" ");
        printf("|||\n");
    }

    printf("</pre>\n");
    printf("</div>\n");
}

void html_holiday_card(const char *recipient, const char *message, const char *theme) {
    (void)theme; // Suppress unused parameter warning
    printf("<div class='holiday-card'>\n");
    printf("<h2>🎄 Happy Holidays, %s!</h2>\n", recipient);
    printf("<div style='background: rgba(255,255,255,0.9); color: #333; padding: 20px; border-radius: 10px; margin: 20px 0;'>\n");
    printf("<p style='font-size: 18px; font-style: italic;'>%s</p>\n", message);
    printf("<p style='text-align: right; color: #8b0000; margin-top: 20px;'>From Santa 🎅</p>\n");
    printf("</div>\n");
    printf("</div>\n");
}

void html_countdown_timer(void) {
    printf("<div class='countdown'>\n");
    printf("<h2>⏰ Christmas Countdown</h2>\n");
    printf("<div id='countdown-display' style='font-size: 24px; text-align: center; margin: 20px 0;'></div>\n");
    printf("<script>\n");
    printf("function updateCountdown() {\n");
    printf("    const now = new Date();\n");
    printf("    const christmas = new Date(now.getFullYear(), 11, 25);\n");
    printf("    if (now > christmas) christmas.setFullYear(now.getFullYear() + 1);\n");
    printf("    const diff = christmas - now;\n");
    printf("    const days = Math.floor(diff / (1000 * 60 * 60 * 24));\n");
    printf("    const hours = Math.floor((diff %% (1000 * 60 * 60 * 24)) / (1000 * 60 * 60));\n");
    printf("    const minutes = Math.floor((diff %% (1000 * 60 * 60)) / (1000 * 60));\n");
    printf("    const seconds = Math.floor((diff %% (1000 * 60)) / 1000);\n");
    printf("    document.getElementById('countdown-display').innerHTML = \n");
    printf("        days + ' days, ' + hours + ' hours, ' + minutes + ' minutes, ' + seconds + ' seconds until Christmas!';\n");
    printf("}\n");
    printf("updateCountdown();\n");
    printf("setInterval(updateCountdown, 1000);\n");
    printf("</script>\n");
    printf("</div>\n");
}

// Generate error pages
void html_error_page(int status_code, const char *message) {
    printf("Status: %d\n", status_code);
    html_header("Error", NULL);
    printf("<div class='error'>\n");
    printf("<h1>❌ Error %d</h1>\n", status_code);
    printf("<p>%s</p>\n", message);
    printf("<p><a href='/'>← Back to Home</a></p>\n");
    printf("</div>\n");
    html_footer();
}

// Generate success pages
void html_success_page(const char *title, const char *message) {
    (void)message; // Suppress unused parameter warning
    html_header(title, NULL);
    printf("<div class='success'>\n");
    printf("<h1>✅ %s</h1>\n", title);
    printf("<p>Success! Your request has been processed.</p>\n");
    printf("<p><a href='/'>← Create Another</a></p>\n");
    printf("</div>\n");
    html_footer();
}