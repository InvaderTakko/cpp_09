/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sruff <sruff@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 13:30:28 by sruff             #+#    #+#             */
/*   Updated: 2026/06/16 16:03:26 by sruff            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BitcoinExchange.hpp"
#include <array>
#include <charconv>
#include <cmath>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

static std::optional<int> parseInt(std::string_view text)
{
	int			value = 0;
	const auto	result = std::from_chars(text.data(), text.data() + text.size(), value);

	if (result.ec != std::errc() || result.ptr != text.data() + text.size())
		return (std::nullopt); // failure = no value
	return (value);
}

static std::optional<double> parsePrice(std::string_view text)
{
	double		value = 0.0;
	const auto	result = std::from_chars(text.data(), text.data() + text.size(), value);

	if (result.ec != std::errc() || result.ptr != text.data() + text.size()) // struct from_chars_result {
		return (std::nullopt);												//	const char* ptr;  // where parsing stopped
																			//	std::errc     ec;  // success or error kind
																			//	};
	if (!std::isfinite(value))
		return (std::nullopt);
	return (value);
}

static std::string_view trim(std::string_view text) //remove spaces etc // string_view = no ownership no copy
{
	while (!text.empty() && (text.front() == ' ' || text.front() == '\t'))
		text.remove_prefix(1);
	while (!text.empty() && (text.back() == ' ' || text.back() == '\t' || text.back() == '\r'))
		text.remove_suffix(1);
	return (text);
}

static constexpr bool isLeapYear(int year)// compile time
{
	if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
		return (true);
	else return (false);
}

static constexpr int daysInMonth(int month, int year) //compile time
{
	constexpr std::array<int, 13>	days = 
	{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; //12 months but start with 0 for natural index

	if (month == 2 && isLeapYear(year))
		return (29);
	
	return (days[month]);
}

static bool parseDate(std::string_view text)
{
	if (text.size() != 10 || text[4] != '-' || text[7] != '-')
		return (false);

	const std::optional<int>	year = parseInt(text.substr(0, 4));
	const std::optional<int>	month = parseInt(text.substr(5, 2));
	const std::optional<int>	day = parseInt(text.substr(8, 2));

	if (!year || !month || !day)
		return (false);
	if (*month < 1 || *month > 12)
		return (false);
	if (*day < 1 || *day > daysInMonth(*month, *year))
		return (false);
	return (true);
}

BitcoinExchange::BitcoinExchange()
{
}

BitcoinExchange::BitcoinExchange(const BitcoinExchange &other)
	: _data(other._data)
{
}

BitcoinExchange &BitcoinExchange::operator=(const BitcoinExchange &other)
{
	if (this != &other)
		_data = other._data;
	return (*this);
}

BitcoinExchange::~BitcoinExchange()
{
}

BitcoinExchange::Database BitcoinExchange::loadDatabase(const std::string &path)
{
	Database	db;
	std::string	line;

	std::ifstream file(path.c_str());
	if (!file.is_open())
		return (db);

	std::getline(file, line);
	while (std::getline(file, line))
	{
		const std::size_t comma = line.find(','); //returns index of the array not pointer

		if (comma == std::string::npos)
			continue; //skip line without comma

		const std::string				date = line.substr(0, comma);
		const std::string				priceStr = line.substr(comma + 1);
		const std::optional<double>		price = parsePrice(priceStr);

		if (!parseDate(date) || !price)
			continue;  //  not YYYY-MM-DD lenght || whole string not valid number

		db[date] = *price;
	}

	_data = db; // we love object oriented code i guess
	return (db);
}

bool BitcoinExchange::processInput(const std::string &path)
{
	std::ifstream	file(path.c_str());
	std::string		line;

	if (!file.is_open())
	{
		std::cout << "Error: could not open file." << std::endl;
		return (false);
	}

	while (std::getline(file, line))
	{
		if (line == "date | value")
			continue;

		const std::size_t	pipe = line.find('|');
		if (pipe == std::string::npos)
		{
			std::cout << "Error: bad input => " << line << std::endl;
			continue;
		}
		//find and trim out date price etc
		const std::string_view		date = trim(std::string_view(line.data(), pipe)); 
		const std::string_view		valueStr = trim(std::string_view(line.data() + pipe + 1, line.size() - pipe - 1));
		const std::optional<double>	value = parsePrice(valueStr);

		if (!parseDate(date))
		{
			std::cout << "Error: bad input => " << date << std::endl;
			continue;
		}
		if (!value)
		{
			std::cout << "Error: bad input => " << line << std::endl;
			continue;
		}
		if (*value < 0.0)
		{
			std::cout << "Error: not a positive number." << std::endl;
			continue;
		}
		if (*value > 1000.0)
		{
			std::cout << "Error: too large a number." << std::endl;
			continue;
		}

		Database::const_iterator	rate = _data.lower_bound(std::string(date));
		if (rate == _data.end() || rate->first != date)
		{
			if (rate == _data.begin())
			{
				std::cout << "Error: bad input => " << date << std::endl;
				continue;
			}
			--rate;
		}

		std::cout << date << " => " << *value << " = " << *value * rate->second << std::endl;
	}
	return (true);
}
