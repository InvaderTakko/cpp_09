/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sruff <sruff@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 13:30:28 by sruff             #+#    #+#             */
/*   Updated: 2026/06/07 19:16:48 by sruff            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BitcoinExchange.hpp"
#include <fstream>
#include <sstream>
#include <string>


bool parsePrice(const std::string &text, double &out)
{
	std::istringstream	ss(text);
	char				extra;

	if (!(ss >> out))
		return (false);
	if (ss >> extra)
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
		const std::size_t comma = line.find(',');

		if (comma == std::string::npos)
			continue; //skip line without comma

		const std::string	date = line.substr(0, comma);
		const std::string	priceStr = line.substr(comma + 1);
		double				price;

		if (date.size() != 10 || !parsePrice(priceStr, price))
			continue; //  not YYYY-MM-DD lenght || whole string not valid number 

		db[date] = price;
	}

	_data = db;
	return (db);
}

bool BitcoinExchange::processInput(const std::string &path)
{
	(void)path;
	// TODO: parse input file (format: date | value) and print results
	return (false);
}
