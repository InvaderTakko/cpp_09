/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sruff <sruff@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 13:58:15 by sruff             #+#    #+#             */
/*   Updated: 2026/07/05 16:34:48 by sruff            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RPN_HPP
# define RPN_HPP

# include <string>
# include <stack>
# include <list>

class RPN
{
  public:
	RPN();
	RPN(const RPN &other);
	RPN &operator=(const RPN &other);
	~RPN();

	int evaluate(const std::string &expression);

  private:
	std::stack<int, std::list<int> > stack;
	bool isNumber(const std::string &token);
	bool isOp(const char c) const;
	int doMath(int left, int right, const char operation) const;
};

#endif
