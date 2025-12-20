#include <stdio.h>
#include <locale.h>

#define W 30

void row(const char *l, const char *r, const char *s) {
    printf("%s %-*s %s\n", l, W, s, r);
}

int main(void) {
    setlocale(LC_ALL, ""); // UTF-8

    printf("\n=== ASCII ===\n");
    row("|", "|", "❄ plain  fdfdfdf    text");
    row("|", "|", "❄ 2221234567890");

    printf("\n=== Box drawing ===\n");
    row("║", "║", "middle");
    row("║", "║", "alignment test");

    printf("\n=== Heavy borders ===\n");
    row("┃", "┃", "heavy vertical");
    row("┃", "┃", "weight check");

    printf("\n=== Kaomoji ===\n");
    row("║", "║", "(っ◕‿◕)っ");
    row("║", "║", "¯\\_(ツ)_/¯");

    printf("\n=== Emoji ===\n");
    row("║", "║", "🍅 POMODORO");
    row("║", "║", "⏱ 10:25");

    printf("\n=== Mixed ===\n");
    row("║", "║", "A🍅B");
    row("║", "║", "ABC");
    row("║", "║", "━━━");

    printf("\n=== Borders comparison ===\n");
    row("│", "│", "light");
    row("║", "║", "double");
    row("┃", "┃", "heavy");

    printf("\n");

    printf("║  ║\n");
    printf("║ ║");

    return 0;

}
