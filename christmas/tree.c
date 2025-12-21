#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cgi_utils.h"
#include "html_utils.h"

int main() {
    // Initialize CGI environment
    cgi_init();

    // Log access
    log_access("tree", "access");

    // Get parameters
    char *height_str = cgi_get_param("height");
    char *ornaments = cgi_get_param("ornaments");
    char *color = cgi_get_param("color");
    char *message = cgi_get_param("message");

    // Set defaults
    int height = height_str ? atoi(height_str) : 10;
    if (height < 3 || height > 20) height = 10;
    if (!ornaments) ornaments = "*!";
    if (!color) color = "#228b22";

    // Generate HTML response
    html_header("🎄 Christmas Tree Generator", NULL);

    printf("<h1>🎄 Christmas Tree Generator</h1>\n");

    // Show the generated tree
    html_christmas_tree(height, ornaments, color);

    // Show custom message if provided
    if (message && strlen(message) > 0) {
        char escaped_message[1024];
        html_escape(message, escaped_message, sizeof(escaped_message));
        printf("<div style='text-align: center; margin: 20px 0; font-size: 18px;'>\n");
        printf("%s\n", escaped_message);
        printf("</div>\n");
    }

    // Form to create another tree
    printf("<h2>🎨 Create Another Tree</h2>\n");
    html_form_start("/cgi-bin/tree.cgi", "GET");

    printf("<label>Tree Height (3-20):</label>\n");
    html_text_input("height", "Enter height", height_str);

    printf("<label>Ornaments (characters to use):</label>\n");
    html_text_input("ornaments", "e.g., *!@#", ornaments);

    printf("<label>Tree Color:</label>\n");
    html_select_start("color");
    html_select_option("#228b22", "Green", strcmp(color, "#228b22") == 0);
    html_select_option("#ff6b6b", "Red", strcmp(color, "#ff6b6b") == 0);
    html_select_option("#ffd93d", "Gold", strcmp(color, "#ffd93d") == 0);
    html_select_option("#6bcf7f", "Light Green", strcmp(color, "#6bcf7f") == 0);
    html_select_end();

    printf("<label>Custom Message:</label>\n");
    html_text_input("message", "Happy Holidays!", message);

    html_submit_button("🎄 Generate Tree");

    html_form_end();

    html_footer();

    return 0;
}