#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>
//#include <ctype.h>
#include <assert.h>

using namespace std;

#define ERROR(a) { cerr << "ERROR: " << a << endl; return -1; }

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

std::ostream& operator<<(std::ostream& os, const HourMin& hm)
{
   return os << (hm.h / 10) << (hm.h % 10) << ':' << (hm.m / 10) << (hm.m % 10);
}

std::ostream& operator<<(std::ostream& os, const YearMon& hm)
{
   return os << (hm.y / 10) << (hm.y % 10) << ':' << (hm.m / 10) << (hm.m % 10);
}

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

std::string loadStrLine(std::istream& is)
{
   string str;
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

std::string lowerCase(const std::string& s)
{
   std::string r = s;
   for (size_t i = 0; i < r.size(); ++i)
      if ((r[i] >= 'A') && (r[i] <= 'Z'))
         r[i] = (char)(r[i] - 'A' + 'a');
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
   oss << fixed << minutes / 60.0 << " h";

   return expandStr(oss.str(), size);
}

// incSeparators separate but are included in the output (not eaten)
std::vector<std::string> splitIntoTokens(const std::string& s, const char* separators, const char* incSeparators="")
{
   std::vector<std::string> r;
   std::string ss = s;
   do
   {
      size_t i = ss.find_first_of(separators);
      size_t ii = ss.find_first_of(incSeparators);
      assert( ii == string::npos || ii!=i );
      if(ii<i && ii>0)
      {
         assert(ii!=string::npos);
         string x = ss.substr(0, ii);
         if (!x.empty())
            r.push_back(x);
         ss = ss.substr(ii);
      }
      else if (i == string::npos)
      {
         if (!ss.empty())
            r.push_back(ss);
         break;
      }
      else
      {
         string x = ss.substr(0, i);
         if (!x.empty())
            r.push_back(x);
         ss = ss.substr(i + 1);
      }
   }
   while (1);
   return r;
}

bool parseUnsigned(unsigned* result, const std::string& s)
{
   unsigned r = 0;
   for (size_t i = 0; i < s.size(); ++i)
   {
      if (!isdigit(s[i]))
         return false;
      r = r * 10u + (unsigned)(s[i] - '0');        /// @todo DP: handle overflow
   }
   if (result != NULL)
      *result = r;
   return true;
}

bool parseHourMin(HourMin* result, const std::string& s)
{
   std::vector<std::string> ss = splitIntoTokens(s, ".:");
   if (ss.size() != 2)
      return false;
   unsigned h,m;
   if (!(parseUnsigned(&h, ss[0]) && parseUnsigned(&m, ss[1])))
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
      map<string, unsigned> s; // minutesPerTypeOfWork;
      typedef map<string, unsigned>::iterator iterator;
      typedef map<string, unsigned>::const_iterator const_iterator;

      void update(const string& tow, unsigned minutes)
      {
         map<string, unsigned>::iterator mi = s.find(tow);
         if (mi != s.end())
            mi->second += minutes;
         else
            s[tow] = minutes;
      }

      unsigned calcTotal() const
      {
         unsigned r = 0;
         for (const_iterator i = s.begin(); i != s.end(); ++i)
            r += i->second;
         return r;
      }
   };

   struct PersonStats
   {
      map<YearMon, PersonMonStats, greater<YearMon> > s;
      PersonMonStats workTypesForAll;
      typedef map<YearMon, PersonMonStats, greater<YearMon> >::iterator iterator;
      typedef map<YearMon, PersonMonStats, greater<YearMon> >::const_iterator const_iterator;

      void update(const YearMon& ym, const string& tow, unsigned minutes)
      {
         s[ym].update(tow, minutes);
         workTypesForAll.update(tow, minutes);
      }
   };

   map<string, PersonStats> s;
   PersonStats statsForWorkTypesForAll;
   typedef map<string, PersonStats>::iterator iterator;
   typedef map<string, PersonStats>::const_iterator const_iterator;

   void update(const std::string& person, const YearMon& ym, const std::string& typeOfWork, unsigned minutes)
   {
      s[person].update(ym, typeOfWork, minutes);
      statsForWorkTypesForAll.update(ym, typeOfWork, minutes);
   }
};


int main(int argc, char* argv[])
{
   string filename = "hours.txt";          // A default filename.
   if (argc > 1)
      filename = argv[1];

   ifstream ifs(filename.c_str());
   if (!ifs.is_open())
      ERROR("Unable to open file \"" << filename << "\"");

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

   std::ostream& oss = ofs;
   std::ostream& oss2 = ofilename2 != ofilename ? ofs2 : ofs;
   //std::ostream& oss = cout;
   //std::ostream& oss2 = cout;

   oss << "Hours calculator" << endl;
   oss << "   Calculating only from year 2010." << endl;

   //typedef map<string, unsigned> MinutesMap;
   MinutesMap minutesMap;

   typedef map<pair<string, string>, unsigned> DMap;  // <person, day> - minutes
   DMap dayMinutes;

   int numLines = 0;
   int numCommentLines = 0;
   bool expectsNewDate = true;
   bool expectsHourLines = false;
   YearMon curYearMon;
   unsigned curDay = 0;
   string curDayStr;
   while (!ifs.eof())
   {
      string line = loadStrLine(ifs);
      ++numLines;

      if (line.substr(0, 2) == "//")
      {
         ++numCommentLines ;
         continue;
      }

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
         vector<string> ss = splitIntoTokens(line, ".: \t");
         //for (size_t i = 0; i < ss.size(); ++i)
         //   cout << "<" << ss[i] << "> ";
         //cout << endl;

         if (ss.size() >= 3)
         {
            unsigned year,mon,day;
            if (parseUnsigned(&year, ss[0]) && parseUnsigned(&mon, ss[1]) && parseUnsigned(&day, ss[2]))
            {
//               cout << "date: " << year << "." << mon << "." << day << endl;
               // Properly recognized date.
               expectsNewDate = false;
               expectsHourLines = true;

               curYearMon = YearMon(year, mon);
               curDay = day;
               curDayStr = line;

               if (year < 2010)
                  break;
            }
         }
         continue;
      }

      if (expectsHourLines)
      {
         vector<string> ss = splitIntoTokens(line, " \t", "[/");
         size_t n = ss.size();
         if (n < 3)
         {
            oss << "WARNING: Wrong hours in line " << numLines << " (" << curYearMon << "." << curDay << "):   \"" << line << "\"" << endl;
            continue;
         }

         string person = ss[0];
         if (person[person.size() - 1] == ':')
            person = person.substr(0, person.size() - 1);
         person = lowerCase(person);

//         cout << "   " << person << "   :::   ";

         vector<HourMin> hms;
         size_t i = 1;
         while (i < ss.size())
         {
            if(!isdigit(ss[i][0]))
            {
               if(ss[i][0]=='[' ||
                  ( ss[i][0]='/' && ss[i].size()>=2 && ss[i][1]=='/' ) // //
                 )
               {
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

         unsigned totalMinutesForLine = 0;

         for (size_t j = 0; j + 1 < hms.size(); j += 2)
         {
            HourMin hm1 = hms[j];
            HourMin hm2 = hms[j + 1];

//            cout << hm1 << " -> " << hm2;
            unsigned minutes = hm2 - hm1;
//            cout << "   total: " << (minutes / 60) << "h" << (minutes % 60) << "m   (" << minutes << " m)";

            totalMinutesForLine += minutes;
         }
         if ((hms.size() % 2) != 0)
         {
            oss << "WARNING: Wrong hours in line " << numLines << " (" << curYearMon << "." << curDay << ") (odd number of moments):   \"" << line << "\"" << endl;
            continue;
         }

         string typeOfWork;
         if (i != ss.size())
         {
            if ((ss[i][0] == '[') && (ss[i][ ss[i].size() - 1 ] == ']'))
               typeOfWork = lowerCase( ss[i].substr(1, ss[i].size() - 2) );
         }
         vector<string> typesOfWork = splitIntoTokens(typeOfWork, "\\/ ");
         if (typesOfWork.empty())
            typesOfWork.push_back("UNASSIGNED");
         typeOfWork.clear();
         sort(typesOfWork.begin(), typesOfWork.end());
         for (size_t k = 0; k < typesOfWork.size(); ++k)
         {
            if (!typeOfWork.empty())
               typeOfWork.push_back('/');
            typeOfWork += typesOfWork[k];
         }
         //cout << "     [[[ ";
         //for (size_t k = 0; k < typesOfWork.size(); ++k)
         //   cout << typesOfWork[k] << " ";
         //cout << "]]]";
//         cout << "     [[[ " << typeOfWork << " ]]]";

//         cout << endl;

         minutesMap.update(person, curYearMon, typeOfWork, totalMinutesForLine);

         if(dayMinutes.find(make_pair(person, curDayStr))== dayMinutes.end())
            dayMinutes[make_pair(person, curDayStr)]=totalMinutesForLine;
         else
            dayMinutes[make_pair(person, curDayStr)]+=totalMinutesForLine;
      }
   }

   oss << "number of lines: " << numLines << " (including " << numCommentLines  << " comment lines)" << endl;

   // All people.
   for (MinutesMap::const_iterator pi = minutesMap.s.begin(); pi != minutesMap.s.end(); ++pi)
   {
      oss << endl;
      oss << pi->first << ":" << endl;

      // All months per person.
      for (MinutesMap::PersonStats::const_iterator pmi = pi->second.s.begin(); pmi != pi->second.s.end(); ++pmi)
      {
         // All work types per month (per person).
         oss << "   " << pmi->first << ":" << endl;
         for (MinutesMap::PersonMonStats::const_iterator mi = pmi->second.s.begin(); mi != pmi->second.s.end(); ++mi)
            oss << "      " << expandStr(mi->first + ':', 12) << " " << hoursMinsStr(mi->second, 12) << "   ( " << mi->second << " m )" << endl;
         unsigned total = pmi->second.calcTotal();
         oss << "      " << expandStr("*:", 12) << " " << hoursMinsStr(total, 12) << "   ( " << total << " m )" << endl;
      }

      {
         // All work types for all months.
         oss << "   all:" << endl;
         for (MinutesMap::PersonMonStats::const_iterator mi = pi->second.workTypesForAll.s.begin(); mi != pi->second.workTypesForAll.s.end(); ++mi)
            oss << "      " << expandStr(mi->first + ':', 12) << " " << hoursMinsStr(mi->second, 12) << "   ( " << mi->second << " m )" << endl;
         unsigned total = pi->second.workTypesForAll.calcTotal();
         oss << "      " << expandStr("*:", 12) << " " << hoursMinsStr(total, 12) << "   ( " << total << " m )" << endl;
      }
      //unsigned minutes = mi->second;
      //cout << mi->first << ":   " << (minutes / 60) << "h" << (minutes % 60) << "m   (" << minutes << " m)" << endl;
   }

   /// For all people combined.
   {
      oss << endl;
      oss << "all crew combined" << ":" << endl;

      // All months per person.
      for (MinutesMap::PersonStats::const_iterator pmi = minutesMap.statsForWorkTypesForAll.s.begin(); pmi != minutesMap.statsForWorkTypesForAll.s.end(); ++pmi)
      {
         // All work types per month (per person).
         oss << "   " << pmi->first << ":" << endl;
         for (MinutesMap::PersonMonStats::const_iterator mi = pmi->second.s.begin(); mi != pmi->second.s.end(); ++mi)
            oss << "      " << expandStr(mi->first + ':', 12) << " " << hoursMinsStr(mi->second, 12) << "   ( " << mi->second << " m )" << endl;
         unsigned total = pmi->second.calcTotal();
         oss << "      " << expandStr("*:", 12) << " " << hoursMinsStr(total, 12) << "   ( " << total << " m )" << endl;
      }

      {
         // All work types for all months.
         oss << "   all:" << endl;
         for (MinutesMap::PersonMonStats::const_iterator mi = minutesMap.statsForWorkTypesForAll.workTypesForAll.s.begin();
              mi != minutesMap.statsForWorkTypesForAll.workTypesForAll.s.end(); ++mi)
            oss << "      " << expandStr(mi->first + ':', 12) << " " << hoursMinsStr(mi->second, 12) << "   ( " << mi->second << " m )" << endl;
         unsigned total = minutesMap.statsForWorkTypesForAll.workTypesForAll.calcTotal();
         oss << "      " << expandStr("*:", 12) << " " << hoursMinsStr(total, 12) << "   ( " << total << " m )" << endl;
      }
   }

   oss << endl;

   oss2<<"Person\tDay\tHours\n";
   for(DMap::iterator it = dayMinutes.begin(); it!=dayMinutes.end(); ++it)
   {
      oss2 <<it->first.first<<"\t" << it->first.second << "\t" << fixed << setprecision(9)<< it->second / 60.0 << std::endl;
   }
   //system("PAUSE");  // does not help

   return 0;
}
