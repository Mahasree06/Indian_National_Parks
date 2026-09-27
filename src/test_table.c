#include <stdio.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>

int main()
{
    htmlDocPtr doc;
    xmlXPathContextPtr context;
    xmlXPathObjectPtr result;

    doc = htmlReadFile(
        "download.html",
        NULL,
        HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING
    );

    if (doc == NULL)
    {
        printf("Failed to open HTML\n");
        return 1;
    }

    context = xmlXPathNewContext(doc);

    result = xmlXPathEvalExpression(
        (xmlChar *)"(//table)[2]//tr[1]/*",
        context
    );

    if (result == NULL)
    {
        printf("XPath failed\n");
        return 1;
    }

    if (result->nodesetval == NULL)
    {
        printf("No nodes found\n");
        return 1;
    }

    printf("Cells found: %d\n\n",
           result->nodesetval->nodeNr);

    for (int i = 0;
         i < result->nodesetval->nodeNr;
         i++)
    {
        xmlNodePtr node =
            result->nodesetval->nodeTab[i];

        xmlChar *text =
            xmlNodeGetContent(node);

        printf(
            "CELL %d | TAG: %s | CONTENT: %s\n",
            i,
            node->name,
            text ? (char *)text : ""
        );

        if (text)
            xmlFree(text);
    }

    xmlXPathFreeObject(result);
    xmlXPathFreeContext(context);
    xmlFreeDoc(doc);

    return 0;
}
