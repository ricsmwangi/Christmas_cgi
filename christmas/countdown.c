#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cgi_utils.h"
#include "html_utils.h"

int main() {
    // Initialize CGI environment
    cgi_init();

    // Log access
    log_access("countdown", "access");

    html_header("⏰ Christmas Countdown", NULL);

    printf("<h1>⏰ Christmas Countdown</h1>\n");
    printf("<p>Counting down to Christmas Day!</p>\n");

    // Show countdown timer
    html_countdown_timer();

    // Additional festive content
    printf("<div style='text-align: center; margin: 30px 0;'>\n");
    printf("<h2>🎄 Merry Christmas!</h2>\n");
    printf("<p>May your holidays be filled with joy, love, and lots of C programming! 🎅</p>\n");
    printf("<p>Built with pure C and CGI - no frameworks required!</p>\n");
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

    html_footer();

    return 0;
}