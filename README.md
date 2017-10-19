# About
hours_calc is a simple utility to summarize simple text based timessheets.
It was designed for small teams that want to track each memebers commintment.
The timesheet file can have data for many people. You can use an vcs like git or an online document collaboration tool like google docs to manage it.

hours_calc timesheet files shold look something like the one blow:

```
2017-10-02
  Joe:  9:10 14:20  15:30  16:20  [TESTING] I tested the app on Windows
  Alice: 8:00  14:20 [DEVELOPMENT] Fixed a few bugs in the Mac Version
  Alice: 18:20 20:20  [DEVELOPMENT]
  
2017-10-01
  Joe:  10:00 15:20  [DEVELOPMENT]
  Alice:  12:00  14:20 [DEVELOPMENT]
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
number of lines: 8

alice:
   2017-10:
      development: 10.67 h        ( 640 m )
      *:           10.67 h        ( 640 m )
   all time sum:
      development: 10.67 h        ( 640 m )
      *:           10.67 h        ( 640 m )

joe:
   2017-10:
      development: 5.33 h         ( 320 m )
      testing:     6.00 h         ( 360 m )
      *:           11.33 h        ( 680 m )
   all time sum:
      development: 5.33 h         ( 320 m )
      testing:     6.00 h         ( 360 m )
      *:           11.33 h        ( 680 m )

all crew combined:
   2017-10:
      development: 16.00 h        ( 960 m )
      testing:     6.00 h         ( 360 m )
      *:           22.00 h        ( 1320 m )
   all time sum:
      development: 16.00 h        ( 960 m )
      testing:     6.00 h         ( 360 m )
      *:           22.00 h        ( 1320 m )
```
and a hour_stats.csv:
```
Person	Day	Hours
alice	2017-10-01	2.333333333
alice	2017-10-02	8.333333333
joe	2017-10-01	5.333333333
joe	2017-10-02	6.000000000

```

The tool is not picky with the timesheet format:
* You can use // and # style comments
* You can ommit the : after the name
* You can ommit the category within [ ] after each entry
* All lines that are text only will also be treated like comments
* Indentation is up to you **however an empty line indicates that a new date entry should be comming**
* Numbers within dates can be separated with - or . and you can tag dates with days of the week or anything else like '2016-12-24 Xmass'
* You can go past midnight so the tool understands entries like 26:00 (2 am the next day) - awesome for people that stay up late


# Building

For Windows just get Visual Studio 2017 (there is a free and excelent community eddition) and open up the .sln file.

For unixish systems with g++ use make. 
