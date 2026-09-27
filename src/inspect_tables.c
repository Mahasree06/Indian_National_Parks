#include <stdio.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>

int main()
{
    htmlDocPtr doc;
    xmlXPathContextPtr context;
    xmlXPathObjectPtr result;
    int i;

    doc = htmlReadFile("download.html", NULL,
                       HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);

    if (doc == NULL)
    {
        printf("Failed to open HTML file\n");
        return 1;
    }

    context = xmlXPathNewContext(doc);

    /* Get only TABLE 2 */
    result = xmlXPathEvalExpression(
    (xmlChar *)"(//table)[1]//tr",
    context
);

    if (result == NULL || result->nodesetval == NULL)
    {
        printf("Table 2 not found\n");
        return 1;
    }

    printf("Rows in Table 2: %d\n\n",
           result->nodesetval->nodeNr);

    /* Print every row */
    for (i = 0; i < result->nodesetval->nodeNr; i++)
    {
        xmlNodePtr row;
        xmlNodePtr cell;

        row = result->nodesetval->nodeTab[i];

        cell = row->children;

        while (cell != NULL)
        {
            if (cell->type == XML_ELEMENT_NODE &&
                (xmlStrcmp(cell->name, (xmlChar *)"th") == 0 ||
                 xmlStrcmp(cell->name, (xmlChar *)"td") == 0))
            {
                xmlChar *text;

                text = xmlNodeGetContent(cell);

                if (text != NULL)
                {
                    printf("%s | ", text);
                    xmlFree(text);
                }
            }

            cell = cell->next;
        }

        printf("\n");
    }

    xmlXPathFreeObject(result);
    xmlXPathFreeContext(context);
    xmlFreeDoc(doc);
    xmlCleanupParser();

    return 0;
}
