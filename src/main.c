#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "../include/queue.h"

#define TOTAL_RECORDS 110

SharedQueue queue;


/* =========================
   PRODUCER THREAD
   ========================= */

void *producer_thread(void *arg)
{
    FILE *fp;
    char line[5000];
    int count = 0;

    fp = fopen("project_data.csv", "r");

    if (fp == NULL)
    {
        perror("project_data.csv");
        return NULL;
    }

    /* Skip CSV header */
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        ParkRecord record;

        memset(&record, 0, sizeof(ParkRecord));

        /*
         * Read all 7 CSV columns
         */
        sscanf(
            line,
            "\"%255[^\"]\",\"%99[^\"]\",\"%255[^\"]\",\"%19[^\"]\",\"%999[^\"]\",\"%999[^\"]\",\"%999[^\"]\"",
            record.park_name,
            record.state_ut,
            record.location,
            record.formed_year,
            record.notable_features,
            record.flora_fauna,
            record.rivers_lakes
        );

        queue_push(&queue, record);

        count++;

        printf(
            "Producer: %s | %s\n",
            record.park_name,
            record.state_ut
        );
    }

    fclose(fp);

    printf(
        "\nProducer finished. Records: %d\n",
        count
    );

    return NULL;
}


/* =========================
   CSV THREAD
   ========================= */

void *csv_thread(void *arg)
{
    FILE *fp;
    int i;

    fp = fopen("thread_output.csv", "w");

    if (fp == NULL)
    {
        perror("thread_output.csv");
        return NULL;
    }

    /*
     * Write CSV header
     */
    fprintf(
        fp,
        "park_name,state_ut,location,formed_year,notable_features,flora_fauna,rivers_lakes\n"
    );

    /*
     * Get records from queue
     * and write them to CSV
     */
    for (i = 0; i < TOTAL_RECORDS; i++)
    {
        ParkRecord record;

        record = queue_pop(&queue);

        fprintf(
            fp,
            "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n",
            record.park_name,
            record.state_ut,
            record.location,
            record.formed_year,
            record.notable_features,
            record.flora_fauna,
            record.rivers_lakes
        );
    }

    fclose(fp);

    printf(
        "CSV thread finished. Records written: %d\n",
        TOTAL_RECORDS
    );

    return NULL;
}


/* =========================
   MAIN
   ========================= */

int main()
{
    pthread_t producer;
    pthread_t csv;

    /*
     * Initialize queue
     */
    queue_init(&queue);

    /*
     * Create producer thread
     */
    pthread_create(
        &producer,
        NULL,
        producer_thread,
        NULL
    );

    /*
     * Create CSV writer thread
     */
    pthread_create(
        &csv,
        NULL,
        csv_thread,
        NULL
    );

    /*
     * Wait for producer
     */
    pthread_join(
        producer,
        NULL
    );

    /*
     * Wait for CSV thread
     */
    pthread_join(
        csv,
        NULL
    );

    printf(
        "\nAll threads completed successfully.\n"
    );

    return 0;
}
