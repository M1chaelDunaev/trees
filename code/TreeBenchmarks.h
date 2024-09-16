#ifndef TREE_BENCHMARKS_H
#define TREE_BENCHMARKS_H

#include <chrono>
#include <random>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include "TreeMap.h"

#ifndef NOW_VEC_AND_DURCAST
#define NOW_VEC_AND_DURCAST

auto now = std::chrono::steady_clock::now;
#define DurCast std::chrono::duration_cast<std::chrono::milliseconds>

std::vector<std::pair<size_t, size_t>> getVec()
{
	std::vector<std::pair<size_t, size_t>> v;
	v.reserve(10000000);

	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<size_t> distrib(0, 200000000); // from 0 to 200m

	for (size_t i = 0; i < 10000000; ++i)
	{
		v.push_back(std::make_pair(distrib(mt), i));
	}

	return v;
}

std::vector<size_t> getRandIndexVec()
{
	std::vector<size_t> v;
	v.reserve(10000000);

	for (size_t i = 0; i < 10000000; ++i)
	{
		v.push_back(i);
	}

	std::random_device rd;
	std::mt19937 mt(rd());
	std::shuffle(v.begin(), v.end(), mt);

	return v;
}

#endif // !NOW_VEC_AND_DURCAST

void tree_insertionBenchmark()
{
	auto v = getVec();

	TreeMap<size_t, size_t> m1;
	std::map<size_t, size_t> m2;

	auto b1 = now();
	for (auto& elem : v)
	{
		m1.insert(elem);
	}
	auto e1 = now();

	auto b2 = now();
	for (auto& elem : v)
	{
		m2.insert(elem);
	}
	auto e2 = now();

	auto dur1 = DurCast(e1 - b1);
	auto dur2 = DurCast(e2 - b2);

	std::cout << "Insertion of 10000000 std::pair<size_t, size_t> pairs:\n";
	std::cout << "TreeMap time: " << dur1.count() << "ms\n";
	std::cout << "std::map time: " << dur2.count() << "ms\n\n";
}

void tree_erasingBenchmark()
{
	auto v = getVec();

	TreeMap<size_t, size_t> m1;
	std::map<size_t, size_t> m2;

	for (auto& elem : v)
	{
		m1.insert(elem);
	}

	for (auto& elem : v)
	{
		m2.insert(elem);
	}

	std::vector<size_t> iv = getRandIndexVec();

	size_t resCount1 = 0;
	size_t resCount2 = 0;

	auto b1 = now();
	for (auto elem : iv)
	{
		resCount1 += m1.erase(v[elem].first);
	}
	auto e1 = now();

	auto b2 = now();
	for (auto elem : iv)
	{
		resCount2 += m2.erase(v[elem].first);
	}
	auto e2 = now();

	auto dur1 = DurCast(e1 - b1);
	auto dur2 = DurCast(e2 - b2);

	std::cout << "Erasing of 10000000 size_t elements:\n";
	std::cout << "TreeMap time: " << dur1.count() << "ms : erased " << resCount1 << std::endl;
	std::cout << "std::map time: " << dur2.count() << "ms : erased " << resCount2 << std::endl << std::endl;
}

void tree_findingBenchmark()
{
	auto v = getVec();

	TreeMap<size_t, size_t> m1;
	std::map<size_t, size_t> m2;

	for (auto& elem : v)
	{
		m1.insert(elem);
	}

	for (auto& elem : v)
	{
		m2.insert(elem);
	}

	std::vector<size_t> iv = getRandIndexVec();

	size_t resCount1 = 0;
	size_t resCount2 = 0;

	auto b1 = now();
	for (auto elem : iv)
	{
		if (!m1.find(v[elem].first).isEnd())
			++resCount1;
	}
	auto e1 = now();

	auto b2 = now();
	for (auto elem : iv)
	{
		if (m2.find(v[elem].first) != m2.end())
			++resCount2;

		m2.find(v[elem].first);
	}
	auto e2 = now();

	auto dur1 = DurCast(e1 - b1);
	auto dur2 = DurCast(e2 - b2);

	std::cout << "Finding of 10000000 size_t elements:\n";
	std::cout << "TreeMap time: " << dur1.count() << "ms : found " << resCount1 << std::endl;
	std::cout << "std::map time: " << dur2.count() << "ms : found " << resCount2 << std::endl << std::endl;
}

void tree_forwardIterationBenchmark()
{
	auto v = getVec();

	TreeMap<size_t, size_t> m1;
	std::map<size_t, size_t> m2;

	for (auto& elem : v)
	{
		m1.insert(elem);
	}

	for (auto& elem : v)
	{
		m2.insert(elem);
	}

	size_t sum1 = 0;
	size_t sum2 = 0;

	auto b1 = now();
	for (auto& elem : m1)
	{
		sum1 += elem.second;
	}
	auto e1 = now();

	auto b2 = now();
	for (auto& elem : m2)
	{
		sum2 += elem.second;
	}
	auto e2 = now();

	auto dur1 = DurCast(e1 - b1);
	auto dur2 = DurCast(e2 - b2);

	std::cout << "Forward iteration of " << m1.size() << " elements container:\n";
	std::cout << "TreeMap time: " << dur1.count() << "ms : total sum is " << sum1 << std::endl;
	std::cout << "std::map time: " << dur2.count() << "ms : total sum is " << sum2 << std::endl << std::endl;
}

void tree_reverseIterationBenchmark()
{
	auto v = getVec();

	TreeMap<size_t, size_t> m1;
	std::map<size_t, size_t> m2;

	for (auto& elem : v)
	{
		m1.insert(elem);
	}

	for (auto& elem : v)
	{
		m2.insert(elem);
	}

	size_t sum1 = 0;
	size_t sum2 = 0;

	auto b1 = now();
	for (auto it = m1.back(); !it.isBeforeBegin(); --it)
	{
		sum1 += it->second;
	}
	auto e1 = now();

	auto b2 = now();
	for (auto it = (--(m2.end())); it != m2.begin(); --it)
	{
		sum2 += it->second;
	}

	sum2 += m2.begin()->second;
	auto e2 = now();

	auto dur1 = DurCast(e1 - b1);
	auto dur2 = DurCast(e2 - b2);

	std::cout << "Reverse iteration of " << m1.size() << " elements container:\n";
	std::cout << "TreeMap time: " << dur1.count() << "ms : total sum is " << sum1 << std::endl;
	std::cout << "std::map time: " << dur2.count() << "ms : total sum is " << sum2 << std::endl << std::endl;
}

#endif // !TREE_BENCHMARKS_H
