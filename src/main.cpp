#include <xlnt/xlnt.hpp>

#include "structs.h"


// what to skip
bool skip_weekends;
std::vector<Date> skip_dates;
// not implemented yet
bool leap_year;
// used for skipping weekends
WeekDay starting_weekday;
Date start_date, end_date;
// amount of hours to fill
float hours;
// time of day related variables
float day_start, day_end, lunch_break_start, lunch_break_end;
/* initializes the global variables. to be replaced by a yaml read  */
void Init() {
	skip_weekends = true;
	leap_year = false;
	start_date = Date("16.03");
	end_date = Date("15.04");
	hours = 60.0f;
	starting_weekday = stat::weekday.at("monday");

	day_start = 9.0f;
	day_end = 20.0f;
	lunch_break_start = 13.0f;
	lunch_break_end = 14.5f;
}
unsigned int GetFreeDays() {
	unsigned int free_days = 0;
	size_t skip_index = 0;
	auto weekday = starting_weekday;
	Date current_date(start_date);
	while (current_date != end_date) {
		// if not skipping weekends
		if (!skip_weekends && (weekday == WeekDay::SAT || weekday == WeekDay::SUN))
			free_days++;
		else {
			// if skipping this date
			if (skip_index < skip_dates.size() && current_date == skip_dates[skip_index])
				skip_index++;
			else
				free_days++;
		}
		weekday = static_cast<WeekDay>((static_cast<unsigned char>(weekday) + 1) % 6);
		++current_date;
	}
	return free_days;
}

int main (int argc, char** argv)
{
	srand(1984);
	float filled_hours = 0.0f;

	while (filled_hours != hours) {

	}
}