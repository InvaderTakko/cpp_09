/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sruff <sruff@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 13:58:32 by sruff             #+#    #+#             */
/*   Updated: 2026/06/22 18:32:34 by sruff            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PMERGEME_HPP
# define PMERGEME_HPP

# include <cstddef>
# include <vector>
# include <deque>
# include <string>

class PmergeMe {
public:
	PmergeMe();
	PmergeMe(const PmergeMe& other);
	PmergeMe& operator=(const PmergeMe& other);
	~PmergeMe();

	void printSequence(const std::string &label, const std::vector<int> &sequence);
	std::vector<int> parseArguments(int argc, char **argv);
	void run(int argc, char **argv);

private:
	struct BlockRef
	{
		std::size_t	offset;
		std::size_t	size;
	};

	struct PendingRef
	{
		BlockRef	block;
		std::size_t	upperBoundIndex;
		bool		hasUpperBound;
	};

	struct BlockArena
	{
		std::vector<int>			values;
		std::vector<std::size_t>	ids;

		void		reserveForInput(std::size_t inputSize);
		BlockRef	makeSingle(int value, std::size_t id);
		BlockRef	merge(const BlockRef &left, const BlockRef &right);
		void		appendToVector(const BlockRef &block, std::vector<int> &output) const;
		void		appendToDeque(const BlockRef &block, std::deque<int> &output) const;
	};

	std::vector<int> sortVector(const std::vector<int> &input);
	std::deque<int> sortDeque(const std::vector<int> &input);
	std::vector<BlockRef> sortVectorBlocks(const std::vector<BlockRef> &blocks,
		BlockArena &arena);
	std::deque<BlockRef> sortDequeBlocks(const std::deque<BlockRef> &blocks,
		BlockArena &arena);

	std::vector<std::size_t> buildJacobsthalInsertionOrder(std::size_t count);
	bool blockLess(const BlockArena &arena, const BlockRef &left,
		const BlockRef &right) const;
	BlockRef makeOrderedPair(const BlockRef &left, const BlockRef &right,
		BlockArena &arena);
	void insertPendingBlock(std::vector<BlockRef> &mainChain,
		std::vector<PendingRef> &pending, std::size_t pendingIndex,
		const BlockArena &arena);
	void insertPendingBlock(std::deque<BlockRef> &mainChain,
		std::deque<PendingRef> &pending, std::size_t pendingIndex,
		const BlockArena &arena);
	void adjustPendingBounds(std::vector<PendingRef> &pending,
		std::size_t insertionIndex, std::size_t insertedPendingIndex);
	void adjustPendingBounds(std::deque<PendingRef> &pending,
		std::size_t insertionIndex, std::size_t insertedPendingIndex);
	void validateMatchingResults(const std::vector<int> &vectorResult,
		const std::deque<int> &dequeResult) const;
	void printProcessingTime(const std::string &containerName,
		std::size_t count, double microseconds) const;
};

#endif
