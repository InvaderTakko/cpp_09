/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sruff <sruff@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 13:58:06 by sruff             #+#    #+#             */
/*   Updated: 2026/07/05 16:33:40 by sruff            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RPN.hpp"
#include <exception>
#include <iostream>

int	main(int argc, char **argv)
{
	RPN	calculator;

	if (argc != 2)
	{
		std::cerr << "invalid argument number" << std::endl;
		return (1);
	}
	try 
	{
		std::cout << calculator.evaluate(argv[1]) << std::endl;
	}
	
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << std::endl;
		std::cerr << e.what() << std::endl; //not sure if good error msg allowed :)
		
		return (1);
	}


	return (0);
}
