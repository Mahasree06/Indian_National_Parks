# Indian National Parks Database

A C-based multithreaded data processing and SQLite database project for collecting, parsing, storing, and querying information about Indian National Parks.

## Project Overview

This project demonstrates:

- Web data extraction
- HTML parsing using libxml2
- Multithreading using POSIX pthreads
- Producer-consumer architecture
- Thread-safe queue
- CSV data generation
- SQLite database creation and querying
- Interactive command-line database menu
- Makefile-based compilation
- Git and GitHub version control

## Technologies Used

- C
- POSIX Threads (pthread)
- libcurl
- libxml2
- SQLite3
- Linux / Ubuntu
- Git
- GitHub
- Make

## Project Features

### 1. Data Collection

The project processes national park information from web-based data.

### 2. HTML Parsing

HTML data is parsed using:

- libxml2
- XPath

The required information includes:

- Park name
- State/UT
- Location
- Formed year
- Notable features
- Flora/Fauna
- Rivers/Lakes

### 3. Multithreading

The project uses POSIX threads to process park data concurrently.

The producer-consumer architecture uses a thread-safe queue for communication between threads.

### 4. CSV Generation

Processed park information is stored in CSV format.

### 5. SQLite Database

The collected data is stored in an SQLite database.

Database table:

`national_parks`

### 6. Interactive Database Menu

The database program provides:

1. List all parks
2. Search park
3. Search by state
4. Count parks by state
5. Search parks by formed year
6. Database statistics
7. Search by flora/fauna
8. Search by rivers/lakes
9. Search parks by location
10. Exit

## Database Statistics

Current database contains:

- 110 national parks
- 30 states/UTs
- Earliest park year: 1955
- Latest park year: 2025

## Project Structure

```text
Indian_National_Parks/
│
├── data/
├── include/
│   ├── national_park.h
│   └── queue.h
│
├── sql/
│   ├── create_database.sql
│   └── national_parks.db
│
├── src/
│   ├── db_query.c
│   ├── main.c
│   ├── queue.c
│   ├── parse_parks.c
│   ├── table_context.c
│   ├── find_tables.c
│   ├── inspect_row.c
│   ├── inspect_tables.c
│   └── test_*.c
│
├── Makefile
├── project_data.csv
├── download.html
├── thread_output.csv
└── README.md
