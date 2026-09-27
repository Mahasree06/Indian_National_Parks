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
        printf("Failed to open HTML\n");
        return 1;
    }

    context = xmlXPathNewContext(doc);

    result = xmlXPathEvalExpression(
    (xmlChar *)"(//table)[4]//tr[2]",
    context

    );

    if (result == NULL ||
        result->nodesetval == NULL ||
        result->nodesetval->nodeNr == 0)
    {
        printf("Mouling row not found\n");
        return 1;
    }

    xmlNodePtr row = result->nodesetval->nodeTab[0];
    xmlNodePtr cell = row->children;

    while (cell != NULL)
    {
        if (cell->type == XML_ELEMENT_NODE &&
            (xmlStrcmp(cell->name, (xmlChar *)"td") == 0 ||
             xmlStrcmp(cell->name, (xmlChar *)"th") == 0))
        {
            xmlChar *text = xmlNodeGetContent(cell);

            printf("\nTAG: %s\n", cell->name);

            if (text != NULL)
            {
                printf("CONTENT: %s\n", text);
                xmlFree(text);
            }

            xmlChar *colspan =
                xmlGetProp(cell, (xmlChar *)"colspan");

            xmlChar *rowspan =
                xmlGetProp(cell, (xmlChar *)"rowspan");

            printf("colspan: %s\n",
                   colspan ? (char *)colspan : "none");

            printf("rowspan: %s\n",
                   rowspan ? (char *)rowspan : "none");

            if (colspan)
                xmlFree(colspan);

            if (rowspan)
                xmlFree(rowspan);
        }

        cell = cell->next;
    }

    xmlXPathFreeObject(result);
    xmlXPathFreeContext(context);
    xmlFreeDoc(doc);
    xmlCleanupParser();

    return 0;
}
