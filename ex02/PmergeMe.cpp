/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sruff <sruff@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 13:58:28 by sruff             #+#    #+#             */
/*   Updated: 2026/07/05 16:05:28 by sruff            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PmergeMe.hpp"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <iomanip>
#include <limits>

PmergeMe::PmergeMe()
{
}

PmergeMe::PmergeMe(const PmergeMe &other)
{
	(void)other;
}

PmergeMe &PmergeMe::operator=(const PmergeMe &other)
{
	(void)other;
	return (*this);
}

PmergeMe::~PmergeMe()
{
}

typedef std::chrono::high_resolution_clock Clock;

static double elapsedTime(const Clock::time_point &start,
	const Clock::time_point &end)
{
	return (std::chrono::duration<double, std::micro>(end - start).count());
}

static int parseInput(const std::string &argument)
{
	long long value = 0;

	if (argument.empty())
		throw std::runtime_error("empty argument");
	for (char c : argument)
	{
		if (!std::isdigit(static_cast<unsigned char>(c)))
			throw std::runtime_error("invalid digit");
		value = value * 10 + (c - '0');
		if (value > std::numeric_limits<int>::max())
			throw std::runtime_error("integer overflow");
	}
	if (value <= 0)
		throw std::runtime_error("not positive");
	return (static_cast<int>(value));
}

std::vector<int> PmergeMe::parseArguments(int argc, char **argv)
{
	std::vector<int> values;

	values.reserve(static_cast<std::size_t>(argc - 1));
	for (int i = 1; i < argc; ++i)
		values.push_back(parseInput(argv[i]));
	return (values);
}

void PmergeMe::printSequence(const std::string &label,
	const std::vector<int> &sequence)
{
	std::cout << label;
	for (std::size_t i = 0; i < sequence.size(); ++i)
	{
		if (i != 0)
			std::cout << ' ';
		std::cout << sequence[i];
	}
	std::cout << std::endl;
}

void PmergeMe::printProcessingTime(const std::string &containerName,
	std::size_t count, double microseconds) const
{
	std::cout << "Time to process a range of " << count
			  << " elements with std::" << containerName << " : "
			  << microseconds << " us" << std::endl;
}

void PmergeMe::BlockArena::reserveForInput(std::size_t inputSize)
{
	std::size_t	levels = 1;
	std::size_t	capacity;

	for (std::size_t count = inputSize; count > 1; count /= 2)
		++levels;
	if (inputSize != 0
		&& levels > std::numeric_limits<std::size_t>::max() / inputSize)
		capacity = std::numeric_limits<std::size_t>::max();
	else
		capacity = inputSize * levels;
	values.reserve(capacity);
	ids.reserve(capacity);
}

PmergeMe::BlockRef PmergeMe::BlockArena::makeSingle(int value, std::size_t id)
{
	const BlockRef	block = {values.size(), 1};

	values.push_back(value);
	ids.push_back(id);
	return (block);
}

PmergeMe::BlockRef PmergeMe::BlockArena::merge(const BlockRef &left,
	const BlockRef &right)
{
	const std::size_t	total = left.size + right.size;
	const BlockRef		block = {values.size(), total};

	values.reserve(values.size() + total);
	ids.reserve(ids.size() + total);
	for (std::size_t i = 0; i < left.size; ++i)
	{
		values.push_back(values[left.offset + i]);
		ids.push_back(ids[left.offset + i]);
	}
	for (std::size_t i = 0; i < right.size; ++i)
	{
		values.push_back(values[right.offset + i]);
		ids.push_back(ids[right.offset + i]);
	}
	return (block);
}

void PmergeMe::BlockArena::appendToVector(const BlockRef &block,
	std::vector<int> &output) const
{
	output.insert(output.end(), values.begin() + block.offset,
		values.begin() + block.offset + block.size);
}

void PmergeMe::BlockArena::appendToDeque(const BlockRef &block,
	std::deque<int> &output) const
{
	output.insert(output.end(), values.begin() + block.offset,
		values.begin() + block.offset + block.size);
}

std::vector<std::size_t> PmergeMe::buildJacobsthalInsertionOrder(
	std::size_t count)
{
	std::vector<std::size_t>	order;
	std::size_t					previous = 1;
	std::size_t					current = 3;
	const std::size_t			lastLabel = count + 1;

	order.reserve(count);
	while (current <= lastLabel)
	{
		for (std::size_t label = current; label > previous; --label)
			order.push_back(label - 2);
		const std::size_t next = current + 2 * previous;
		previous = current;
		current = next;
	}
	for (std::size_t label = lastLabel; label > previous; --label)
		order.push_back(label - 2);
	std::cout << "Jacobsthal insertion order:";
	for (std::size_t index : order)
		std::cout << ' ' << index;
	std::cout << std::endl;
	return (order);
}

std::vector<int> PmergeMe::sortVector(const std::vector<int> &input)
{
	BlockArena				arena;
	std::vector<BlockRef>	blocks;
	std::vector<int>		result;

	arena.reserveForInput(input.size());
	blocks.reserve(input.size());
	result.reserve(input.size());
	for (std::size_t i = 0; i < input.size(); ++i)
		blocks.push_back(arena.makeSingle(input[i], i));
	blocks = sortVectorBlocks(blocks, arena);
	for (const BlockRef &block : blocks)
		arena.appendToVector(block, result);
	return (result);
}

std::deque<int> PmergeMe::sortDeque(const std::vector<int> &input)
{
	BlockArena				arena;
	std::deque<BlockRef>		blocks;
	std::deque<int>			result;

	arena.reserveForInput(input.size());
	for (std::size_t i = 0; i < input.size(); ++i)
		blocks.push_back(arena.makeSingle(input[i], i));
	blocks = sortDequeBlocks(blocks, arena);
	for (const BlockRef &block : blocks)
		arena.appendToDeque(block, result);
	return (result);
}

std::vector<PmergeMe::BlockRef> PmergeMe::sortVectorBlocks(
	const std::vector<BlockRef> &blocks, BlockArena &arena)
{
	if (blocks.size() < 2)
		return (blocks);

	std::vector<BlockRef>	pairs;
	BlockRef				odd = {0, 0};
	bool					hasOdd = false;

	// first make sorted pairs: each merged block is [small side | large side]
	pairs.reserve(blocks.size() / 2);
	for (std::size_t i = 0; i + 1 < blocks.size(); i += 2)
	{
		const BlockRef	result = makeOrderedPair(blocks[i], blocks[i + 1], arena);

		pairs.push_back(result);
	}
	if (blocks.size() % 2 != 0)
	{
		odd = blocks.back();
		hasOdd = true;
	}

	// recursion sorts the large sides giving us the sorted backbone chain
	const std::vector<BlockRef>	sortedPairs = sortVectorBlocks(pairs, arena);
	std::vector<BlockRef>		mainChain;
	std::vector<PendingRef>		pending;
	std::size_t					pendingCapacity = blocks.size() / 2;

	mainChain.reserve(blocks.size());
	if (hasOdd)
		++pendingCapacity;
	pending.reserve(pendingCapacity);
	// split the sorted pairs: large blocks go into  main chain
	for (std::size_t i = 0; i < sortedPairs.size(); ++i)
	{
		const BlockRef		&pair = sortedPairs[i];
		const std::size_t	half = pair.size / 2;
		const BlockRef		small = {pair.offset, half};
		const BlockRef		large = {pair.offset + half, pair.size - half};

		if (i == 0)
			mainChain.push_back(small);
		else
			// this [small] block only needs to search before its paired large block.
			pending.push_back(PendingRef{small, mainChain.size(), true});
		mainChain.push_back(large);
	}
	if (hasOdd)
		// the odd block has no paired larg block meaning ... no upper bound.
		pending.push_back(PendingRef{odd, 0, false});

	const std::vector<std::size_t>	order =
		buildJacobsthalInsertionOrder(pending.size());

	for (std::size_t index : order)
		insertPendingBlock(mainChain, pending, index, arena);

	return (mainChain);
}

bool PmergeMe::blockLess(const BlockArena &arena, const BlockRef &left,
	const BlockRef &right) const
{
	const std::size_t	leftLast = left.offset + left.size - 1;
	const std::size_t	rightLast = right.offset + right.size - 1;

	if (arena.values[leftLast] != arena.values[rightLast])
		return (arena.values[leftLast] < arena.values[rightLast]);
	return (arena.ids[leftLast] < arena.ids[rightLast]);
}

PmergeMe::BlockRef PmergeMe::makeOrderedPair(const BlockRef &left,
	const BlockRef &right, BlockArena &arena)
{
	if (blockLess(arena, right, left))
		return (arena.merge(right, left));
	return (arena.merge(left, right));
}

void PmergeMe::insertPendingBlock(std::vector<BlockRef> &mainChain,
	std::vector<PendingRef> &pending, std::size_t pendingIndex,
	const BlockArena &arena)
{
	const PendingRef	item = pending[pendingIndex];
	std::size_t			searchEndIndex = mainChain.size();

	if (item.hasUpperBound)
		searchEndIndex = item.upperBoundIndex;
	// bounded pending blocks search only until before their paired large block.
	std::vector<BlockRef>::iterator	searchBegin = mainChain.begin();
	std::vector<BlockRef>::iterator	searchEnd = searchBegin + searchEndIndex;
	std::vector<BlockRef>::iterator	insertPos = std::upper_bound(
		searchBegin, searchEnd, item.block,
		// [&arena, this] lets this compare function use arena and blockLess().
		// upper_bound calls it to ask if left belongs before right.
		[&arena, this](const BlockRef &left, const BlockRef &right) {
			return (blockLess(arena, left, right));
		});
	const std::size_t insertionIndex =
		static_cast<std::size_t>(insertPos - mainChain.begin());

	mainChain.insert(insertPos, item.block);
	// inserting shifts later positions so saved upperbound indexes must shift too
	adjustPendingBounds(pending, insertionIndex, pendingIndex);
}

void PmergeMe::insertPendingBlock(std::deque<BlockRef> &mainChain,
	std::deque<PendingRef> &pending, std::size_t pendingIndex,
	const BlockArena &arena)
{
	const PendingRef	item = pending[pendingIndex];
	std::size_t			searchEndIndex = mainChain.size();

	if (item.hasUpperBound)
		searchEndIndex = item.upperBoundIndex;
	// bounded pending blocks search only until before their paired large block.
	std::deque<BlockRef>::iterator	searchBegin = mainChain.begin();
	std::deque<BlockRef>::iterator	searchEnd = searchBegin + searchEndIndex;
	std::deque<BlockRef>::iterator	insertPos = std::upper_bound(
		searchBegin, searchEnd, item.block,
		// [&arena, this] lets this tiny compare function use arena and blockLess().
		// upper_bound calls it to ask if left belongs before right.
		[&arena, this](const BlockRef &left, const BlockRef &right) {
			return (blockLess(arena, left, right));
		});
	const std::size_t insertionIndex =
		static_cast<std::size_t>(insertPos - mainChain.begin());

	mainChain.insert(insertPos, item.block);
	// inserting shifts later positions so saved upperbound indexes must shift too
	adjustPendingBounds(pending, insertionIndex, pendingIndex);
}

void PmergeMe::adjustPendingBounds(std::vector<PendingRef> &pending,
	std::size_t insertionIndex, std::size_t insertedPendingIndex)
{
	for (std::size_t i = 0; i < pending.size(); ++i)
	{
		if (i != insertedPendingIndex && pending[i].hasUpperBound
			&& pending[i].upperBoundIndex >= insertionIndex)
			++pending[i].upperBoundIndex;
	}
}

void PmergeMe::adjustPendingBounds(std::deque<PendingRef> &pending,
	std::size_t insertionIndex, std::size_t insertedPendingIndex)
{
	for (std::size_t i = 0; i < pending.size(); ++i)
	{
		if (i != insertedPendingIndex && pending[i].hasUpperBound
			&& pending[i].upperBoundIndex >= insertionIndex)
			++pending[i].upperBoundIndex;
	}
}

std::deque<PmergeMe::BlockRef> PmergeMe::sortDequeBlocks(
	const std::deque<BlockRef> &blocks, BlockArena &arena)
{
	if (blocks.size() < 2)
		return (blocks);

	std::deque<BlockRef>	pairs;
	BlockRef			odd = {0, 0};
	bool				hasOdd = false;

	// Phase 1: pair blocks as [smaller half | larger half]
	for (std::size_t i = 0; i + 1 < blocks.size(); i += 2)
		pairs.push_back(makeOrderedPair(blocks[i], blocks[i + 1], arena));
	if (blocks.size() % 2 != 0)
	{
		odd = blocks.back();
		hasOdd = true;
	}

	// Phase 2: recursively sort pairs by their larger half
	const std::deque<BlockRef>	sortedPairs = sortDequeBlocks(pairs, arena);
	std::deque<BlockRef>		mainChain;
	std::deque<PendingRef>	pending;

	// Phase 3: build the main chain and keep smaller blocks pending
	for (std::size_t i = 0; i < sortedPairs.size(); ++i)
	{
		const BlockRef		&pair = sortedPairs[i];
		const std::size_t	half = pair.size / 2;
		const BlockRef		small = {pair.offset, half};
		const BlockRef		large = {pair.offset + half, pair.size - half};

		if (i == 0)
			mainChain.push_back(small);
		else
			pending.push_back(PendingRef{small, mainChain.size(), true});
		mainChain.push_back(large);
	}
	if (hasOdd)
		pending.push_back(PendingRef{odd, 0, false});

	const std::vector<std::size_t>	order =
		buildJacobsthalInsertionOrder(pending.size());

	// Phase 4: insert pending blocks in jacobsthal order
	for (std::size_t index : order)
		insertPendingBlock(mainChain, pending, index, arena);
	return (mainChain);
}

void PmergeMe::validateMatchingResults(const std::vector<int> &vectorResult,
	const std::deque<int> &dequeResult) const
{
	if (vectorResult.size() != dequeResult.size()
		|| !std::equal(vectorResult.begin(), vectorResult.end(),
			dequeResult.begin()))
		throw std::runtime_error("container results differ");
}

void PmergeMe::run(int argc, char **argv)
{
	if (argc < 2)
		throw std::runtime_error("missing arguments");

	const std::vector<int> input = parseArguments(argc, argv);

	printSequence("Before: ", input);

	const Clock::time_point vectorStart = Clock::now();
	const std::vector<int> vectorResult = sortVector(input);
	const Clock::time_point vectorEnd = Clock::now();

	const Clock::time_point dequeStart = Clock::now();
	const std::deque<int> dequeResult = sortDeque(input);
	const Clock::time_point dequeEnd = Clock::now();

	validateMatchingResults(vectorResult, dequeResult);

	printSequence("After: ", vectorResult);
	std::cout << std::fixed << std::setprecision(5);
	printProcessingTime("vector", input.size(),
		elapsedTime(vectorStart, vectorEnd));
	printProcessingTime("deque", input.size(),
		elapsedTime(dequeStart, dequeEnd));
}
