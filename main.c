//Zakat Calculation System
 /*Zakat = 2.5% of net zakatable wealth, if that wealth is at or above Nisab
   and has been held for one lunar year*/
 /* Nisab:
 *   Gold   = 87.48 g  (7.5 tola)
 *   Silver = 612.36 g (52.5 tola)
 * Gold/silver prices change daily, so the user enters today's price per gram.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ZAKAT_RATE        0.025
#define GOLD_NISAB_GRAMS  87.48
#define SILVER_NISAB_GRAMS 612.36
#define OUTPUT_FILE       "zakat_records.txt"

// Read a non-negative number safely; keeps asking until input is valid.
double read_amount(const char *prompt)
{
    char buf[100];
    char *end;
    double value;

    while (1) {
        printf("%s", prompt);
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            printf("\nInput closed. Exiting.\n");
            exit(1);
        }
        value = strtod(buf, &end);
        while (*end == ' ' || *end == '\t') end++;
        if (end != buf && (*end == '\n' || *end == '\0') && value >= 0) {
            return value;
        }
        printf("  Invalid input. Please enter a non-negative number.\n");
    }
}

// Read a menu choice between min and max.
int read_choice(const char *prompt, int min, int max)
{
    while (1) {
        double v = read_amount(prompt);
        int c = (int)v;
        if (v == (double)c && c >= min && c <= max) {
            return c;
        }
        printf("  Please enter a number from %d to %d.\n", min, max);
    }
}

// Prints the full report to any stream (screen or file).
void print_report(FILE *out,
                  double gold_price_gram, double silver_price_gram,
                  int nisab_choice, double nisab_value,
                  double cash, double bank, double gold_grams,
                  double silver_grams, double gold_value, double silver_value,
                  double business_stock, double investments,
                  double receivables, double other_assets,
                  double total_assets, double debts, double net_wealth,
                  int zakat_due, double zakat)
{
    fprintf(out, "==========================================\n");
    fprintf(out, "               SUMMARY\n");
    fprintf(out, "==========================================\n");
    fprintf(out, "Gold price per gram : %15.2f\n", gold_price_gram);
    fprintf(out, "Silver price/gram   : %15.2f\n", silver_price_gram);
    fprintf(out, "Nisab used          : %s\n",
            nisab_choice == 1 ? "Silver (612.36 g)" : "Gold (87.48 g)");
    fprintf(out, "------------------------------------------\n");
    fprintf(out, "Cash in hand        : %15.2f\n", cash);
    fprintf(out, "Bank balance        : %15.2f\n", bank);
    fprintf(out, "Gold (%.2f g)       : %15.2f\n", gold_grams, gold_value);
    fprintf(out, "Silver (%.2f g)     : %15.2f\n", silver_grams, silver_value);
    fprintf(out, "Business inventory  : %15.2f\n", business_stock);
    fprintf(out, "Investments         : %15.2f\n", investments);
    fprintf(out, "Receivables         : %15.2f\n", receivables);
    fprintf(out, "Other assets        : %15.2f\n", other_assets);
    fprintf(out, "------------------------------------------\n");
    fprintf(out, "Total assets        : %15.2f\n", total_assets);
    fprintf(out, "Less debts          : %15.2f\n", debts);
    fprintf(out, "Net zakatable wealth: %15.2f\n", net_wealth);
    fprintf(out, "Nisab threshold     : %15.2f\n", nisab_value);
    fprintf(out, "------------------------------------------\n");

    if (zakat_due) {
        fprintf(out, "Your wealth is above Nisab. Zakat is due.\n");
        fprintf(out, "ZAKAT PAYABLE (2.5%%): %.2f\n", zakat);
    } else {
        fprintf(out, "Your net wealth is below Nisab.\n");
        fprintf(out, "No Zakat is due.\n");
    }
}

int main(void)
{
    double gold_price_gram, silver_price_gram;
    double nisab_value;
    int nisab_choice;

    double cash, bank, gold_grams, silver_grams, business_stock;
    double investments, receivables, other_assets;
    double gold_value, silver_value;

    double debts, total_assets, net_wealth, zakat = 0.0;
    int zakat_due;

    printf("==========================================\n");
    printf("          ZAKAT CALCULATION SYSTEM\n");
    printf("==========================================\n\n");

    // Step 1: Nisab
    printf("STEP 1: Current metal prices (in PKR)\n");
    gold_price_gram   = read_amount("  Gold price per gram   : ");
    silver_price_gram = read_amount("  Silver price per gram : ");

    printf("\nWhich Nisab do you want to use?\n");
    printf("  1. Silver Nisab (612.36 g) - commonly used, lower threshold\n");
    printf("  2. Gold Nisab   (87.48 g)\n");
    nisab_choice = read_choice("Choice (1 or 2): ", 1, 2);

    if (nisab_choice == 1) {
        nisab_value = SILVER_NISAB_GRAMS * silver_price_gram;
    } else {
        nisab_value = GOLD_NISAB_GRAMS * gold_price_gram;
    }
    printf("\nNisab value: %.2f\n\n", nisab_value);

    //Step 2: Assets
    printf("STEP 2: Enter your zakatable assets (enter 0 if none)\n");
    cash           = read_amount("  Cash in hand                      : ");
    bank           = read_amount("  Bank balance / savings            : ");
    gold_grams     = read_amount("  Gold owned (in grams)             : ");
    silver_grams   = read_amount("  Silver owned (in grams)           : ");
    business_stock = read_amount("  Business inventory (resale value) : ");
    investments    = read_amount("  Shares / investments (market val) : ");
    receivables    = read_amount("  Money others owe you (sure to get): ");
    other_assets   = read_amount("  Other zakatable assets            : ");

    gold_value   = gold_grams * gold_price_gram;
    silver_value = silver_grams * silver_price_gram;

    // Step 3: Liabilities
    printf("\nSTEP 3: Liabilities\n");
    debts = read_amount("  Debts / loans due immediately     : ");

    //Calculation
    total_assets = cash + bank + gold_value + silver_value + business_stock
                 + investments + receivables + other_assets;
    net_wealth = total_assets - debts;

    zakat_due = (net_wealth >= nisab_value && net_wealth > 0);
    if (zakat_due) {
        zakat = net_wealth * ZAKAT_RATE;
    }

    //Show report on screen
    printf("\n");
    print_report(stdout,
                 gold_price_gram, silver_price_gram, nisab_choice, nisab_value,
                 cash, bank, gold_grams, silver_grams, gold_value, silver_value,
                 business_stock, investments, receivables, other_assets,
                 total_assets, debts, net_wealth, zakat_due, zakat);

    printf("\nNote: Zakat applies only if this wealth has been held for one\n");
    printf("full lunar year. For a final ruling, consult a scholar.\n");

    //Save report to file (appends, so old records are kept) 
    FILE *fp = fopen(OUTPUT_FILE, "a");
    if (fp == NULL) {
        printf("\nError: could not open %s for writing.\n", OUTPUT_FILE);
        return 1;
    }

    time_t now = time(NULL);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S",
             localtime(&now));

    fprintf(fp, "\n");
    fprintf(fp, "ZAKAT RECORD - %s\n", timestamp);
    fprintf(fp, "\n");
    print_report(fp,
                 gold_price_gram, silver_price_gram, nisab_choice, nisab_value,
                 cash, bank, gold_grams, silver_grams, gold_value, silver_value,
                 business_stock, investments, receivables, other_assets,
                 total_assets, debts, net_wealth, zakat_due, zakat);
    fprintf(fp, "\nNote: Zakat applies only if this wealth has been held for\n");
    fprintf(fp, "one full lunar year. For a final ruling, consult a scholar.\n");

    fclose(fp);
    printf("\nRecord saved to %s\n", OUTPUT_FILE);

    return 0;
}
