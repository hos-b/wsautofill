#include <iostream>
#include <cstdlib>

#include <vector>
#include <unordered_map>


#include <string>
#include <sstream>
#include <iomanip>

#ifndef __WS_STRUCTS_H__
#define __WS_STRUCTS_H__


enum WeekDayEnum: unsigned int {
	MON = 0,
	TUE = 1,
	WED = 2,
	THU = 3,
	FRI = 4,
	SAT = 5,
	SUN = 6
};
// anonymous namespace
namespace
{
	static const unsigned int months_norm [] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	static const unsigned int months_leap [] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	static const std::unordered_map<std::string, WeekDayEnum> weekday_str({{"monday", WeekDayEnum::MON}, {"tuesday", WeekDayEnum::TUE},
																    	   {"wednesday", WeekDayEnum::WED}, {"thursday", WeekDayEnum::THU},
																    	   {"friday", WeekDayEnum::FRI}, {"saturday", WeekDayEnum::SAT},
																    	   {"sunday", WeekDayEnum::SUN}});
	static const std::unordered_map<WeekDayEnum, std::string> weekday_enum({{WeekDayEnum::MON, "monday"}, {WeekDayEnum::TUE, "tuesday"},
																    		{WeekDayEnum::WED, "wednesday"}, {WeekDayEnum::THU, "thursday"},
																    		{WeekDayEnum::FRI, "friday"}, {WeekDayEnum::SAT, "saturday"},
																    		{WeekDayEnum::SUN, "sunday"}});
}

class WeekDay 
{
public:
	WeekDay() : weekday_(weekday_str.at("monday")){}
	WeekDay(const std::string wd_str) : weekday_(weekday_str.at(wd_str)) {}
	std::string ToString() {
		return weekday_enum.at(weekday_);
	}
	bool IsWeekend() {
		return weekday_ == SAT || weekday_ == SUN;
	}
	WeekDay& operator++() {
		weekday_ = static_cast<WeekDayEnum>((static_cast<unsigned int>(weekday_) + 1) % 7);
		return *this;
	}
private:
	WeekDayEnum weekday_;
};

class Date 
{
public:
  Date() : day_(1), month_(1) {}
  Date(const std::string &date_str) {
	  if (date_str.length() != 5) {
		  std::cout << "ERROR: expected date format DD.MM" << std::endl;
		  std::exit(EXIT_FAILURE);
	  } else {
			try {
				day_ = std::stoi(date_str.substr(0, 2));
		  		month_ = std::stoi(date_str.substr(3, 2));
			} catch (std::invalid_argument const &e) {
				std::cout << "ERROR: expected date format DD.MM" << std::endl;
				std::exit(EXIT_FAILURE);
			}
	  }
	}
	Date(unsigned char day, unsigned char month) : day_(day), month_(month) {
		if (day_ == 0 || month_ == 0 || month_ > 12 || day_ > months_norm[month_]) {
			std::cout << "ERROR: invalid date DD.MM" << ToString() << std::endl;
			std::exit(EXIT_FAILURE);
		}
	}
	std::string ToString() {
		std::stringstream ss;
		ss << std::setw(2) << std::setfill('0') << day_ << ".";
		ss << std::setw(2) << std::setfill('0') << month_;
		return ss.str();
	}
	Date& operator++() {
		if (++day_ > months_norm[month_]) {
			month_ = (month_ + 1) % 13;
			if (month_ == 0)
				month_ = 1;
			day_ = 1;
		}
		return *this;
	}
	bool operator==(const Date& other) {
		return day_ == other.day_ && month_ == other.month_;
	}
	bool operator!=(const Date& other) {
		return day_ != other.day_ || month_ != other.month_;
	}
	bool operator<(const Date& other) {
		if (month_ < other.month_ || (month_ == other.month_ && day_ < other.day_))
			return true;
		return false;
	}
	bool operator<=(const Date& other) {
		if (month_ < other.month_ || (month_ == other.month_ && day_ <= other.day_))
			return true;
		return false;
	}
// private:
	unsigned int day_;
	unsigned int month_;
};

/* getting rid of ccls warnings */
void Placeholder() {
	(void)months_norm;
	(void)months_leap;
}
#endif