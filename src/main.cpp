#include <xlnt/xlnt.hpp>

#include <yaml-cpp/yaml.h>
#include "structs.h"
#include <ctime>

// what to skip
bool skip_weekends;
std::vector<Date> skip_dates;
// not implemented yet
bool leap_year;
// used for skipping weekends
WeekDay starting_weekday;
Date start_date, end_date;
std::string year;
// amount of hours to fill
float hours;
float avg_hour_per_day;
float hour_std;
// time of day related variables
float day_start, day_end, lunch_break_start, lunch_break_end;

// excel sheet stuff
std::string input_filename;
std::string output_filename;
unsigned int date_column;
unsigned int start_column;
unsigned int end_column;
unsigned int first_row;
/* initializes the global variables. to be replaced by a yaml read  */
void Init() {
	YAML::Node base = YAML::LoadFile("params.yaml");
	skip_weekends = base["skip-weekends"].as<bool>();
	leap_year = base["leap-year"].as<bool>();
	start_date = Date(base["start-date"].as<std::string>());
	end_date = Date(base["end-date"].as<std::string>());
	year = base["year"].as<std::string>();
	hours = base["hours"].as<float>();
	avg_hour_per_day = base["daily-average"].as<float>();
	hour_std = base["daily-std"].as<float>();
	starting_weekday = WeekDay(base["starting-weekday"].as<std::string>());

	day_start = base["start-of-day"].as<float>();
	day_end = base["end-of-day"].as<float>();
	lunch_break_start = base["lunch-break-start"].as<float>();
	lunch_break_end = base["lunch-break-end"].as<float>();

	input_filename = base["input-file"].as<std::string>();
	output_filename = base["output-file"].as<std::string>();
	date_column = base["date-column"].as<int>();
	start_column = base["start-time-column"].as<int>();
	end_column = base["end-time-column"].as<int>();
	first_row = base["first-row"].as<int>();
	// last_row = base["last-row"].as<int>();

	// filling out skip dates
	YAML::Node sd_node = base["skip-dates"];
	if (sd_node.IsSequence()) {
		for (unsigned int t = 0; t < sd_node.size(); ++t) {
			Date new_date(sd_node[t].as<std::string>());
			// assert dates are written in order
			if (skip_dates.size() > 0 && new_date <= skip_dates.back()) {
				std::cout << "ERROR: wrong date order in the yaml file" << std::endl;
				std::exit(EXIT_FAILURE);
			}
			skip_dates.emplace_back(new_date);
		}
	}
}
/* calculates the free days in the given range based on yaml parameters */
unsigned int GetFreeDays() {
	unsigned int free_days = 0;
	size_t skip_index = 0;
	auto weekday = starting_weekday;
	Date current_date(start_date);
	while (current_date != end_date) {
		// if not skipping weekends
		if (weekday.IsWeekend()) {
			if (!skip_weekends) {
				if (skip_index < skip_dates.size() && current_date == skip_dates[skip_index])
					skip_index++;
				else
					free_days++;
			}
		} else {
			if (skip_index < skip_dates.size() && current_date == skip_dates[skip_index])
				skip_index++;
			else
				free_days++;
		}
		++weekday;
		++current_date;
	}
	return free_days;
}
/* returns a random number in [avg_hour +/- std] range */
float GetRandomFloat(float avg_hour, float hstd) {
	auto randn = std::rand() / static_cast<double>(RAND_MAX);
	auto ret_val = avg_hour + hstd * (2 * randn - 1);
	// limiting decimal to .0 or .5
	int num_int = static_cast<int>(ret_val);
	int num_frac = static_cast<int>(ret_val * 10) % 10;
	if (num_frac >= 5)
		return static_cast<float>(num_int) + 0.5f;
	else if (num_frac <= -5)
		return static_cast<float>(num_int) - 0.5f;
	return static_cast<float>(num_int);
}
/* returns a probability based on the amount of days/hours left */
float GetFillProbability(unsigned int free_days, float remaining_hours) {
	float avg_hpd = remaining_hours / (free_days * 1.0f);
	return std::fmin(avg_hpd / avg_hour_per_day, 1.0f);
}
/* returns a random schedule based on the parameters */
std::vector<std::pair<std::string, float>> GetRandomSchedule() {
	std::vector<std::pair<std::string, float>> schedule;
	float remaining_hours = hours;
	// coin toss criterion
	unsigned int free_days = GetFreeDays();
	size_t skip_index = 0;
	auto weekday = starting_weekday;
	for (Date current_date = start_date; current_date < end_date; ++current_date, ++weekday) {
		// skipping current date if not available for scheduling
		if (weekday.IsWeekend() && skip_weekends)
				continue;
		if (skip_index < skip_dates.size() && current_date == skip_dates[skip_index]) {
			skip_index++;
			continue;
		}
		// (unfair) coin toss
		if (std::rand() / (double)RAND_MAX < GetFillProbability(free_days, remaining_hours)) {
			if (remaining_hours <= avg_hour_per_day) {
				// fill the day with whatever's left
				schedule.push_back(std::make_pair(current_date.ToString(), remaining_hours));
				remaining_hours = 0;
				break;
			} else {
				float hrs = GetRandomFloat(avg_hour_per_day, hour_std);
				if (hrs > remaining_hours)
					hrs = remaining_hours;
				schedule.push_back(std::make_pair(current_date.ToString(), hrs));
				remaining_hours -= hrs;
			}
		}
		free_days--;
		if (remaining_hours == 0)
			break;
	}
	if (remaining_hours != 0) {
		std::cout << "incomplete schedule, remaining: " << remaining_hours << " hours. ";
		std::cout << "running second pass... "<< std::endl;
		do {
			auto random_index = std::rand() % schedule.size();
			schedule[random_index].second += 0.5f;
			remaining_hours -= 0.5f;
		} while (remaining_hours != 0);
	}
	else
		std::cout << "successufl schedule with " << schedule.size() << " elements" << std::endl;

	return schedule;
}
std::string GetFormattedTime(float time) {
	unsigned int num_int = static_cast<unsigned int>(time);
	unsigned int num_frac = static_cast<unsigned int>(time * 10) % 10;

	std::string ret;
	if  (num_int >= 10)
		ret = std::to_string(num_int);
	else
		ret = "0" + std::to_string(num_int);
	if (num_frac == 5)
		ret += ":30";
	else
		ret += ":00";

	return ret;
}
int main (int argc, char** argv)
{
	(void)argc; (void)argv;
	Init();
	// std::srand(std::time(NULL));
	std::srand(std::time(NULL));
	std::vector<std::pair<std::string, float>> schedule = GetRandomSchedule();
	// printing schedule
	std::cout << "schedule:" << std::endl;
	for (auto& elem : schedule)
		std::cout << elem.first << "." + year << "\t" << elem.second << std::endl;

	xlnt::workbook wb;
    wb.title("schedule");
	// cannot edit some existing files due to known XLNT bug https://github.com/tfussell/xlnt/issues/436
    // wb.load("input.xlsx");
    auto sheet = wb.active_sheet();
    std::cout << "creating spreadsheet..." << std::endl;
	float hrs;
	std::string date;
	unsigned int row = 1;
	// float morning_limit = lunch_break_start - day_start;
	// float evening_limit = day_end - lunch_break_end;
	for (unsigned int i = 0; i < schedule.size(); ++i) {
		std::tie(date, hrs) = schedule[i];
		float morning_start = day_start + GetRandomFloat(0.8f, 0.5); // returns 0, 0.5 or 1
		// divide into two entries if it cuts into lunch break
		if (morning_start + hrs > lunch_break_start) {
			float morning_end = lunch_break_start + GetRandomFloat(0.0, 1.0f); // returns -0.5 0 or 0.5
			float time_left = hrs - (morning_end - morning_start);
			// if randomization fits the whole thing in the morning slot
			if (time_left < 0) {
				morning_end = morning_start + hrs;
				time_left = 0;
			}
			sheet.cell(date_column, row).value(date + "." + year);
			sheet.cell(start_column, row).value(GetFormattedTime(morning_start));
			sheet.cell(end_column, row++).value(GetFormattedTime(morning_end));
			// fill the evening slot
			if (time_left != 0) {
				float evening_start = lunch_break_end + GetRandomFloat(0.0f, 1.0f); // returns -0.5 0 or 0.5
				float evening_end = evening_start + time_left;
				sheet.cell(date_column, row).value(date + "." + year);
				sheet.cell(start_column, row).value(GetFormattedTime(evening_start));
				sheet.cell(end_column, row++).value(GetFormattedTime(evening_end));
			}
		} else {
			sheet.cell(date_column, row).value(date + "." + year);
			sheet.cell(start_column, row).value(GetFormattedTime(morning_start));
			sheet.cell(end_column, row++).value(GetFormattedTime(morning_start + hrs));
		}
	}
	wb.save(output_filename);
    std::cout << "results saved to " << output_filename << std::endl;
	// filling out the excel sheet
}