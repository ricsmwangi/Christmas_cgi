#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cgi_utils.h"
#include "html_utils.h"

#define MAX_PARTICIPANTS 20

int main() {
    // Initialize CGI environment
    cgi_init();

    // Get form data
    char *submit = cgi_get_param("submit");
    char *participants_str = cgi_get_param("participants");

    html_header("🎅 Secret Santa Randomizer", NULL);

    printf("<h1>🎅 Secret Santa Randomizer</h1>\n");

    if (submit && participants_str) {
        // Parse participants (assuming comma-separated)
        char *participants[MAX_PARTICIPANTS];
        int count = 0;

        char *token = strtok(strdup(participants_str), ",");
        while (token && count < MAX_PARTICIPANTS) {
            // Trim whitespace
            while (*token == ' ') token++;
            char *end = token + strlen(token) - 1;
            while (end > token && *end == ' ') *end-- = '\0';

            if (strlen(token) > 0) {
                participants[count++] = strdup(token);
            }
            token = strtok(NULL, ",");
        }

        if (count >= 2) {
            // Implement Secret Santa assignment logic
            srand(time(NULL)); // Seed random number generator
            
            // Create assignment array
            int assignments[MAX_PARTICIPANTS];
            int available[MAX_PARTICIPANTS];
            
            // Initialize available recipients
            for (int i = 0; i < count; i++) {
                available[i] = i;
            }
            
            // Shuffle assignments to ensure randomness
            for (int i = count - 1; i > 0; i--) {
                int j = rand() % (i + 1);
                int temp = available[i];
                available[i] = available[j];
                available[j] = temp;
            }
            
            // Assign ensuring no one gets themselves
            int valid_assignment = 0;
            int attempts = 0;
            const int MAX_ATTEMPTS = 100;
            
            while (!valid_assignment && attempts < MAX_ATTEMPTS) {
                valid_assignment = 1;
                
                // Reset assignments
                for (int i = 0; i < count; i++) {
                    assignments[i] = -1;
                    available[i] = i;
                }
                
                // Shuffle available recipients
                for (int i = count - 1; i > 0; i--) {
                    int j = rand() % (i + 1);
                    int temp = available[i];
                    available[i] = available[j];
                    available[j] = temp;
                }
                
                // Assign recipients
                for (int i = 0; i < count; i++) {
                    int recipient_idx = -1;
                    
                    // Find a recipient that's not themselves
                    for (int j = 0; j < count; j++) {
                        if (available[j] != -1 && available[j] != i) {
                            recipient_idx = available[j];
                            available[j] = -1; // Mark as taken
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
                // Display successful assignments
                printf("<div class='success'>\n");
                printf("<h2>🎅 Secret Santa Assignments</h2>\n");
                printf("<p>Here are your Secret Santa pairings:</p>\n");
                printf("<div style='background: rgba(255,255,255,0.9); color: #333; padding: 20px; border-radius: 10px; margin: 20px 0;'>\n");
                
                for (int i = 0; i < count; i++) {
                    printf("<p><strong>%s</strong> → buys for <strong>%s</strong></p>\n", 
                           participants[i], participants[assignments[i]]);
                }
                
                printf("</div>\n");
                printf("<p style='color: #666; font-size: 14px;'>Keep these assignments secret! 🎄</p>\n");
                printf("</div>\n");
            } else {
                printf("<div class='error'>\n");
                printf("<h2>❌ Assignment Failed</h2>\n");
                printf("<p>Unable to create valid assignments. Try again or add more participants.</p>\n");
                printf("</div>\n");
            }
            
            // Free memory
            for (int i = 0; i < count; i++) {
                free(participants[i]);
            }
            printf("<div class='error'>\n");
            printf("<p>❌ Please enter at least 2 participants!</p>\n");
            printf("</div>\n");
        }
    }

    // Form for participants
    printf("<h2>🎄 Add Participants</h2>\n");
    html_form_start("/cgi-bin/santa.cgi", "POST");

    printf("<label>Participants (comma-separated):</label>\n");
    html_text_input("participants", "Alice, Bob, Charlie, Diana", participants_str);

    html_submit_button("🎅 Generate Assignments");

    html_form_end();

    html_footer();

    return 0;
}