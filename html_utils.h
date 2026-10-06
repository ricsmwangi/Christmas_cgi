#ifndef HTML_UTILS_H
#define HTML_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Generate HTML document header
void html_header(const char *title, const char *css);

// Generate HTML document footer
void html_footer(void);

// Generate a simple form
void html_form_start(const char *action, const char *method);
void html_form_end(void);

// Generate form inputs
void html_text_input(const char *name, const char *placeholder, const char *value);
void html_textarea(const char *name, const char *placeholder, const char *value);
void html_select_start(const char *name);
void html_select_option(const char *value, const char *text, int selected);
void html_select_end(void);
void html_submit_button(const char *text);

// Generate festive HTML elements
void html_christmas_tree(int height, const char *ornaments, const char *color);
void html_holiday_card(const char *recipient, const char *message, const char *theme);
void html_countdown_timer(void);

// Validate a CSS color string (#rgb / #rrggbb) to prevent injection
int is_safe_color(const char *c);

// Generate error pages
void html_error_page(int status_code, const char *message);

// Generate success pages
void html_success_page(const char *title, const char *message);

#endif // HTML_UTILS_H