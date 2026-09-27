#include <stdio.h>
#include <libxml/HTMLparser.h>

int main()
{
    htmlDocPtr doc;

    doc = htmlReadFile("download.html", NULL, HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);

    if (doc == NULL)
    {
        printf("Failed to open HTML file\n");
        return 1;
    }

    printf("HTML file parsed successfully\n");

    xmlFreeDoc(doc);
    xmlCleanupParser();

    return 0;
}
