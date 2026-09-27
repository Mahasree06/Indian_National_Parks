#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>

#include "../include/national_park.h"

#define MAX_TEXT 1000


void clean_text(char *text)
{
    int i;
    int j = 0;
    int space = 0;

    for (i = 0; text[i] != '\0'; i++)
    {
        if (isspace((unsigned char)text[i]))
        {
            if (!space)
            {
                text[j++] = ' ';
                space = 1;
            }

            space = 1;
        }
        else
        {
            text[j++] = text[i];
            space = 0;
        }
    }

    text[j] = '\0';
}


void remove_references(char *text)
{
    char temp[MAX_TEXT];

    int i = 0;
    int j = 0;

    while (text[i] != '\0')
    {
        if (text[i] == '[')
        {
            while (text[i] != '\0' && text[i] != ']')
                i++;

            if (text[i] == ']')
                i++;
        }
        else
        {
            temp[j++] = text[i++];
        }
    }

    temp[j] = '\0';

    strcpy(text, temp);
}


void get_text(xmlNodePtr node, char *output, int size)
{
    xmlChar *content;

    content = xmlNodeGetContent(node);

    if (content == NULL)
    {
        strcpy(output, "N/A");
        return;
    }

    snprintf(output, size, "%s", (char *)content);

    xmlFree(content);

    clean_text(output);
    remove_references(output);
    clean_text(output);

    if (strlen(output) == 0)
        strcpy(output, "N/A");
}


void csv_escape(char *text)
{
    char temp[MAX_TEXT];

    int i = 0;
    int j = 0;

    while (text[i] != '\0' && j < MAX_TEXT - 2)
    {
        if (text[i] == '"')
        {
            temp[j++] = '"';
            temp[j++] = '"';
        }
        else
        {
            temp[j++] = text[i];
        }

        i++;
    }

    temp[j] = '\0';

    strcpy(text, temp);
}


int is_year(const char *text)
{
    int i;

    if (strlen(text) != 4)
        return 0;

    for (i = 0; i < 4; i++)
    {
        if (!isdigit((unsigned char)text[i]))
            return 0;
    }

    return 1;
}


int main()
{
    htmlDocPtr doc;
    xmlXPathContextPtr context;
    xmlXPathObjectPtr tables;

    FILE *csv;

    doc = htmlReadFile(
        "download.html",
        NULL,
        HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING
    );

    if (doc == NULL)
    {
        printf("Failed to open download.html\n");
        return 1;
    }

    context = xmlXPathNewContext(doc);

    if (context == NULL)
    {
        printf("Failed to create XPath context\n");
        xmlFreeDoc(doc);
        return 1;
    }


    tables = xmlXPathEvalExpression(
        (xmlChar *)"//table",
        context
    );

    if (tables == NULL ||
        tables->nodesetval == NULL)
    {
        printf("No tables found\n");

        xmlXPathFreeContext(context);
        xmlFreeDoc(doc);

        return 1;
    }


    csv = fopen("project_data.csv", "w");

    if (csv == NULL)
    {
        printf("Failed to create project_data.csv\n");

        xmlXPathFreeObject(tables);
        xmlXPathFreeContext(context);
        xmlFreeDoc(doc);

        return 1;
    }


    fprintf(
        csv,
        "park_name,state_ut,location,formed_year,notable_features,flora_fauna,rivers_lakes\n"
    );


    int total_records = 0;


    for (int i = 1;
         i < tables->nodesetval->nodeNr;
         i++)
    {
        xmlNodePtr table;

        table = tables->nodesetval->nodeTab[i];


        /*
         * Check whether this is a park table.
         */

        xmlXPathObjectPtr header_result;

        header_result = xmlXPathNodeEval(
            table,
            (xmlChar *)".//tr[1]/*[self::th or self::td]",
            context
        );

        if (header_result == NULL ||
            header_result->nodesetval == NULL ||
            header_result->nodesetval->nodeNr == 0)
        {
            if (header_result != NULL)
                xmlXPathFreeObject(header_result);

            continue;
        }


        char first_header[MAX_TEXT];

        get_text(
            header_result->nodesetval->nodeTab[0],
            first_header,
            sizeof(first_header)
        );


        if (strcmp(first_header, "Name") != 0)
        {
            xmlXPathFreeObject(header_result);
            continue;
        }


        xmlXPathFreeObject(header_result);


        /*
         * Find State/UT from nearest h2.
         */

        char state[STATE_SIZE];

        strcpy(state, "Unknown");


        xmlXPathObjectPtr state_result;

        state_result = xmlXPathNodeEval(
            table,
            (xmlChar *)"preceding::h2[1]",
            context
        );


        if (state_result != NULL &&
            state_result->nodesetval != NULL &&
            state_result->nodesetval->nodeNr > 0)
        {
            char state_text[MAX_TEXT];

            get_text(
                state_result->nodesetval->nodeTab[0],
                state_text,
                sizeof(state_text)
            );


            char *p;

            p = strchr(state_text, '(');

            if (p != NULL)
                *p = '\0';

            clean_text(state_text);


            snprintf(
                state,
                STATE_SIZE,
                "%s",
                state_text
            );
        }


        if (state_result != NULL)
            xmlXPathFreeObject(state_result);


        /*
         * Get rows.
         */

        xmlXPathObjectPtr rows;

        rows = xmlXPathNodeEval(
            table,
            (xmlChar *)".//tr",
            context
        );


        if (rows == NULL ||
            rows->nodesetval == NULL)
        {
            if (rows != NULL)
                xmlXPathFreeObject(rows);

            continue;
        }


        /*
         * Process every data row.
         */

        for (int r = 1;
             r < rows->nodesetval->nodeNr;
             r++)
        {
            xmlNodePtr row;

            row = rows->nodesetval->nodeTab[r];


            xmlXPathObjectPtr cells;

            cells = xmlXPathNodeEval(
                row,
                (xmlChar *)"./th | ./td",
                context
            );


            if (cells == NULL ||
                cells->nodesetval == NULL ||
                cells->nodesetval->nodeNr == 0)
            {
                if (cells != NULL)
                    xmlXPathFreeObject(cells);

                continue;
            }


            int cell_count =
                cells->nodesetval->nodeNr;


            char park_name[MAX_TEXT] = "N/A";
            char location[MAX_TEXT] = "N/A";
            char formed_year[MAX_TEXT] = "N/A";
            char features[MAX_TEXT] = "N/A";
            char flora[MAX_TEXT] = "N/A";
            char rivers[MAX_TEXT] = "N/A";


            /*
             * Cell 0 is always the park name.
             */

            get_text(
                cells->nodesetval->nodeTab[0],
                park_name,
                sizeof(park_name)
            );


            if (strcmp(park_name, "Name") == 0 ||
                strcmp(park_name, "N/A") == 0)
            {
                xmlXPathFreeObject(cells);
                continue;
            }


            /*
             * Temporary storage for remaining cells.
             */

            char values[10][MAX_TEXT];

            int value_count = 0;


            for (int c = 1;
                 c < cell_count && value_count < 10;
                 c++)
            {
                get_text(
                    cells->nodesetval->nodeTab[c],
                    values[value_count],
                    MAX_TEXT
                );

                value_count++;
            }


            /*
             * The normal table structure is:
             *
             * Name
             * Image
             * Location
             * Formed
             * Features
             * Flora/Fauna
             * Rivers/Lakes
             *
             * But Image may be missing.
             *
             * Therefore:
             *
             * First identify the year.
             */

            int year_index = -1;

            for (int c = 0;
                 c < value_count;
                 c++)
            {
                if (is_year(values[c]))
                {
                    year_index = c;
                    break;
                }
            }


            /*
             * If a year was found, everything around
             * it can be mapped more safely.
             */

            if (year_index >= 0)
            {
                strcpy(formed_year, values[year_index]);


                /*
                 * The cell immediately before the year
                 * is normally Location.
                 *
                 * But if it looks like image information,
                 * don't use it.
                 */

                if (year_index > 0)
                {
                    char *previous =
                        values[year_index - 1];

                    if (strstr(previous, "located") == NULL &&
                        strstr(previous, "photo") == NULL)
                    {
                        strcpy(location, previous);
                    }
                }


                /*
                 * Values after the year:
                 *
                 * +1 = Features
                 * +2 = Flora/Fauna
                 * +3 = Rivers/Lakes
                 */

               int next_index = year_index + 1;

/*
 * Some Wikipedia rows contain an Area column
 * after the Formed year.
 *
 * Example:
 *
 * 1986
 * 483 Km2
 * Features
 * Flora/Fauna
 * Rivers
 *
 * Skip the Area column.
 */

if (next_index < value_count)
{
    if (strstr(values[next_index], "Km2") != NULL ||
        strstr(values[next_index], "km2") != NULL ||
        strstr(values[next_index], "km²") != NULL)
    {
        next_index++;
    }
}


/*
 * After skipping Area, the columns are:
 *
 * Features
 * Flora/Fauna
 * Rivers/Lakes
 */

if (next_index < value_count)
{
    strcpy(
        features,
        values[next_index]
    );

    next_index++;
}

if (next_index < value_count)
{
    strcpy(
        flora,
        values[next_index]
    );

    next_index++;
}

if (next_index < value_count)
{
    strcpy(
        rivers,
        values[next_index]
    );
}
            }


            /*
             * Special case:
             *
             * If the cell before the year is actually
             * an image description, location is unknown.
             */

            if (year_index == 1)
            {
                /*
                 * Example:
                 *
                 * Name
                 * Image
                 * 1992
                 * Features
                 * Rivers
                 *
                 * So location is N/A.
                 */

                strcpy(location, "N/A");
            }


            /*
             * CSV escaping.
             */

            csv_escape(park_name);
            csv_escape(location);
            csv_escape(formed_year);
            csv_escape(features);
            csv_escape(flora);
            csv_escape(rivers);


            fprintf(
                csv,
                "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n",
                park_name,
                state,
                location,
                formed_year,
                features,
                flora,
                rivers
            );


            total_records++;


            xmlXPathFreeObject(cells);
        }


        xmlXPathFreeObject(rows);
    }


    fclose(csv);


    printf(
        "Total park records written: %d\n",
        total_records
    );

    printf(
        "CSV file created: project_data.csv\n"
    );


    xmlXPathFreeObject(tables);

    xmlXPathFreeContext(context);

    xmlFreeDoc(doc);

    xmlCleanupParser();


    return 0;
}
