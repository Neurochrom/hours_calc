#include <assert.h>

#include <algorithm>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

struct HourMin
{
   unsigned h, m;

   HourMin() {}
   HourMin(unsigned h, unsigned m) : h(h), m(m) {}

   bool operator<(const HourMin& other) const
   {
      if (h < other.h)
         return true;
      if (h > other.h)
         return false;
      return m < other.m;
   }
};

struct YearMon
{
   unsigned y, m;

   YearMon() {}
   YearMon(unsigned y, unsigned m) : y(y), m(m) {}

   bool operator<(const YearMon& other) const
   {
      if (y < other.y)
         return true;
      if (y > other.y)
         return false;
      return m < other.m;
   }

   bool operator>(const YearMon& other) const { return other < *this; }
};

unsigned operator-(const HourMin& hmLater, const HourMin& hmSooner)
{
   HourMin hmLater2 = hmLater;
   if (hmLater2 < hmSooner)
      hmLater2.h += 24;
   unsigned r = (hmLater2.h - hmSooner.h) * 60;
   if (hmLater2.m >= hmSooner.m)
      r += hmLater2.m - hmSooner.m;
   else
      r -= hmSooner.m - hmLater2.m;
   return r;
}

std::ostream& operator<<(std::ostream& os, const YearMon& hm)
{
   return os << (hm.y / 10) << (hm.y % 10) << '-' << (hm.m / 10) << (hm.m % 10);
}

std::string loadStrLine(std::istream& is)
{
   std::string str;
   getline(is, str);
   return str;
}

bool isEmptyOrWhitespace(const std::string& s)
{
   for (size_t i = 0; i < s.size(); ++i)
      if (!isspace(s[i]))
         return false;
   return true;
}

/// @todo handle UTF-8
std::string lowerCase(const std::string& s)
{
   std::string r = s;
   for (size_t i = 0; i < r.size(); ++i)
      if ((r[i] >= 'A') && (r[i] <= 'Z'))
         r[i] = (char)(r[i] - 'A' + 'a');
   return r;
}

/// @todo handle UTF-8
std::string upperCase(const std::string& s)
{
    std::string r = s;
    for (size_t i = 0; i < r.size(); ++i)
        if ((r[i] >= 'a') && (r[i] <= 'z'))
            r[i] = (char)(r[i] - 'a' + 'A');
    return r;
}


std::string expandStr(const std::string& str, size_t size)
{
   size_t strLen = str.length();
   if (strLen >= size)
      return str;
   else
      return str + std::string(size - strLen, ' ');
}

std::string hoursMinsStr(unsigned minutes, size_t size = 0)
{
   std::ostringstream oss;
   oss.precision(2);
   oss << std::fixed << minutes / 60.0 << " h";

   return expandStr(oss.str(), size);
}

// incSeparators separate but are included in the output (not eaten)
std::vector<std::string> splitIntoTokens(const std::string& s, const char* separators, const char* incSeparators="")
{
   std::vector<std::string> r;
   std::string ss = s;
   for(;;)
   {
      size_t i = ss.find_first_of(separators);
      size_t ii = ss.find_first_of(incSeparators);
      assert( ii == std::string::npos || ii!=i );
      if(ii<i && ii>0)
      {
         assert(ii!=std::string::npos);
         std::string x = ss.substr(0, ii);
         if (!x.empty())
            r.push_back(x);
         ss = ss.substr(ii);
      }
      else if (i == std::string::npos)
      {
         if (!ss.empty())
            r.push_back(ss);
         break;
      }
      else
      {
         std::string x = ss.substr(0, i);
         if (!x.empty())
            r.push_back(x);
         ss = ss.substr(i + 1);
      }
   }
   return r;
}

bool parseHourMin(HourMin* result, const std::string& s)
{
   std::vector<std::string> ss = splitIntoTokens(s, ".:");
   if (ss.size() != 2)
      return false;
   unsigned h,m;
   try
   {
      h = std::stoi(ss[0]);
      m = std::stoi(ss[1]);
   }
   catch (std::exception&)
   {
      return false;
   }
   if (m > 59)
      return false;
   if (h > 47)  // We shall allow 26 instead of 2am the next day but there is a limit
      return false;
   if (result != 0)
      *result = HourMin(h, m);
   return true;
}

class MinutesMap
{
public:
   struct PersonMonStats
   {
      std::map<std::string, unsigned> s;  // workType - minutes

      void update(const std::string& tow, unsigned minutes)
      {
         auto mi = s.find(tow);
         if (mi != s.end())
            mi->second += minutes;
         else
            s[tow] = minutes;
      }

      unsigned calcTotal() const
      {
         unsigned sum = 0;
         for (auto& it : s)
            sum += it.second;
         return sum;
      }
   };

   struct PersonStats
   {
      std::map<YearMon, PersonMonStats, std::greater<YearMon>> s;
      PersonMonStats workTypesForAll;

      void update(const YearMon& ym, const std::string& tow, unsigned minutes)
      {
         s[ym].update(tow, minutes);
         workTypesForAll.update(tow, minutes);
      }
   };

   std::map<std::string, PersonStats> s;
   PersonStats statsForWorkTypesForAll;

   void update(const std::string& person, const YearMon& ym, const std::string& typeOfWork, unsigned minutes)
   {
      s[person].update(ym, typeOfWork, minutes);
      statsForWorkTypesForAll.update(ym, typeOfWork, minutes);
   }
};


void skipUTF8BOM(std::ifstream* ifs)
{
   char c[3];
   ifs->read(c, 3);
   if (ifs->bad() || c[0] != (char)0xef || c[1] != (char)0xbb || c[2] != (char)0xbf)
      ifs->seekg(0);
}

using namespace std;
#define ERROR(a) { cerr << "ERROR: " << a << endl; return -1; }

int main(int argc, char* argv[])
{
   try {
      string filename = "hours.txt";
      if (argc > 1)
      {
         if (argv[1][0] == '-' || argv[1][0] == '/')
         {
            cout << "Usage:\n"
                    " hours_calc [input_filename [main_txt_stats_output_filename [csv_stats_output_filename [cutoff_year]]]]\n"
                    " By default:\n"
                    "  input_filename = hours.txt\n"
                    "  main_txt_stats_output_filename = hour_stats.txt\n"
                    "  csv_stats_output_filename = hour_stats.csv\n"
                    "  cutoff_year = 2010" << endl;
            return 0;
         }
         filename = argv[1];
      }
      ifstream ifs(filename.c_str());
      if (!ifs.is_open())
         ERROR("Unable to open file \"" << filename << "\"");

      skipUTF8BOM(&ifs);

      string ofilename = "hour_stats.txt";   // Also a default.
      if (argc > 2)
         ofilename = argv[2];
      ofstream ofs(ofilename.c_str());
      if (!ofs.is_open())
         ERROR("Unable to open file \"" << ofilename << "\"");

      string ofilename2 = "hour_stats.csv";   // Also a default.
      if (argc > 3)
         ofilename2 = argv[3];
      ofstream ofs2;
      if(ofilename2 != ofilename)
      {
         ofs2.open(ofilename2.c_str());
         if (!ofs2.is_open())
            ERROR("Unable to open file \"" << ofilename2 << "\"");
      }

      unsigned cutoffYear = 2010;
      if (argc > 4)
         cutoffYear = stoi(string(argv[4]));

      std::ostream& oss = ofs;
      std::ostream& oss2 = ofilename2 != ofilename ? ofs2 : ofs;

      oss << "Hours calculator" << endl;
      oss << "   Calculating only from year " << cutoffYear << endl;

      MinutesMap minutesMap;

      typedef map<pair<string, string>, unsigned> DMap;  // <person, day> - minutes
      DMap dayMinutes;

      unsigned numLines = 0;
      bool expectsNewDate = true;
      bool expectsHourLines = false;
      YearMon curYearMon;
      unsigned curDay = 0;
      string curDayStr;
      while (!ifs.eof())
      {
         string line = loadStrLine(ifs);
         ++numLines;

         if (isEmptyOrWhitespace(line))
         {
            // Empty lines separate dates.
            expectsNewDate = true;
            expectsHourLines = false;
            continue;
         }

         assert(!(expectsNewDate & expectsHourLines));

         if (expectsNewDate)
         {
            vector<string> ss = splitIntoTokens(line, ".:- \t");

            if (ss.size() >= 3)
            {
               unsigned year,mon,day;
               try
               {
                  year = stoi(ss[0]);
                  mon = stoi(ss[1]);
                  day = stoi(ss[2]);
               }
               catch (exception)
               {
                  continue;
               }

               expectsNewDate = false;
               expectsHourLines = true;

               curYearMon = YearMon(year, mon);
               curDay = day;
               curDayStr = line;
               while (isspace(curDayStr.back()))
                  curDayStr.pop_back();

               if (year < 2010)
                  break;
            }

            continue;
         }

         if (expectsHourLines)
         {
            vector<string> ss = splitIntoTokens(line, " \t", "[/");

            // something without a name and 2 hour:minute entries can only be a comment
            if (ss.size() < 3)
               continue;

            string person = ss[0];

            // Something starting with punctuation or a number etc. is not a name - assume a comment line
            if ( person[0] > ' ' && person[0] < 'A')
               continue;

            // Remove the recommended but optional : behind the name
            if (person[person.size() - 1] == ':')
               person = person.substr(0, person.size() - 1);
            person = upperCase(person);

            vector<HourMin> hms;
            size_t i = 1;
            while (i < ss.size())
            {
               if(!isdigit(ss[i][0]))
               {
                  if (ss[i][0] == '[' ||
                      ss[i][0] == '#' ||
                      (ss[i][0]=='/' && ss[i].size()>=2 && ss[i][1]=='/')  // //
                     )
                  {
                     // A comment or category from here on.
                     break;
                  }
                  else
                  {
                     ++i;
                     continue;
                  }
               }
               HourMin hm;
               if (!parseHourMin(&hm, ss[i]))
                  break;
               hms.push_back(hm);
               ++i;
            }

            // Skip the lines with 0 valid time entries.
            if (!hms.size())
               continue;

            unsigned totalMinutesForLine = 0;

            for (size_t j = 0; j + 1 < hms.size(); j += 2)
            {
               HourMin hm1 = hms[j];
               HourMin hm2 = hms[j + 1];
               unsigned minutes = hm2 - hm1;
               totalMinutesForLine += minutes;
            }
            if ((hms.size() % 2) != 0)
            {
               oss << "WARNING: Uneven number of time entries for " << curYearMon << "-" << curDay << " in line " << numLines << ": \"" << line << "\"" << endl;
               continue;
            }

            string typeOfWork;
            if (i != ss.size())
            {
               if ((ss[i][0] == '[') && (ss[i][ ss[i].size() - 1 ] == ']'))
                  typeOfWork = upperCase( ss[i].substr(1, ss[i].size() - 2) );
            }
            vector<string> typesOfWork = splitIntoTokens(typeOfWork, "\\/,");
            if (typesOfWork.empty())
               typesOfWork.push_back("UNASSIGNED");
            typeOfWork.clear();

#ifdef DO_NOT_SPLIT_TYPES_OF_WORK
            sort(typesOfWork.begin(), typesOfWork.end());
            for (size_t k = 0; k < typesOfWork.size(); ++k)
            {
               if (!typeOfWork.empty())
                  typeOfWork.push_back('/');
               typeOfWork += typesOfWork[k];
            }
            minutesMap.update(person, curYearMon, typeOfWork, totalMinutesForLine);
#else
            double iTypesOfWorkSize;
            iTypesOfWorkSize = 1.0 / typesOfWork.size();
            for (auto& tow : typesOfWork)
                minutesMap.update(person, curYearMon, tow, unsigned(totalMinutesForLine * iTypesOfWorkSize));
#endif
            if (dayMinutes.find(make_pair(person, curDayStr)) == dayMinutes.end())
               dayMinutes[make_pair(person, curDayStr)] = totalMinutesForLine;
            else
               dayMinutes[make_pair(person, curDayStr)] += totalMinutesForLine;
         }
      }

      oss << "number of lines: " << numLines << endl;

      // All people.
      for (auto pi = minutesMap.s.cbegin(); pi != minutesMap.s.cend(); ++pi)
      {
         oss << endl;
         oss << pi->first << ":" << endl;

         // All months per person.
         for (auto pmi = pi->second.s.cbegin(); pmi != pi->second.s.cend(); ++pmi)
         {
            // All work types per month (per person).
            oss << "   " << pmi->first << ":" << endl;
            for (auto mi = pmi->second.s.begin(); mi != pmi->second.s.end(); ++mi)
               oss << "      " << expandStr(mi->first + ':', 12) << " " << hoursMinsStr(mi->second, 12) << "   ( " << mi->second << " m )" << endl;
            unsigned total = pmi->second.calcTotal();
            oss << "      " << expandStr("*:", 12) << " " << hoursMinsStr(total, 12) << "   ( " << total << " m )" << endl;
         }

         {
            // All work types for all months.
            oss << "   all time sum:" << endl;
            for (auto mi = pi->second.workTypesForAll.s.cbegin(); mi != pi->second.workTypesForAll.s.cend(); ++mi)
               oss << "      " << expandStr(mi->first + ':', 12) << " " << hoursMinsStr(mi->second, 12) << "   ( " << mi->second << " m )" << endl;
            unsigned total = pi->second.workTypesForAll.calcTotal();
            oss << "      " << expandStr("*:", 12) << " " << hoursMinsStr(total, 12) << "   ( " << total << " m )" << endl;
         }
      }

      /// For all people combined.
      {
         oss << endl;
         oss << "The whole crew combined" << ":" << endl;

         // All months per person.
         for (auto pmi = minutesMap.statsForWorkTypesForAll.s.begin(); pmi != minutesMap.statsForWorkTypesForAll.s.end(); ++pmi)
         {
            // All work types per month (per person).
            oss << "   " << pmi->first << ":" << endl;
            for (auto mi = pmi->second.s.begin(); mi != pmi->second.s.end(); ++mi)
               oss << "      " << expandStr(mi->first + ':', 12) << " " << hoursMinsStr(mi->second, 12) << "   ( " << mi->second << " m )" << endl;
            unsigned total = pmi->second.calcTotal();
            oss << "      " << expandStr("*:", 12) << " " << hoursMinsStr(total, 12) << "   ( " << total << " m )" << endl;
         }

         {
            // All work types for all months.
            oss << "   all time sum:" << endl;
            for (auto mi = minutesMap.statsForWorkTypesForAll.workTypesForAll.s.begin();
                 mi != minutesMap.statsForWorkTypesForAll.workTypesForAll.s.end(); ++mi)
               oss << "      " << expandStr(mi->first + ':', 12) << " " << hoursMinsStr(mi->second, 12) << "   ( " << mi->second << " m )" << endl;
            unsigned total = minutesMap.statsForWorkTypesForAll.workTypesForAll.calcTotal();
            oss << "      " << expandStr("*:", 12) << " " << hoursMinsStr(total, 12) << "   ( " << total << " m )" << endl;
         }
      }

      oss << endl;

      oss2 << "Person\tDay\tHours\n";
      for (auto it = dayMinutes.begin(); it!=dayMinutes.end(); ++it)
      {
         oss2 <<it->first.first<<"\t" << it->first.second << "\t" << fixed << setprecision(9)<< it->second / 60.0 << endl;
      }

      return 0;
   }
   catch (std::exception& e)
   {
      ERROR(e.what());
   }
   catch (...)
   {
      ERROR("Unknown exception");
   }
}
