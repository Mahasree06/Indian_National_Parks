
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sqlite3.h>

#define DB_PATH "sql/national_parks.db"
#define INPUT_SIZE 256

/* ---------------------------------------------------------
   Remove leading and trailing spaces
   --------------------------------------------------------- */
void trim(char *str)
{
    int start = 0;
    int end;

    while (isspace((unsigned char)str[start]))
        start++;

    end = strlen(str) - 1;

    while (end >= start && isspace((unsigned char)str[end]))
        end--;

    memmove(str, str + start, end - start + 1);
    str[end - start + 1] = '\0';
}

/* ---------------------------------------------------------
   Convert string to lowercase
   --------------------------------------------------------- */
void to_lower(char *str)
{
    int i;

    for (i = 0; str[i] != '\0'; i++)
        str[i] = tolower((unsigned char)str[i]);
}

/* ---------------------------------------------------------
   List all parks
   --------------------------------------------------------- */
void list_all_parks(sqlite3 *db)
{
    sqlite3_stmt *stmt;

    const char *sql =
        "SELECT id, park_name, state_ut "
        "FROM national_parks "
        "ORDER BY id;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("Query preparation failed.\n");
        return;
    }

    printf("\n===== ALL NATIONAL PARKS =====\n\n");

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(stmt, 0);
        const char *park = (const char *)sqlite3_column_text(stmt, 1);
        const char *state = (const char *)sqlite3_column_text(stmt, 2);

        printf("%d | %s | %s\n", id, park, state);
    }

    sqlite3_finalize(stmt);
}

/* ---------------------------------------------------------
   Search park by name
   --------------------------------------------------------- */
void search_park(sqlite3 *db)
{
    char input[INPUT_SIZE];
    char search[INPUT_SIZE];
    sqlite3_stmt *stmt;

    printf("\nEnter park name: ");
    fgets(input, sizeof(input), stdin);

    trim(input);

    strcpy(search, input);
    to_lower(search);

    const char *sql =
        "SELECT park_name, state_ut, location, formed_year, "
        "notable_features, flora_fauna, rivers_lakes "
        "FROM national_parks "
        "WHERE LOWER(park_name) LIKE '%' || ? || '%';";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("Query preparation failed.\n");
        return;
    }

    sqlite3_bind_text(stmt, 1, search, -1, SQLITE_TRANSIENT);

    int found = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        found = 1;

        printf("\n===== PARK DETAILS =====\n\n");

        printf("Park: %s\n",
               sqlite3_column_text(stmt, 0));

        printf("State: %s\n",
               sqlite3_column_text(stmt, 1));

        printf("Location: %s\n",
               sqlite3_column_text(stmt, 2));

        printf("Formed Year: %s\n",
               sqlite3_column_text(stmt, 3));

        printf("Notable Features: %s\n",
               sqlite3_column_text(stmt, 4));

        printf("Flora/Fauna: %s\n",
               sqlite3_column_text(stmt, 5));

        printf("Rivers/Lakes: %s\n",
               sqlite3_column_text(stmt, 6));
    }

    if (!found)
        printf("\nPark not found.\n");

    sqlite3_finalize(stmt);
}

/* ---------------------------------------------------------
   Search by state
   --------------------------------------------------------- */
void search_state(sqlite3 *db)
{
    char input[INPUT_SIZE];
    char search[INPUT_SIZE];
    sqlite3_stmt *stmt;

    printf("\nEnter state/UT: ");
    fgets(input, sizeof(input), stdin);

    trim(input);

    strcpy(search, input);
    to_lower(search);

    const char *sql =
        "SELECT park_name, state_ut "
        "FROM national_parks "
        "WHERE LOWER(state_ut) LIKE '%' || ? || '%' "
        "ORDER BY park_name;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("Query preparation failed.\n");
        return;
    }

    sqlite3_bind_text(stmt, 1, search, -1, SQLITE_TRANSIENT);

    printf("\n===== PARKS IN %s =====\n\n", input);

    int found = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        found = 1;

        printf("%s | %s\n",
               sqlite3_column_text(stmt, 0),
               sqlite3_column_text(stmt, 1));
    }

    if (!found)
        printf("No parks found for this state/UT.\n");

    sqlite3_finalize(stmt);
}

/* ---------------------------------------------------------
   Count parks by state
   --------------------------------------------------------- */
void count_by_state(sqlite3 *db)
{
    sqlite3_stmt *stmt;

    const char *sql =
        "SELECT state_ut, COUNT(*) AS total "
        "FROM national_parks "
        "GROUP BY state_ut "
        "ORDER BY total DESC, state_ut;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("Query preparation failed.\n");
        return;
    }

    printf("\n===== PARK COUNT BY STATE/UT =====\n\n");

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        printf("%-30s | %d parks\n",
               sqlite3_column_text(stmt, 0),
               sqlite3_column_int(stmt, 1));
    }

    sqlite3_finalize(stmt);
}

/* ---------------------------------------------------------
   Search by formed year
   --------------------------------------------------------- */
void search_year(sqlite3 *db)
{
    char input[INPUT_SIZE];
    sqlite3_stmt *stmt;

    printf("\nEnter formed year: ");
    fgets(input, sizeof(input), stdin);

    trim(input);

    const char *sql =
        "SELECT park_name, state_ut, formed_year "
        "FROM national_parks "
        "WHERE formed_year = ? "
        "ORDER BY park_name;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("Query preparation failed.\n");
        return;
    }

    sqlite3_bind_text(stmt, 1, input, -1, SQLITE_TRANSIENT);

    printf("\n===== PARKS FORMED IN %s =====\n\n", input);

    int found = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        found = 1;

        printf("%s | %s | %s\n",
               sqlite3_column_text(stmt, 0),
               sqlite3_column_text(stmt, 1),
               sqlite3_column_text(stmt, 2));
    }

    if (!found)
        printf("No parks found for this year.\n");

    sqlite3_finalize(stmt);
}

/* ---------------------------------------------------------
   Database statistics
   --------------------------------------------------------- */
void database_statistics(sqlite3 *db)
{
    sqlite3_stmt *stmt;

    printf("\n===== DATABASE STATISTICS =====\n\n");

    const char *sql =
        "SELECT "
        "(SELECT COUNT(*) FROM national_parks), "
        "(SELECT COUNT(DISTINCT state_ut) FROM national_parks), "
        "(SELECT MIN(CAST(formed_year AS INTEGER)) "
        " FROM national_parks "
        " WHERE formed_year GLOB '[0-9]*'), "
        "(SELECT MAX(CAST(formed_year AS INTEGER)) "
        " FROM national_parks "
        " WHERE formed_year GLOB '[0-9]*');";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("Query preparation failed.\n");
        return;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        printf("Total parks        : %d\n",
               sqlite3_column_int(stmt, 0));

        printf("Total states/UTs   : %d\n",
               sqlite3_column_int(stmt, 1));

        printf("Earliest park year : %d\n",
               sqlite3_column_int(stmt, 2));

        printf("Latest park year   : %d\n",
               sqlite3_column_int(stmt, 3));
    }

    sqlite3_finalize(stmt);
}

/* ---------------------------------------------------------
   Search flora/fauna
   --------------------------------------------------------- */
void search_flora_fauna(sqlite3 *db)
{
    char input[INPUT_SIZE];
    char search[INPUT_SIZE];
    sqlite3_stmt *stmt;

    printf("\nEnter animal/flora/species: ");
    fgets(input, sizeof(input), stdin);

    trim(input);

    strcpy(search, input);
    to_lower(search);

    const char *sql =
        "SELECT park_name, state_ut "
        "FROM national_parks "
        "WHERE LOWER(flora_fauna) LIKE '%' || ? || '%' "
        "ORDER BY park_name;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("Query preparation failed.\n");
        return;
    }

    sqlite3_bind_text(stmt, 1, search, -1, SQLITE_TRANSIENT);

    printf("\n===== PARKS WITH '%s' =====\n\n", input);

    int found = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        found = 1;

        printf("%s | %s\n",
               sqlite3_column_text(stmt, 0),
               sqlite3_column_text(stmt, 1));
    }

    if (!found)
        printf("No parks found.\n");

    sqlite3_finalize(stmt);
}

/* ---------------------------------------------------------
   Search by river/lake
   --------------------------------------------------------- */
void search_river_lake(sqlite3 *db)
{
    char input[INPUT_SIZE];
    char search[INPUT_SIZE];
    sqlite3_stmt *stmt;

    printf("\nEnter river/lake: ");
    fgets(input, sizeof(input), stdin);

    trim(input);

    strcpy(search, input);
    to_lower(search);

    const char *sql =
        "SELECT park_name, state_ut, rivers_lakes "
        "FROM national_parks "
        "WHERE LOWER(rivers_lakes) LIKE '%' || ? || '%' "
        "ORDER BY park_name;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("Query preparation failed.\n");
        return;
    }

    sqlite3_bind_text(stmt, 1, search, -1, SQLITE_TRANSIENT);

    printf("\n===== PARKS WITH RIVER/LAKE '%s' =====\n\n", input);

    int found = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        found = 1;

        printf("%s | %s | %s\n",
               sqlite3_column_text(stmt, 0),
               sqlite3_column_text(stmt, 1),
               sqlite3_column_text(stmt, 2));
    }

    if (!found)
        printf("No parks found.\n");

    sqlite3_finalize(stmt);
}

/* ---------------------------------------------------------
   Search by location
   --------------------------------------------------------- */
void search_location(sqlite3 *db)
{
    char input[INPUT_SIZE];
    char search[INPUT_SIZE];
    sqlite3_stmt *stmt;

    printf("\nEnter location: ");
    fgets(input, sizeof(input), stdin);

    trim(input);

    strcpy(search, input);
    to_lower(search);

    const char *sql =
        "SELECT park_name, state_ut, location "
        "FROM national_parks "
        "WHERE LOWER(location) LIKE '%' || ? || '%' "
        "ORDER BY park_name;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("Query preparation failed.\n");
        return;
    }

    sqlite3_bind_text(stmt, 1, search, -1, SQLITE_TRANSIENT);

    printf("\n===== PARKS AT/NEAR '%s' =====\n\n", input);

    int found = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        found = 1;

        printf("%s | %s | %s\n",
               sqlite3_column_text(stmt, 0),
               sqlite3_column_text(stmt, 1),
               sqlite3_column_text(stmt, 2));
    }

    if (!found)
        printf("No parks found.\n");

    sqlite3_finalize(stmt);
}

/* ---------------------------------------------------------
   Search by notable features
   --------------------------------------------------------- */
void search_features(sqlite3 *db)
{
    char input[INPUT_SIZE];
    char search[INPUT_SIZE];
    sqlite3_stmt *stmt;

    printf("\nEnter notable feature/keyword: ");
    fgets(input, sizeof(input), stdin);

    trim(input);

    strcpy(search, input);
    to_lower(search);

    const char *sql =
        "SELECT park_name, state_ut, notable_features "
        "FROM national_parks "
        "WHERE LOWER(notable_features) LIKE '%' || ? || '%' "
        "ORDER BY park_name;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("Query preparation failed.\n");
        return;
    }

    sqlite3_bind_text(stmt, 1, search, -1, SQLITE_TRANSIENT);

    printf("\n===== PARKS WITH FEATURE '%s' =====\n\n", input);

    int found = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        found = 1;

        printf("%s | %s\n",
               sqlite3_column_text(stmt, 0),
               sqlite3_column_text(stmt, 1));

        printf("Feature: %s\n\n",
               sqlite3_column_text(stmt, 2));
    }

    if (!found)
        printf("No parks found.\n");

    sqlite3_finalize(stmt);
}

/* ---------------------------------------------------------
   Main menu
   --------------------------------------------------------- */
int main(void)
{
    sqlite3 *db;
    int choice;
    char input[INPUT_SIZE];

    if (sqlite3_open(DB_PATH, &db) != SQLITE_OK)
    {
        printf("Database opening failed: %s\n",
               sqlite3_errmsg(db));

        sqlite3_close(db);
        return 1;
    }

    printf("Database opened successfully.\n");

    while (1)
    {
        printf("\n");
        printf("===== NATIONAL PARK DATABASE =====\n");
        printf("1. List all parks\n");
        printf("2. Search park\n");
        printf("3. Search by state\n");
        printf("4. Count parks by state\n");
        printf("5. Search parks by formed year\n");
        printf("6. Database statistics\n");
        printf("7. Search by flora/fauna\n");
        printf("8. Search by river/lake\n");
        printf("9. Search by location\n");
        printf("10. Search by notable features\n");
        printf("11. Exit\n");

        printf("\nEnter choice: ");

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        choice = atoi(input);

        switch (choice)
        {
            case 1:
                list_all_parks(db);
                break;

            case 2:
                search_park(db);
                break;

            case 3:
                search_state(db);
                break;

            case 4:
                count_by_state(db);
                break;

            case 5:
                search_year(db);
                break;

            case 6:
                database_statistics(db);
                break;

            case 7:
                search_flora_fauna(db);
                break;

            case 8:
                search_river_lake(db);
                break;

            case 9:
                search_location(db);
                break;

            case 10:
                search_features(db);
                break;

            case 11:
                printf("\nGoodbye!\n");
                sqlite3_close(db);
                return 0;

            default:
                printf("\nInvalid choice. Please enter 1-11.\n");
        }
    }

    sqlite3_close(db);

    return 0;
}


