#include <stdio.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>

int main()
{
    htmlDocPtr doc;
    xmlXPathContextPtr context;
    xmlXPathObjectPtr result;

    doc = htmlReadFile("download.html", NULL,
                       HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);

    if (doc == NULL)
    {
        printf("Failed to open download.html\n");
        return 1;
    }

    context = xmlXPathNewContext(doc);

    /*
     * Get headings (h2/h3) from the page.
     * These headings identify the State/UT sections.
     */
    result = xmlXPathEvalExpression(
        (xmlChar *)"//h2 | //h3",
        context
    );

    if (result == NULL || result->nodesetval == NULL)
    {
        printf("No headings found\n");
        return 1;
    }

    printf("State/UT headings found:\n\n");

    for (int i = 0; i < result->nodesetval->nodeNr; i++)
    {
        xmlNodePtr node = result->nodesetval->nodeTab[i];

        xmlChar *text = xmlNodeGetContent(node);

        if (text != NULL)
        {
            printf("%s : %s\n",
                   node->name,
                   text);

            xmlFree(text);
        }
    }

    xmlXPathFreeObject(result);
    xmlXPathFreeContext(context);
    xmlFreeDoc(doc);
    xmlCleanupParser();

    return 0;
}
