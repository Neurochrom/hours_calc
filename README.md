# About
hours_calc is a simple utility to summarize simple text based timessheets.
It was designed for small teams that want to track each memebers commintment.
The timesheet file can have data for many people. You can use an vcs like git or an online document collaboration tool like google docs to manage it.

hours_calc timesheet files shold look something like the one blow:

```
2017-10-02
  Joe:  9:10 14:20  15:30  16:20  [TESTING] I tested the app on Windows
  Alice: 8:00  14:20 [TESTING,DEVELOPMENT] Fixed a few bugs in the Mac Version
  Alice: 18:20 20:20  [DEVELOPMENT]

2017-10-01
  Joe:  10:00 15:20  [DEVELOPMENT]
  Alice:  12:00  14:40 [DEVELOPMENT]
  Alice:  14:40  16:00  doctor visit  20:00 26:00 [TESTING] Found many bugs
```

If you name the file hours.txt and put it beside the executable you don't even need to use any additional command line paramaters.
In case you need them though:

```
Usage:
 hours_calc [input_filename [main_txt_stats_output_filename [csv_stats_output_filename [cutoff_year]]]]
 By default:
  input_filename = hours.txt
  main_txt_stats_output_filename = hour_stats.txt
  csv_stats_output_filename = hour_stats.csv
  cutoff_year = 2010
```

The utility generates two files:
hour_stats.txt
```
Hours calculator
   Calculating only from year 2010
number of lines: 9

ALICE:
   2017-10:
      DEVELOPMENT: 7.83 h         ( 470 m )
      TESTING:     10.50 h        ( 630 m )
      *:           18.33 h        ( 1100 m )
   all time sum:
      DEVELOPMENT: 7.83 h         ( 470 m )
      TESTING:     10.50 h        ( 630 m )
      *:           18.33 h        ( 1100 m )

JOE:
   2017-10:
      DEVELOPMENT: 5.33 h         ( 320 m )
      TESTING:     6.00 h         ( 360 m )
      *:           11.33 h        ( 680 m )
   all time sum:
      DEVELOPMENT: 5.33 h         ( 320 m )
      TESTING:     6.00 h         ( 360 m )
      *:           11.33 h        ( 680 m )

The whole crew combined:
   2017-10:
      DEVELOPMENT: 13.17 h        ( 790 m )
      TESTING:     16.50 h        ( 990 m )
      *:           29.67 h        ( 1780 m )
   all time sum:
      DEVELOPMENT: 13.17 h        ( 790 m )
      TESTING:     16.50 h        ( 990 m )
      *:           29.67 h        ( 1780 m )
```
and a hour_stats.csv:
```
Person	Day	Hours
ALICE	2017-10-01	10.000000000
ALICE	2017-10-02	8.333333333
JOE	2017-10-01	5.333333333
JOE	2017-10-02	6.000000000
```

The tool is not picky with the timesheet format:
* You can use // and # style comments.
* You can ommit the : after the name.
* You can ommit the category within [ ] after each entry.
* You can enter multiple categories within [ ] separated by / or , (but no spaces). Time will be split equally between each category.
* All lines that are text only will also be treated like comments.
* Indentation is up to you **however an empty line indicates that a new date entry should be comming**.
* Numbers within dates can be separated with - or . and you can tag dates with days of the week or anything else like '2016-12-24 Xmass'.
* You can go past midnight, the tool understands entries like 26:00 (2 am the next day) - awesome for people that stay up late.


# Building

For Windows just get Visual Studio 2017 (there is a free and excellent community edition) and open up the .sln file.

For unixish systems with g++ use make.
