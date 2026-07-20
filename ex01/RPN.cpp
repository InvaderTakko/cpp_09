/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sruff <sruff@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 13:58:12 by sruff             #+#    #+#             */
/*   Updated: 2026/07/05 16:35:14 by sruff            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RPN.hpp"
#include <cctype>
#include <limits>
#include <ostream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <system_error>

RPN::RPN()
{
}

RPN::RPN(const RPN &other) : stack(other.stack)
{
	(void)other;
}

RPN &RPN::operator=(const RPN &other)
{
	if (this != &other)
		stack  = other.stack;
	return (*this);
}

RPN::~RPN()
{
}

int RPN::doMath(int left, int right, const char operation) const
{
	long long res;
	switch (operation)
	{
		case '+' :
			res = left + right;
			break;
		case '-' :
			res = left - right;
			break;
		case '*' :
			res = left * right;
			break;
		case '/' :
		{
			if (right != 0)
			{
				res = left / right;
				break;
			}	
			else
				throw std::runtime_error("trying to devide by yero");
			break;
		}
		default :
			throw std::runtime_error("none valid operator");;
			
	}
	if (res < std::numeric_limits<int>::min() || res > std::numeric_limits<int>::max())
		throw std::overflow_error("integer overflow");
	return (res);
}
bool RPN::isOp(const char c) const
{
	if (c=='+' || c == '-' || c == '*' || c == '/')
		return true;
	else
	 	return false;
}

bool RPN::isNumber(const std::string &token)
{
	if (token.length() == 1 && std::isdigit(static_cast<unsigned char>(token[0])))
		return true;
	else
		return false;
}

int RPN::evaluate(const std::string &expression)
{

	std::istringstream ss(expression); //inputstring
	std::string token;
	//std::string prev_token = {};
	while (ss >> token) // takes care of whitespace
	{
		if (isNumber(token))
		{
			stack.push((token[0] - '0'));
			continue;
		}	// throw error? intended behaviour?
		if (token.length() != 1 || !isOp(token[0]))
			throw std::runtime_error("not a valid token");
		if (stack.size() < 2)
			throw std::runtime_error("wrong amount of Operators");
		int right = stack.top();
		stack.pop();
		int left = stack.top();
		stack.pop();
		//std::cout << "stack left: " << left << " Operator: "<< token[0]<< " stack right: "<< right <<std::endl;
		stack.push(doMath(left, right, token[0]));
		
	}
	if (stack.size() != 1)
		throw std::runtime_error("not valid RPN format");
	int result = stack.top();
	stack.pop();
	return (result);
}
