#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cgi_utils.h"
#include "html_utils.h"

#define MAX_PARTICIPANTS 20

// Helper function to escape single quotes in strings for JavaScript
void escape_for_js(const char *str) {
    if (!str) return;
    for (int i = 0; str[i]; i++) {
        if (str[i] == '\'') printf("\\\\'");
        else if (str[i] == '"') printf("\\\\\"");
        else if (str[i] == '\\') printf("\\\\\\\\");
        else if (str[i] == '\n') printf("\\\\n");
        else if (str[i] == '\r') printf("\\\\r");
        else printf("%c", str[i]);
    }
}

// Function prototypes
void show_main_menu();
void show_tree_section();
void show_card_section();
void show_santa_section();
void show_countdown_section();
void show_about_section();

int main() {
    // Initialize CGI environment
    cgi_init();

    // Get action parameter to determine what to show
    char *action = cgi_get_param("action");

    html_header("🎄 Santa's Christmas Mini Market", NULL);

    printf("<h1>🎄 Santa's Christmas Mini Market</h1>\n");
    printf("<p>Welcome to your one-stop shop for holiday fun! Choose what you'd like to create:</p>\n");

    // If no action specified, show the main menu
    if (!action) {
        show_main_menu();
    } else if (strcmp(action, "tree") == 0) {
        show_tree_section();
    } else if (strcmp(action, "card") == 0) {
        show_card_section();
    } else if (strcmp(action, "santa") == 0) {
        show_santa_section();
    } else if (strcmp(action, "countdown") == 0) {
        show_countdown_section();
    } else if (strcmp(action, "about") == 0) {
        show_about_section();
    } else {
        // Invalid action, show main menu
        printf("<div class='error'>\n");
        printf("<p>❌ Invalid option selected. Please choose from the menu below.</p>\n");
        printf("</div>\n");
        show_main_menu();
    }

    html_footer();

    return 0;
}

void show_main_menu() {
    // Dancing Santa at the top of dashboard
    printf("<div style='text-align: center; margin-bottom: 40px; font-size: 80px; animation: santaDance 1.5s infinite;'>🎅</div>\n");
    printf("<style>\n");
    printf("    @keyframes santaDance {\n");
    printf("        0%% { transform: translateX(-10px) rotate(-5deg); }\n");
    printf("        25%% { transform: translateX(10px) rotate(5deg); }\n");
    printf("        50%% { transform: translateX(-10px) rotate(-5deg); }\n");
    printf("        75%% { transform: translateX(10px) rotate(5deg); }\n");
    printf("        100%% { transform: translateX(-10px) rotate(-5deg); }\n");
    printf("    }\n");
    printf("</style>\n");
    
    printf("<div style='display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 20px; margin: 30px 0;'>\n");

    // Christmas Tree Option
    printf("<div style='background: linear-gradient(135deg, #228b22, #32cd32); color: white; padding: 25px; border-radius: 15px; text-align: center; box-shadow: 0 4px 15px rgba(0,0,0,0.2); transition: all 0.3s ease; cursor: pointer;' onmouseover='this.style.transform=\"translateY(-5px)\"; this.style.boxShadow=\"0 8px 25px rgba(0,0,0,0.3)\"' onmouseout='this.style.transform=\"translateY(0)\"; this.style.boxShadow=\"0 4px 15px rgba(0,0,0,0.2)\"'>\n");
    printf("<h2>🎄 Christmas Tree</h2>\n");
    printf("<p>Generate beautiful ASCII Christmas trees with custom ornaments and colors!</p>\n");
    printf("<a href='?action=tree' style='background: #ffd700; color: #228b22; padding: 10px 20px; border-radius: 25px; text-decoration: none; font-weight: bold; display: inline-block; margin-top: 10px; transition: all 0.3s ease;' onmouseover='this.style.background=\"#ffed4e\"' onmouseout='this.style.background=\"#ffd700\"'>Create Tree</a>\n");
    printf("</div>\n");

    // Holiday Card Option
    printf("<div style='background: linear-gradient(135deg, #ff69b4, #ffb6c1); color: white; padding: 25px; border-radius: 15px; text-align: center; box-shadow: 0 4px 15px rgba(0,0,0,0.2); transition: all 0.3s ease; cursor: pointer;' onmouseover='this.style.transform=\"translateY(-5px)\"; this.style.boxShadow=\"0 8px 25px rgba(0,0,0,0.3)\"' onmouseout='this.style.transform=\"translateY(0)\"; this.style.boxShadow=\"0 4px 15px rgba(0,0,0,0.2)\"'>\n");
    printf("<h2>🎅 Holiday Card</h2>\n");
    printf("<p>Create personalized holiday cards for friends and family!</p>\n");
    printf("<a href='?action=card' style='background: #ffd700; color: #ff69b4; padding: 10px 20px; border-radius: 25px; text-decoration: none; font-weight: bold; display: inline-block; margin-top: 10px; transition: all 0.3s ease;' onmouseover='this.style.background=\"#ffed4e\"' onmouseout='this.style.background=\"#ffd700\"'>Create Card</a>\n");
    printf("</div>\n");

    // Secret Santa Option
    printf("<div style='background: linear-gradient(135deg, #ff4500, #ff6347); color: white; padding: 25px; border-radius: 15px; text-align: center; box-shadow: 0 4px 15px rgba(0,0,0,0.2); transition: all 0.3s ease; cursor: pointer;' onmouseover='this.style.transform=\"translateY(-5px)\"; this.style.boxShadow=\"0 8px 25px rgba(0,0,0,0.3)\"' onmouseout='this.style.transform=\"translateY(0)\"; this.style.boxShadow=\"0 4px 15px rgba(0,0,0,0.2)\"'>\n");
    printf("<h2>🎁 Secret Santa</h2>\n");
    printf("<p>Generate random gift assignments for your holiday party!</p>\n");
    printf("<a href='?action=santa' style='background: #ffd700; color: #ff4500; padding: 10px 20px; border-radius: 25px; text-decoration: none; font-weight: bold; display: inline-block; margin-top: 10px; transition: all 0.3s ease;' onmouseover='this.style.background=\"#ffed4e\"' onmouseout='this.style.background=\"#ffd700\"'>Start Santa</a>\n");
    printf("</div>\n");

    // Countdown Option
    printf("<div style='background: linear-gradient(135deg, #4169e1, #6495ed); color: white; padding: 25px; border-radius: 15px; text-align: center; box-shadow: 0 4px 15px rgba(0,0,0,0.2); transition: all 0.3s ease; cursor: pointer;' onmouseover='this.style.transform=\"translateY(-5px)\"; this.style.boxShadow=\"0 8px 25px rgba(0,0,0,0.3)\"' onmouseout='this.style.transform=\"translateY(0)\"; this.style.boxShadow=\"0 4px 15px rgba(0,0,0,0.2)\"'>\n");
    printf("<h2>⏰ Christmas Countdown</h2>\n");
    printf("<p>Live countdown to Christmas Day with festive animations!</p>\n");
    printf("<a href='?action=countdown' style='background: #ffd700; color: #4169e1; padding: 10px 20px; border-radius: 25px; text-decoration: none; font-weight: bold; display: inline-block; margin-top: 10px; transition: all 0.3s ease;' onmouseover='this.style.background=\"#ffed4e\"' onmouseout='this.style.background=\"#ffd700\"'>View Countdown</a>\n");
    printf("</div>\n");

    printf("</div>\n");

    // Footer navigation
    printf("<div style='text-align: center; margin-top: 40px; padding: 20px; background: rgba(255,255,255,0.1); border-radius: 10px;'>\n");
    printf("<p style='margin: 0; color: #ffd700;'>🎄 Celebrate the holidays! 🎅</p>\n");
    printf("<p style='margin: 5px 0; font-size: 14px;'>Choose your activity above</p>\n");
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

    printf("<div style='background: rgba(34, 139, 34, 0.1); padding: 20px; border-radius: 10px; margin: 20px 0;'>\n");
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

    printf("<div style='display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 15px; margin: 20px 0;'>\n");

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

    printf("<div style='background: rgba(255, 105, 180, 0.1); padding: 20px; border-radius: 10px; margin: 20px 0;'>\n");
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

    printf("<div style='display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 15px; margin: 20px 0;'>\n");

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

void show_santa_section() {
    // Get form data
    char *participants_str = cgi_get_param("participants");

    printf("<div style='background: rgba(255, 69, 0, 0.1); padding: 20px; border-radius: 10px; margin: 20px 0;'>\n");
    printf("<h2>🎁 Secret Santa Randomizer</h2>\n");

    if (participants_str && participants_str[0] != '\0') {
        // Parse and process participants (same logic as santa.c)
        char *participants[MAX_PARTICIPANTS];
        int count = 0;

        char *token = strtok(strdup(participants_str), ",");
        while (token && count < MAX_PARTICIPANTS) {
            while (*token == ' ') token++;
            char *end = token + strlen(token) - 1;
            while (end > token && *end == ' ') *end-- = '\0';

            if (strlen(token) > 0) {
                participants[count++] = strdup(token);
            }
            token = strtok(NULL, ",");
        }

        if (count >= 2) {
            // Secret Santa assignment logic (same as santa.c)
            srand(time(NULL));
            int assignments[MAX_PARTICIPANTS];
            int available[MAX_PARTICIPANTS];

            for (int i = 0; i < count; i++) {
                available[i] = i;
            }

            for (int i = count - 1; i > 0; i--) {
                int j = rand() % (i + 1);
                int temp = available[i];
                available[i] = available[j];
                available[j] = temp;
            }

            int valid_assignment = 0;
            int attempts = 0;
            const int MAX_ATTEMPTS = 100;

            while (!valid_assignment && attempts < MAX_ATTEMPTS) {
                valid_assignment = 1;

                for (int i = 0; i < count; i++) {
                    assignments[i] = -1;
                    available[i] = i;
                }

                for (int i = count - 1; i > 0; i--) {
                    int j = rand() % (i + 1);
                    int temp = available[i];
                    available[i] = available[j];
                    available[j] = temp;
                }

                for (int i = 0; i < count; i++) {
                    int recipient_idx = -1;

                    for (int j = 0; j < count; j++) {
                        if (available[j] != -1 && available[j] != i) {
                            recipient_idx = available[j];
                            available[j] = -1;
                            break;
                        }
                    }

                    if (recipient_idx == -1) {
                        valid_assignment = 0;
                        break;
                    }

                    assignments[i] = recipient_idx;
                }

                attempts++;
            }

            if (valid_assignment) {
                printf("<h3>Secret Santa Assignments:</h3>\n");
                printf("<div style='background: rgba(255,255,255,0.9); color: #333; padding: 20px; border-radius: 10px; margin: 20px 0;'>\n");

                for (int i = 0; i < count; i++) {
                    printf("<p><strong>%s</strong> → buys for <strong>%s</strong></p>\n",
                           participants[i], participants[assignments[i]]);
                }

                printf("</div>\n");
            } else {
                printf("<div class='error'>\n");
                printf("<p>❌ Unable to create valid assignments. Try again!</p>\n");
                printf("</div>\n");
            }

            for (int i = 0; i < count; i++) {
                free(participants[i]);
            }
        } else {
            printf("<div class='error'>\n");
            printf("<p>❌ Please enter at least 2 participants!</p>\n");
            printf("</div>\n");
        }
    }

    // Secret Santa form
    printf("<h3>Add Participants:</h3>\n");
    html_form_start("?action=santa", "GET");

    printf("<label>Participants (comma-separated):</label>\n");
    html_text_input("participants", "Alice, Bob, Charlie, Diana", participants_str);

    html_submit_button("🎁 Generate Assignments");

    html_form_end();

    printf("<p style='text-align: center; margin-top: 20px;'>\n");
    printf("<a href='?' class='nav-back'>← Back to Main Menu</a>\n");
    printf("</p>\n");

    printf("</div>\n");
}

void show_countdown_section() {
    printf("<div style='background: rgba(65, 105, 225, 0.1); padding: 20px; border-radius: 10px; margin: 20px 0;'>\n");
    printf("<h2>⏰ Christmas Countdown</h2>\n");

    html_countdown_timer();

    printf("<div style='text-align: center; margin: 30px 0;'>\n");
    printf("<h3>🎄 Merry Christmas!</h3>\n");
    printf("<p>May your holidays be filled with joy, love, and celebration! 🎅</p>\n");
    printf("<p>Enjoy the festive season! 🎉</p>\n");
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
    printf(" ***************\n");
    printf("        |||\n");
    printf("        |||\n");
    printf("</pre>\n");
    printf("</div>\n");

    printf("<p style='text-align: center; margin-top: 20px;'>\n");
    printf("<a href='?' class='nav-back'>← Back to Main Menu</a>\n");
    printf("</p>\n");

    printf("</div>\n");
}

void show_about_section() {
    printf("<div style='background: rgba(255, 215, 0, 0.1); padding: 20px; border-radius: 10px; margin: 20px 0;'>\n");
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