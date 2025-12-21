#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cgi_utils.h"
#include "html_utils.h"

int main() {
    // Initialize CGI environment
    cgi_init();

    // Check if form was submitted
    char *submit = cgi_get_param("submit");
    char *recipient = cgi_get_param("recipient");
    char *message = cgi_get_param("message");
    char *theme = cgi_get_param("theme");

    html_header("🎄 Holiday Card Creator", NULL);

    // Show card only if recipient AND message are provided (not requiring submit button)
    if (recipient && recipient[0] != '\0' && message && message[0] != '\0') {
        // Show the created card
        printf("<h1>🎄 Your Holiday Card</h1>\n");
        html_holiday_card(recipient, message, theme);

        // Option to create another
        printf("<h2>📝 Create Another Card</h2>\n");
    } else {
        // Show form
        printf("<h1>🎄 Create a Holiday Card</h1>\n");
        printf("<p>Fill out the form below to create a beautiful holiday card!</p>\n");
    }

    // Form for creating card
    html_form_start("?action=card", "GET");

    printf("<label>Recipient Name:</label>\n");
    html_text_input("recipient", "Who is this card for?", recipient);

    printf("<label>Your Message:</label>\n");
    html_textarea("message", "Write your holiday message here...", message);

    printf("<label>Card Theme:</label>\n");
    html_select_start("theme");
    html_select_option("traditional", "Traditional", (!theme || strcmp(theme, "traditional") == 0));
    html_select_option("modern", "Modern", theme && strcmp(theme, "modern") == 0);
    html_select_option("winter", "Winter Wonderland", theme && strcmp(theme, "winter") == 0);
    html_select_end();

    html_submit_button("🎄 Create Card");

    html_form_end();

    html_footer();

    return 0;
}