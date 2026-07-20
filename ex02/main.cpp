/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sruff <sruff@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 13:58:21 by sruff             #+#    #+#             */
/*   Updated: 2026/06/20 15:28:30 by sruff            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PmergeMe.hpp"
#include <exception>
#include <iostream>

int	main(int argc, char **argv)
{
	try
	{
		PmergeMe	sorter;

		sorter.run(argc, argv);
	}
	catch (const std::exception &)
	{
		std::cerr << "Error" << std::endl;
		return (1);
	}
	return (0);
}
