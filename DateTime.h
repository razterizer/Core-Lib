//
//  DateTime.h
//  Core
//
//  Created by Rasmus Anthin on 2022-11-08.
//

#pragma once
#include "StringHelper.h"
#include "Rand.h"
#include <array>
#include <map>
#include <iostream>

// #TODO: List:
// 1. Function now().
// 2. Unix time.
// 3. Use month lengths = { jan 31, feb 28, mar 31, apr 30, maj 31, jun 30, jul 31, aug 31, sep 30, oct 31, nov 30, dec 31 }.
// 4. Use leap years.
// 5. Add hundredths of a second.
// 6. Simplify the code.
// 7. Compatibility with date/time struct of STL/chrono.
// 8. Daylight saving.
// 9. Print with format.
// 10. Time zones.

//#define USE_REAL_DAYS_PER_MONTH

namespace datetime
{

  static const double second_eps = 1e-5;

  template<int N, int V0>
  class datetime_unit_t
  {
  protected:
    int val = V0;
    int carry = 0;
    bool tared = false;
    
    virtual int get_max_val() const { return N; }
    
  public:
    datetime_unit_t(int v) : val(v) { /*carry = normalize();*/ }
    datetime_unit_t(const datetime_unit_t& dtu) : val(dtu.val) {}
    
    void normalize()
    {
      int max_val = get_max_val();
      carry = 0;
      val -= V0;
      if (std::abs(val) >= max_val)
      {
        carry = val / max_val;
        val -= carry * max_val;
      }
      if (val < 0)
      {
        carry--;
        val += max_val;
      }
      val += V0;
    }
  
    datetime_unit_t operator+(int v) const
    {
      datetime_unit_t new_val(val);
      new_val += v;
      return new_val;
    }
  
    datetime_unit_t operator-(int v) const
    {
      datetime_unit_t new_val(val);
      new_val -= v;
      return new_val;
    }
  
    void operator+=(int v)
    {
      val += v;
      normalize();
    }
  
    void operator-=(int v)
    {
      *this += (-v);
    }
    
    void tare()
    {
      if (!tared)
      {
        val -= V0;
        tared = true;
      }
    }
    
    void untare()
    {
      if (tared)
      {
        val += V0;
        tared = false;
      }
    }
  
    int get_val() const { return val; }
    int get_carry() const { return carry; }
  
    operator const int&() const { return val; }
  
    bool in_range(int v_before, int v_after, Range type) const
    {
      //e.g. before = 21, after = 1, hour = 23 or 0 or 17.
      if (v_before <= v_after) // 0 .. 23
        return math::in_range<int>(val, v_before, v_after, type);
      else if (val > v_after)
        return math::in_range<int>(val, v_before, v_after + N, type);
      else if (val < v_before)
        return math::in_range<int>(val, v_before - N, v_after, type);
      return false;
    }
  };

  using month_t = datetime_unit_t<12, 1>;
  class day_t : public datetime_unit_t<30, 1>
  {
    std::array<int, 12> days_in_month { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    const month_t* month = nullptr;
    
  protected:
    virtual int get_max_val() const
    {
#ifdef USE_REAL_DAYS_PER_MONTH
      if (month != nullptr)
      {
        int idx = month->get_val() - 1;
        int max_val = days_in_month[idx];
        return max_val;
      }
#endif
      return datetime_unit_t::get_max_val();
    }
  
  public:
    day_t(const month_t& m, int day) : datetime_unit_t(day), month(&m) {}
    day_t(const month_t& m, const day_t& day) : datetime_unit_t(day), month(&m) {}
    
    void operator=(const day_t& day)
    {
      val = day.val;
      carry = day.carry;
      tared = day.tared;
    }
    
    void operator=(int day)
    {
      val = day;
    }
  };
  //using day_t = datetime_unit_t<30, 1>;
  using hour_t = datetime_unit_t<24, 0>;
  using sexagesimal_t = datetime_unit_t<60, 0>;

  // ////////////////////////////////////////////////

  struct Date
  {
    int year = 0;
    month_t month = 0;
    day_t day { month, 0 };
    
    Date() = default;
    Date(int y, int m, int d) : year(y), month(m), day(month, d) {}
    
    void tare()
    {
      month.tare();
      day.tare();
    }
    
    void untare()
    {
      month.untare();
      day.untare();
    }
    
    void normalize()
    {
      day.normalize();
      month = month.get_val() + day.get_carry();
      month.normalize();
      year += month.get_carry();
    }
    
    // 1999-12-30 23:59:59 -> 2000-01-01 00:00:00
    double to_years() const { return static_cast<double>(year) + static_cast<double>(month.get_val() - 1)/12 + static_cast<double>(day.get_val() - 1)/360; }
    double to_months() const { return static_cast<double>(year)*12 + static_cast<double>(month.get_val() - 1) + static_cast<double>(day.get_val() - 1)/30; }
    double to_days() const { return static_cast<double>(year)*360 + static_cast<double>(month.get_val() - 1)*30 + static_cast<double>(day.get_val() - 1); }
    
    void from_years(double years)
    {
      year = static_cast<int>(years);
      double months = (years - year)*12;
      month = static_cast<int>(months);
      double days = (months - month)*30;
      day = static_cast<int>(days);
      
      month = month.get_val() + 1; // Or month = month + 1 ?
      day = day.get_val() + 1; // Or day = day + 1 ?
      
      normalize();
    }
    
    void from_months(double months)
    {
      //from_years(months/12);
      
      int month_i = static_cast<int>(months);
      double years = static_cast<double>(month_i - 1)/12.;
      int year_i = static_cast<int>(years);
      int month_carry = year_i * 12;
      month_i -= month_carry;
      months -= month_carry;
      
      double days = (months - month_i)*30;
      int day_i = static_cast<int>(days);
      
      day = day_i + 1;
      month = month_i + 1;
      year = year_i;
      
      normalize();
    }
    
    void from_days(double days)
    {
      //from_years(days/360);
      
      int day_i = static_cast<int>(days);
      
      double months = static_cast<double>(day_i - 1)/30.;
      int month_i = static_cast<int>(months);
      int day_carry = month_i * 30;
      day_i -= day_carry;
      
      double years = static_cast<double>(month_i - 1)/12.;
      int year_i = static_cast<int>(years);
      int month_carry = year_i * 12;
      month_i -= month_carry;
      
      day = day_i + 1;
      month = month_i + 1;
      year = year_i;
      
      normalize();
    }
    
    Date operator+(const Date& date) const
    {
      Date new_date = *this;
      new_date += date;
      return new_date;
    }
    
    Date operator-(const Date& date) const
    {
      Date new_date = *this;
      new_date -= date;
      return new_date;
    }
    
    Date operator+() const
    {
      return *this;
    }
    
    Date operator-() const
    {
      return { -year, -month, -day };
    }
    
    void operator+=(const Date& date)
    {
      day = day.get_val() + date.day;
      month = month.get_val() + date.month.get_val() + day.get_carry(); // Avoids wrapping with carry.
      year += date.year + month.get_carry();
      normalize();
    }
    
    void operator-=(const Date& date)
    {
      *this += (-date);
    }
    
    bool operator==(const Date& date) const
    {
      return year == date.year && month == date.month && day == date.day;
    }
    
    Date add_years_ret(double years) const
    {
      Date apa = *this;
      Date bpa;
      bpa.from_years(years);
      bpa.tare();
      return apa + bpa;
    }
    
    void add_years(double years)
    {
      Date other;
      other.from_years(years);
      other.day = other.day.get_val() - 1;
      other.month = other.month.get_val() - 1;
      *this += other;
    }
    
    Date add_months_ret(double months) const
    {
      Date apa = *this;
      Date bpa;
      bpa.from_months(months);
      bpa.tare();
      return apa + bpa;
    }
    
    void add_months(double months)
    {
      Date other;
      other.from_months(months);
      other.day = other.day.get_val() - 1;
      other.month = other.month.get_val() - 1;
      *this += other;
    }
    
    Date add_days_ret(double days) const
    {
      Date apa = *this;
      Date bpa;
      bpa.from_days(days);
      bpa.tare();
      return apa + bpa;
    }
    
    void add_days(double days)
    {
      Date other;
      other.from_days(days);
      other.day = other.day.get_val() - 1;
      other.month = other.month.get_val() - 1;
      *this += other;
    }
    
    bool in_range(const Date& date_before, const Date& date_after, Range type) const
    {
      auto years_here = to_years();
      auto years_before = date_before.to_years();
      auto years_after = date_after.to_years();
      if (years_before <= years_after) // 0 .. 23
        return math::in_range<double>(years_here, years_before, years_after, type);
      return false;
    }
  };

  struct Time
  {
    hour_t Hour = 0; // 24-hour format.
    sexagesimal_t minute = 0;
    sexagesimal_t second = 0;
    
    Time() = default;
    Time(int H, int m, int s) : Hour(H), minute(m), second(s) {}
    
    void normalize()
    {
      second.normalize();
      minute = minute.get_val() + second.get_carry();
      minute.normalize();
      Hour += minute.get_carry();
    }
    
    double to_hours() const { return static_cast<double>(Hour) + static_cast<double>(minute)/60 + static_cast<double>(second)/3'600; }
    double to_minutes() const { return static_cast<double>(Hour)*60 + static_cast<double>(minute) + static_cast<double>(second)/60; }
    double to_seconds() const { return static_cast<double>(Hour)*3'600 + static_cast<double>(minute)*60 + static_cast<double>(second); }
    
    void from_hours(double hours, bool norm = false)
    {
      Hour = static_cast<int>(hours);
      double minutes = (hours - Hour)*60;
      minute = static_cast<int>(minutes);
      double seconds = (minutes - minute)*60;
      second = static_cast<int>(seconds + second_eps);
      
      if (norm)
        normalize();
    }
    
    void from_minutes(double minutes, bool norm = false)
    {
      from_hours(minutes/60, norm);
    }
    
    void from_seconds(double seconds, bool norm = false)
    {
      from_hours(seconds/3'600, norm);
    }
    
    Time operator+(const Time& time) const
    {
      Time new_time = *this;
      new_time += time;
      return new_time;
    }
    
    Time operator-(const Time& time) const
    {
      Time new_time = *this;
      new_time -= time;
      return new_time;
    }
    
    Time operator+() const
    {
      return *this;
    }
    
    Time operator-() const
    {
      return { -Hour, -minute, -second };
    }
    
    void operator+=(const Time& time)
    {
      second += time.second;
      minute += time.minute.get_val() + second.get_carry(); // Avoids wrapping with carry.
      Hour += time.Hour.get_val() + minute.get_carry(); // Avoids wrapping with carry.
    }
    
    void operator-=(const Time& time)
    {
      *this += (-time);
    }
    
    bool operator==(const Time& time) const
    {
      return Hour == time.Hour && minute == time.minute && second == time.second;
    }
    
    Time add_hours_ret(double hours) const
    {
      Time apa = *this;
      Time bpa;
      bpa.from_hours(hours);
      return apa + bpa;
    }
    
    void add_hours(double hours)
    {
      Time other;
      other.from_hours(hours);
      *this += other;
    }
    
    Time add_minutes_ret(double minutes) const
    {
      Time apa = *this;
      Time bpa;
      bpa.from_minutes(minutes);
      return apa + bpa;
    }
    
    void add_minutes(double minutes)
    {
      Time other;
      other.from_minutes(minutes);
      *this += other;
    }
    
    Time add_seconds_ret(double seconds) const
    {
      Time apa = *this;
      Time bpa;
      bpa.from_seconds(seconds);
      return apa + bpa;
    }
    
    void add_seconds(double seconds)
    {
      Time other;
      other.from_seconds(seconds);
      *this += other;
    }
    
    int get_carry() const { return Hour.get_carry(); }
    
    bool in_range(const Time& time_before, const Time& time_after, Range type) const
    {
      auto hours_here = to_hours();
      auto hours_before = time_before.to_hours();
      auto hours_after = time_after.to_hours();
      if (hours_before <= hours_after) // 0 .. 23
        return math::in_range<float>(hours_here, hours_before, hours_after, type);
      else if (hours_here > hours_after)
        return math::in_range<float>(hours_here, hours_before, hours_after + 24, type);
      else if (hours_here < hours_before)
        return math::in_range<float>(hours_here, hours_before - 24, hours_after, type);
      return false;
    }
  };

// ///////////////////////////////////////////////////////

  struct DateTime
  {
    Date date;
    Time time;
    
    DateTime() = default;
    DateTime(int y, int M, int d, int H, int m, int s)
      : date(y, M, d)
      , time(H, m, s)
    {}
    DateTime(const Date& d, const Time& t)
      : date(d), time(t)
    {}
    
    void normalize()
    {
      time.normalize();
      date.day = date.day.get_val() + time.get_carry();
      date.normalize();
    }
    
    double to_years() const { return date.to_years() + time.to_hours()/8'640; }
    double to_months() const { return date.to_months() + time.to_hours()/720; }
    double to_days() const { return date.to_days() + time.to_hours()/24; }
    double to_hours() const { return date.to_days()*24 + time.to_hours(); }
    double to_minutes() const { return date.to_days()*1'440 + time.to_minutes(); }
    double to_seconds() const { return date.to_days()*86'400 + time.to_seconds(); }
    
    void from_years(double years)
    {
      date.year = static_cast<int>(years);
      double months = (years - date.year)*12;
      date.month = static_cast<int>(months);
      double days = (months - date.month)*30;
      date.day = static_cast<int>(days);
      double hours = (days - date.day)*24;
      time.Hour = static_cast<int>(hours);
      double minutes = (hours - time.Hour)*60;
      time.minute = static_cast<int>(minutes);
      double seconds = (minutes - time.minute)*60;
      time.second = static_cast<int>(seconds + second_eps);
      
      if (date.day >= 30)
      {
        date.day = 0;
        date.month = date.month.get_val() + 1;
      }
      if (date.month >= 12)
      {
        date.month = 0;
        date.year++;
      }
      
      date.month = date.month.get_val() + 1; // Or month = month + 1 ?
      date.day = date.day.get_val() + 1; // Or day = day + 1 ?
      
      normalize();
    }
    
    void from_months(double months)
    {
      from_years(months/12);
      
      /*
      int month_i = static_cast<int>(months);
      double years = static_cast<double>(month_i - 1)/12.;
      int year_i = static_cast<int>(years);
      int month_carry = year_i * 12;
      month_i -= month_carry;
      months -= month_carry;
      
      double days = (months - month_i)*30;
      int day_i = static_cast<int>(days);
      
      double hours = (days - day_i)*24;
      time.Hour = static_cast<int>(hours);
      double minutes = (hours - time.Hour)*60;
      time.minute = static_cast<int>(minutes);
      double seconds = (minutes - time.minute)*60;
      time.second = static_cast<int>(seconds + second_eps);
      
      date.day = day_i + 1;
      date.month = month_i + 1;
      date.year = year_i;
      
      normalize();
       */
    }
    
    void from_days(double days)
    {
      from_years(days/360);
      
      /*
      int day_i = static_cast<int>(days);
      
      double months = static_cast<double>(day_i - 1)/30.;
      int month_i = static_cast<int>(months);
      int day_carry = month_i * 30;
      day_i -= day_carry;
      days -= day_carry;
      
      double years = static_cast<double>(month_i - 1)/12.;
      int year_i = static_cast<int>(years);
      int month_carry = year_i * 12;
      month_i -= month_carry;
      
      double hours = (days - day_i)*24;
      time.Hour = static_cast<int>(hours);
      double minutes = (hours - time.Hour)*60;
      time.minute = static_cast<int>(minutes);
      double seconds = (minutes - time.minute)*60;
      time.second = static_cast<int>(seconds + second_eps);
      
      date.day = day_i + 1;
      date.month = month_i + 1;
      date.year = year_i;
      
      normalize();
       */
    }
    
    void from_hours(double hours)
    {
      int hour_i = static_cast<int>(hours);
      
      double days = static_cast<double>(hour_i)/24.f;
      int day_i = static_cast<int>(days);
      int hour_carry = day_i * 24;
      hour_i -= hour_carry;
      hours -= hour_carry;
      
      double months = static_cast<double>(day_i - 1)/30.f;
      int month_i = static_cast<int>(months);
      int day_carry = month_i * 30;
      day_i -= day_carry;
      
      double years = static_cast<double>(month_i - 1)/12.f;
      int year_i = static_cast<int>(years);
      int month_carry = year_i * 12;
      month_i -= month_carry;
      
      time.Hour = hour_i;
      double minutes = (hours - time.Hour)*60;
      time.minute = static_cast<int>(minutes);
      double seconds = (minutes - time.minute)*60;
      time.second = static_cast<int>(seconds + second_eps);
      
      date.day = day_i;
      date.month = month_i;
      date.year = year_i;
      
      if (date.day >= 30)
      {
        date.day = 0;
        date.month = date.month.get_val() + 1;
      }
      if (date.month >= 12)
      {
        date.month = 0;
        date.year++;
      }
      
      date.day = date.day.get_val() + 1;
      date.month = date.month.get_val() + 1;
      
      normalize();
    }
    
    void from_minutes(double minutes)
    {
      from_hours(minutes/60);
    }
    
    void from_seconds(double seconds)
    {
      from_hours(seconds/3'600);
    }
    
    DateTime operator+(const DateTime& datetime) const
    {
      DateTime new_datetime = *this;
      new_datetime += datetime;
      return new_datetime;
    }
    
    DateTime operator-(const DateTime& datetime) const
    {
      DateTime new_datetime = *this;
      new_datetime -= datetime;
      return new_datetime;
    }
    
    DateTime operator+() const
    {
      return *this;
    }
    
    DateTime operator-() const
    {
      return { -date, -time };
    }
    
    void operator+=(const DateTime& datetime)
    {
      time += datetime.time;
      date.day = date.day.get_val() + time.get_carry();
      date += datetime.date;
    }
    
    void operator-=(const DateTime& datetime)
    {
      *this += (-datetime);
    }
    
    bool operator==(const DateTime& datetime) const
    {
      return date == datetime.date && time == datetime.time;
    }
    
    DateTime add_years_ret(double years) const
    {
      DateTime apa = *this;
      DateTime bpa;
      bpa.from_years(years);
      bpa.date.tare();
      return apa + bpa;
    }
    
    void add_years(double years)
    {
      DateTime other;
      other.from_years(years);
      other.date.day = other.date.day.get_val() - 1;
      other.date.month = other.date.month.get_val() - 1;
      *this += other;
    }
    
    DateTime add_months_ret(double months) const
    {
      DateTime apa = *this;
      DateTime bpa;
      bpa.from_months(months);
      bpa.date.tare();
      return apa + bpa;
    }
    
    void add_months(double months)
    {
      DateTime other;
      other.from_months(months);
      other.date.day = other.date.day.get_val() - 1;
      other.date.month = other.date.month.get_val() - 1;
      *this += other;
    }
    
    DateTime add_days_ret(double days) const
    {
      DateTime apa = *this;
      DateTime bpa;
      bpa.from_days(days);
      bpa.date.tare();
      return apa + bpa;
    }
    
    void add_days(double days)
    {
      DateTime other;
      other.from_days(days);
      other.date.day = other.date.day.get_val() - 1;
      other.date.month = other.date.month.get_val() - 1;
      *this += other;
    }
    
    DateTime add_hours_ret(double hours) const
    {
      DateTime apa = *this;
      DateTime bpa;
      bpa.from_hours(hours);
      bpa.date.tare();
      return apa + bpa;
    }
    
    void add_hours(double hours)
    {
      DateTime other;
      other.from_hours(hours);
      other.date.day = other.date.day.get_val() - 1;
      other.date.month = other.date.month.get_val() - 1;
      *this += other;
    }
    
    DateTime add_minutes_ret(double minutes) const
    {
      DateTime apa = *this;
      DateTime bpa;
      bpa.from_minutes(minutes);
      bpa.date.tare();
      return apa + bpa;
    }
    
    void add_minutes(double minutes)
    {
      DateTime other;
      other.from_minutes(minutes);
      other.date.day = other.date.day.get_val() - 1;
      other.date.month = other.date.month.get_val() - 1;
      *this += other;
    }
    
    DateTime add_seconds_ret(double seconds) const
    {
      /*
       % Alternative Octave code.
       carry = seconds;
       unit_wraps = [0 12 30 24 60 60];
       for i=6:-1:1
         carry += datetime_in(i);
         datetime_out(i) = mod(carry, unit_wraps(i));
         carry -= datetime_out(i);
         carry /= unit_wraps(i);
       endfor
       */
      DateTime apa = *this;
      DateTime bpa;
      bpa.from_seconds(seconds);
      bpa.date.tare();
      return apa + bpa;
    }
    
    void add_seconds(double seconds)
    {
      DateTime other;
      other.from_seconds(seconds);
      other.date.day = other.date.day.get_val() - 1;
      other.date.month = other.date.month.get_val() - 1;
      *this += other;
    }
    
    bool in_range(const DateTime& datetime_before, const DateTime& datetime_after, Range type) const
    {
      auto days_here = to_days();
      auto days_before = datetime_before.to_days();
      auto days_after = datetime_after.to_days();
      if (days_before <= days_after) // 0 .. 23
        return math::in_range<double>(days_here, days_before, days_after, type);
      return false;
    }
  };

// ///////////////////////////////////////////////////////

  void update_date_time(DateTime& date_time, const DateTime& date_time_0, double time_s)
  {
    date_time = date_time_0;
    date_time.add_seconds(time_s);
  }

  Time rand_time(const Time& start, const Time& end)
  {
    auto start_s = start.to_seconds();
    auto end_s = end.to_seconds();
    if (start_s > end_s)
      end_s += 86'400; // Add one day.
    auto rand_s = rnd::rand_float(start_s, end_s);
    Time rand_time;
    rand_time.from_seconds(rand_s);
    rand_time.normalize();
    return rand_time;
  }

  Time randn_time(const Time& mu, float sigma_seconds, const Time& start, const Time& end)
  {
    auto start_s = start.to_seconds();
    auto end_s = end.to_seconds();
    auto mu_s = mu.to_seconds();
    if (start_s > end_s)
      end_s += 86'400; // Add one day.
    if (start_s > mu_s)
      mu_s += 86'400; // Add one day.
    auto randn_s = rnd::randn_clamp(mu_s, sigma_seconds, start_s, end_s);
    Time randn_time;
    randn_time.from_seconds(randn_s);
    randn_time.normalize();
    return randn_time;
  }

  Date rand_date(const Date& start, const Date& end)
  {
    auto start_d = start.to_days();
    auto end_d = end.to_days();
    auto rand_d = rnd::rand_float(start_d, end_d);
    Date rand_date;
    rand_date.from_days(rand_d);
    return rand_date;
  }

  Date randn_date(const Date& mu, float sigma_days, const Date& start, const Date& end)
  {
    auto start_d = start.to_days();
    auto end_d = end.to_days();
    auto mu_d = mu.to_days();
    auto randn_d = rnd::randn_clamp(mu_d, sigma_days, start_d, end_d);
    Date randn_date;
    randn_date.from_days(randn_d);
    return randn_date;
  }

  DateTime rand_datetime(const DateTime& start, const DateTime& end)
  {
    auto start_d = start.to_days();
    auto end_d = end.to_days();
    auto rand_d = rnd::rand_float(start_d, end_d);
    DateTime rand_datetime;
    rand_datetime.from_days(rand_d);
    return rand_datetime;
  }

  DateTime randn_datetime(const Date& mu, float sigma_days, const DateTime& start, const DateTime& end)
  {
    auto start_d = start.to_days();
    auto end_d = end.to_days();
    auto mu_d = mu.to_days();
    auto randn_d = rnd::randn_clamp(mu_d, sigma_days, start_d, end_d);
    DateTime randn_datetime;
    randn_datetime.from_days(randn_d);
    return randn_datetime;
  }

  std::string get_datetime_str(const DateTime& date_time, const std::string& format = "%Y-%m-%d %H:%M:%S")
  {
    enum class Token { y, Y, m, d, e, f, b, B, H, M, S, I, p, filling };
    std::map<char, Token> char_tokens {
      { 'y', Token::y },
      { 'Y', Token::Y },
      { 'm', Token::m },
      { 'd', Token::d },
      { 'e', Token::e },
      { 'f', Token::f },
      { 'b', Token::b },
      { 'B', Token::B },
      { 'H', Token::H },
      { 'M', Token::M },
      { 'S', Token::S },
      { 'I', Token::I },
      { 'p', Token::p },
    };
    std::vector<Token> tokens;
    std::vector<char> filling_characters;
    // Tokenizer
    auto fmt = format;
    do
    {
      if (fmt.size() > 0)
      {
        if (fmt[0] == '%')
        {
          if (fmt.size() > 1)
          {
            char ch = fmt[1];
            auto it = char_tokens.find(ch);
            if (it != char_tokens.end())
            {
              tokens.emplace_back(it->second);
              fmt.erase(0, 2);
            }
            else
              std::cerr << "Unidentified identifier \"" + std::to_string(ch) + "\".\n";
          }
          else
            std::cerr << "Date-time token marker without acceptable token identifier.\n";
        }
        else
        {
          filling_characters.emplace_back(fmt[0]);
          tokens.emplace_back(Token::filling);
          fmt.erase(0, 1);
        }
      }
    } while (!fmt.empty());
      
    const auto& date = date_time.date;
    const auto& time = date_time.time;
    
    std::string date_time_str;
    
    size_t filling_idx = 0;
    auto adj = str::Adjustment::Right;
    for (auto tkn : tokens)
    {
      switch (tkn)
      {
        case Token::y:
        {
          int y = (date.year - 100*(date.year/100));
          date_time_str += str::adjust_str(std::to_string(y), adj, 2, 0, '0');
          break;
        }
        case Token::Y:
          date_time_str += std::to_string(date.year);
          break;
        case Token::m:
          date_time_str += str::adjust_str(std::to_string(date.month), adj, 2, 0, '0');
          break;
        case Token::d:
          date_time_str += str::adjust_str(std::to_string(date.day), adj, 2, 0, '0');
          break;
        case Token::e:
          date_time_str += std::to_string(date.day);
          break;
        case Token::f:
          date_time_str += std::to_string(date.month);
          break;
        case Token::b:
        {
          std::string short_month;
          switch (date.month)
          {
            case 1: short_month = "Jan"; break;
            case 2: short_month = "Feb"; break;
            case 3: short_month = "Mar"; break;
            case 4: short_month = "Apr"; break;
            case 5: short_month = "May"; break;
            case 6: short_month = "Jun"; break;
            case 7: short_month = "jul"; break;
            case 8: short_month = "Aug"; break;
            case 9: short_month = "Sep"; break;
            case 10: short_month = "Oct"; break;
            case 11: short_month = "Nov"; break;
            case 12: short_month = "Dec"; break;
          }
          date_time_str += short_month;
          break;
        }
        case Token::B:
        {
          std::string long_month;
          switch (date.month)
          {
            case 1: long_month = "January"; break;
            case 2: long_month = "February"; break;
            case 3: long_month = "March"; break;
            case 4: long_month = "April"; break;
            case 5: long_month = "May"; break;
            case 6: long_month = "June"; break;
            case 7: long_month = "July"; break;
            case 8: long_month = "August"; break;
            case 9: long_month = "September"; break;
            case 10: long_month = "October"; break;
            case 11: long_month = "November"; break;
            case 12: long_month = "December"; break;
          }
          date_time_str += long_month;
          break;
        }
        case Token::H:
          date_time_str += str::adjust_str(std::to_string(time.Hour), adj, 2, 0, '0');
          break;
        case Token::M:
          date_time_str += str::adjust_str(std::to_string(time.minute), adj, 2, 0, '0');
          break;
        case Token::S:
          date_time_str += str::adjust_str(std::to_string(time.second), adj, 2, 0, '0');
          break;
        case Token::I:
        {
          int h = time.Hour;
          if (h > 12)
            h -= 12;
          date_time_str += str::adjust_str(std::to_string(h), adj, 2, 0, '0');
          break;
        }
        case Token::p:
        {
          std::string am_pm = "am";
          if (time.Hour > 12)
            am_pm = "pm";
          date_time_str += am_pm;
          break;
        }
        case Token::filling:
          date_time_str += filling_characters[filling_idx++];
          break;
      }
    }
    
    //auto date_time_str = std::to_string(date.year) +
    //"-" + str::adjust_str(std::to_string(date.month), str::Adjustment::Right, 2, 0, '0') +
    //"-" + str::adjust_str(std::to_string(date.day), str::Adjustment::Right, 2, 0, '0') +
    //" " + str::adjust_str(std::to_string(time.Hour), str::Adjustment::Right, 2, 0, '0') +
    //":" + str::adjust_str(std::to_string(time.minute), str::Adjustment::Right, 2, 0, '0') +
    //":" + str::adjust_str(std::to_string(time.second), str::Adjustment::Right, 2, 0, '0');
    return date_time_str;
  }
}
