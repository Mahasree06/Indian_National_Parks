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
        printf("Failed to open HTML file\n");
        return 1;
    }

    context = xmlXPathNewContext(doc);

    if (context == NULL)
    {
        printf("Failed to create XPath context\n");
        xmlFreeDoc(doc);
        return 1;
    }

    result = xmlXPathEvalExpression(
        (xmlChar *)"//table",
        context
    );

    if (result == NULL)
    {
        printf("Failed to find tables\n");
    }
    else
    {
        printf("Number of tables found: %d\n",
               result->nodesetval->nodeNr);
    }

    xmlXPathFreeObject(result);
    xmlXPathFreeContext(context);
    xmlFreeDoc(doc);
    xmlCleanupParser();

    return 0;
}
