//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Copyright 2019, Intel Corporation
//
// Modified to test pmem::obj containers
//

#include "unittest.hpp"

#include <libpmemobj++/experimental/string.hpp>

namespace nvobj = pmem::obj;
namespace pmem_exp = pmem::obj::experimental;
using S = pmem_exp::string;

struct root {
	nvobj::persistent_ptr<S> s, s0, s_arr[45];
};

template <class S>
void
test(nvobj::pool<struct root> &pop, const S &s, typename S::size_type pos,
     typename S::size_type n, const S &expected)
{
	auto r = pop.root();

	nvobj::transaction::run(pop,
				[&] { r->s0 = nvobj::make_persistent<S>(s); });

	auto &s0 = *r->s0;

	const typename S::size_type old_size = s0.size();
	if (pos <= old_size) {
		s0.erase(pos, n);
		UT_ASSERT(s0[s0.size()] == typename S::value_type());
		UT_ASSERT(s0 == expected);
	} else {
		try {
			s0.erase(pos, n);
			UT_ASSERT(false);
		} catch (std::out_of_range &) {
			UT_ASSERT(pos > old_size);
			UT_ASSERT(s == s0);
		}
	}

	nvobj::transaction::run(pop,
				[&] { nvobj::delete_persistent<S>(r->s0); });
}

template <class S>
void
test(nvobj::pool<struct root> &pop, const S &s, typename S::size_type pos,
     const S &expected)
{
	auto r = pop.root();

	nvobj::transaction::run(pop,
				[&] { r->s0 = nvobj::make_persistent<S>(s); });

	auto &s0 = *r->s0;

	const typename S::size_type old_size = s0.size();
	if (pos <= old_size) {
		s0.erase(pos);
		UT_ASSERT(s0[s0.size()] == typename S::value_type());
		UT_ASSERT(s0 == expected);
	} else {
		try {
			s0.erase(pos);
			UT_ASSERT(false);
		} catch (std::out_of_range &) {
			UT_ASSERT(pos > old_size);
			UT_ASSERT(s == s0);
		}
	}

	nvobj::transaction::run(pop,
				[&] { nvobj::delete_persistent<S>(r->s0); });
}

template <class S>
void
test(nvobj::pool<struct root> &pop, const S &s1, const S &expected)
{
	auto r = pop.root();

	nvobj::transaction::run(pop,
				[&] { r->s = nvobj::make_persistent<S>(s1); });

	auto &s = *r->s;

	s.erase();
	UT_ASSERT(s[s.size()] == typename S::value_type());
	UT_ASSERT(s == expected);

	nvobj::transaction::run(pop,
				[&] { nvobj::delete_persistent<S>(r->s); });
}

int
main(int argc, char *argv[])
{
	START();

	if (argc < 2) {
		std::cerr << "usage: " << argv[0] << " file-name" << std::endl;
		return 1;
	}

	auto path = argv[1];
	auto pop = nvobj::pool<root>::create(
		path, "string_test", PMEMOBJ_MIN_POOL, S_IWUSR | S_IRUSR);

	auto r = pop.root();
	{
		auto &s_arr = r->s_arr;
		try {
			nvobj::transaction::run(pop, [&] {
				s_arr[0] = nvobj::make_persistent<S>("");
				s_arr[1] = nvobj::make_persistent<S>("a");
				s_arr[2] = nvobj::make_persistent<S>("ab");
				s_arr[3] = nvobj::make_persistent<S>("abcd");
				s_arr[4] = nvobj::make_persistent<S>("abcde");
				s_arr[5] =
					nvobj::make_persistent<S>("abcdefghi");
				s_arr[6] =
					nvobj::make_persistent<S>("abcdefghij");
				s_arr[7] = nvobj::make_persistent<S>(
					"abcdefghijklmnopqrs");
				s_arr[8] = nvobj::make_persistent<S>(
					"abcdefghijklmnopqrst");
				s_arr[9] = nvobj::make_persistent<S>(
					"abcdefghijlmnopqrst");
				s_arr[10] = nvobj::make_persistent<S>(
					"abcdefghijpqrst");
				s_arr[11] = nvobj::make_persistent<S>(
					"abcdefghijt");
				s_arr[12] =
					nvobj::make_persistent<S>("abcdeghij");
				s_arr[13] =
					nvobj::make_persistent<S>("abcdehij");
				s_arr[14] = nvobj::make_persistent<S>("abcdej");
				s_arr[15] = nvobj::make_persistent<S>("abde");
				s_arr[16] = nvobj::make_persistent<S>("abe");
				s_arr[17] = nvobj::make_persistent<S>("acde");
				s_arr[18] =
					nvobj::make_persistent<S>("acdefghij");
				s_arr[19] = nvobj::make_persistent<S>(
					"acdefghijklmnopqrst");
				s_arr[20] = nvobj::make_persistent<S>("ade");
				s_arr[21] = nvobj::make_persistent<S>("ae");
				s_arr[22] = nvobj::make_persistent<S>("afghij");
				s_arr[23] = nvobj::make_persistent<S>("aj");
				s_arr[24] = nvobj::make_persistent<S>(
					"aklmnopqrst");
				s_arr[25] = nvobj::make_persistent<S>("at");
				s_arr[26] = nvobj::make_persistent<S>("bcde");
				s_arr[27] =
					nvobj::make_persistent<S>("bcdefghij");
				s_arr[28] = nvobj::make_persistent<S>(
					"bcdefghijklmnopqrst");
				s_arr[29] = nvobj::make_persistent<S>(
					"can't happen");
				s_arr[30] = nvobj::make_persistent<S>("cde");
				s_arr[31] = nvobj::make_persistent<S>("e");
				s_arr[32] = nvobj::make_persistent<S>("fghij");
				s_arr[33] = nvobj::make_persistent<S>("j");
				s_arr[34] =
					nvobj::make_persistent<S>("klmnopqrst");
				s_arr[35] = nvobj::make_persistent<S>("t");
				s_arr[36] = nvobj::make_persistent<S>(
					"0123456789012345678901234567890123456789012345678901234567890123456789");
				s_arr[37] = nvobj::make_persistent<S>(
					"123456789012345678901234567890123456789012345678901234567890123456789");
				s_arr[38] = nvobj::make_persistent<S>("9");
				s_arr[39] = nvobj::make_persistent<S>(
					"023456789012345678901234567890123456789012345678901234567890123456789");
				s_arr[40] = nvobj::make_persistent<S>("0");
				s_arr[41] = nvobj::make_persistent<S>("09");
				s_arr[42] = nvobj::make_persistent<S>(
					"0012345678901234567890123456789012345678901234567890123456789");
				s_arr[43] = nvobj::make_persistent<S>(
					"0123456789012345678901234567890123456789");
				s_arr[44] = nvobj::make_persistent<S>(
					"012345678901234567890123456789012345678901234567890123456789012345678");
			});

			test(pop, *s_arr[0], 0, 0, *s_arr[0]);
			test(pop, *s_arr[0], 0, 1, *s_arr[0]);
			test(pop, *s_arr[0], 1, 0, *s_arr[29]);
			test(pop, *s_arr[4], 0, 0, *s_arr[4]);
			test(pop, *s_arr[4], 0, 1, *s_arr[26]);
			test(pop, *s_arr[4], 0, 2, *s_arr[30]);
			test(pop, *s_arr[4], 0, 4, *s_arr[31]);
			test(pop, *s_arr[4], 0, 5, *s_arr[0]);
			test(pop, *s_arr[4], 0, 6, *s_arr[0]);
			test(pop, *s_arr[4], 1, 0, *s_arr[4]);
			test(pop, *s_arr[4], 1, 1, *s_arr[17]);
			test(pop, *s_arr[4], 1, 2, *s_arr[20]);
			test(pop, *s_arr[4], 1, 3, *s_arr[21]);
			test(pop, *s_arr[4], 1, 4, *s_arr[1]);
			test(pop, *s_arr[4], 1, 5, *s_arr[1]);
			test(pop, *s_arr[4], 2, 0, *s_arr[4]);
			test(pop, *s_arr[4], 2, 1, *s_arr[15]);
			test(pop, *s_arr[4], 2, 2, *s_arr[16]);
			test(pop, *s_arr[4], 2, 3, *s_arr[2]);
			test(pop, *s_arr[4], 2, 4, *s_arr[2]);
			test(pop, *s_arr[4], 4, 0, *s_arr[4]);
			test(pop, *s_arr[4], 4, 1, *s_arr[3]);
			test(pop, *s_arr[4], 4, 2, *s_arr[3]);
			test(pop, *s_arr[4], 5, 0, *s_arr[4]);
			test(pop, *s_arr[4], 5, 1, *s_arr[4]);
			test(pop, *s_arr[4], 6, 0, *s_arr[29]);
			test(pop, *s_arr[6], 0, 0, *s_arr[6]);
			test(pop, *s_arr[6], 0, 1, *s_arr[27]);
			test(pop, *s_arr[6], 0, 5, *s_arr[32]);
			test(pop, *s_arr[6], 0, 9, *s_arr[33]);
			test(pop, *s_arr[6], 0, 10, *s_arr[0]);
			test(pop, *s_arr[6], 0, 11, *s_arr[0]);
			test(pop, *s_arr[6], 1, 0, *s_arr[6]);
			test(pop, *s_arr[6], 1, 1, *s_arr[18]);
			test(pop, *s_arr[6], 1, 4, *s_arr[22]);
			test(pop, *s_arr[6], 1, 8, *s_arr[23]);
			test(pop, *s_arr[6], 1, 9, *s_arr[1]);
			test(pop, *s_arr[6], 1, 10, *s_arr[1]);
			test(pop, *s_arr[6], 5, 0, *s_arr[6]);
			test(pop, *s_arr[6], 5, 1, *s_arr[12]);
			test(pop, *s_arr[6], 5, 2, *s_arr[13]);
			test(pop, *s_arr[6], 5, 4, *s_arr[14]);
			test(pop, *s_arr[6], 5, 5, *s_arr[4]);
			test(pop, *s_arr[6], 5, 6, *s_arr[4]);
			test(pop, *s_arr[6], 9, 0, *s_arr[6]);
			test(pop, *s_arr[6], 9, 1, *s_arr[5]);
			test(pop, *s_arr[6], 9, 2, *s_arr[5]);
			test(pop, *s_arr[6], 10, 0, *s_arr[6]);
			test(pop, *s_arr[6], 10, 1, *s_arr[6]);
			test(pop, *s_arr[6], 11, 0, *s_arr[29]);
			test(pop, *s_arr[8], 0, 0, *s_arr[8]);
			test(pop, *s_arr[8], 0, 1, *s_arr[28]);
			test(pop, *s_arr[8], 0, 10, *s_arr[34]);
			test(pop, *s_arr[8], 0, 19, *s_arr[35]);
			test(pop, *s_arr[8], 0, 20, *s_arr[0]);
			test(pop, *s_arr[8], 0, 21, *s_arr[0]);
			test(pop, *s_arr[8], 1, 0, *s_arr[8]);
			test(pop, *s_arr[8], 1, 1, *s_arr[19]);
			test(pop, *s_arr[8], 1, 9, *s_arr[24]);
			test(pop, *s_arr[8], 1, 18, *s_arr[25]);
			test(pop, *s_arr[8], 1, 19, *s_arr[1]);
			test(pop, *s_arr[8], 1, 20, *s_arr[1]);
			test(pop, *s_arr[8], 10, 0, *s_arr[8]);
			test(pop, *s_arr[8], 10, 1, *s_arr[9]);
			test(pop, *s_arr[8], 10, 5, *s_arr[10]);
			test(pop, *s_arr[8], 10, 9, *s_arr[11]);
			test(pop, *s_arr[8], 10, 10, *s_arr[6]);
			test(pop, *s_arr[8], 10, 11, *s_arr[6]);
			test(pop, *s_arr[8], 19, 0, *s_arr[8]);
			test(pop, *s_arr[8], 19, 1, *s_arr[7]);
			test(pop, *s_arr[8], 19, 2, *s_arr[7]);
			test(pop, *s_arr[8], 20, 0, *s_arr[8]);
			test(pop, *s_arr[8], 20, 1, *s_arr[8]);
			test(pop, *s_arr[8], 21, 0, *s_arr[29]);
			test(pop, *s_arr[36], 0, 0, *s_arr[36]);
			test(pop, *s_arr[36], 0, 1, *s_arr[37]);
			test(pop, *s_arr[36], 0, 69, *s_arr[38]);
			test(pop, *s_arr[36], 0, 70, *s_arr[0]);
			test(pop, *s_arr[36], 0, 71, *s_arr[0]);
			test(pop, *s_arr[36], 1, 0, *s_arr[36]);
			test(pop, *s_arr[36], 1, 1, *s_arr[39]);
			test(pop, *s_arr[36], 1, 9, *s_arr[42]);
			test(pop, *s_arr[36], 1, 68, *s_arr[41]);
			test(pop, *s_arr[36], 1, 69, *s_arr[40]);
			test(pop, *s_arr[36], 1, 70, *s_arr[40]);
			test(pop, *s_arr[36], 10, 0, *s_arr[36]);
			test(pop, *s_arr[36], 40, 30, *s_arr[43]);
			test(pop, *s_arr[36], 40, 31, *s_arr[43]);
			test(pop, *s_arr[36], 69, 0, *s_arr[36]);
			test(pop, *s_arr[36], 69, 1, *s_arr[44]);
			test(pop, *s_arr[36], 69, 2, *s_arr[44]);
			test(pop, *s_arr[36], 70, 0, *s_arr[36]);
			test(pop, *s_arr[36], 70, 1, *s_arr[36]);
			test(pop, *s_arr[36], 71, 0, *s_arr[29]);

			test(pop, *s_arr[0], 0, *s_arr[0]);
			test(pop, *s_arr[0], 1, *s_arr[29]);
			test(pop, *s_arr[4], 0, *s_arr[0]);
			test(pop, *s_arr[4], 1, *s_arr[1]);
			test(pop, *s_arr[4], 2, *s_arr[2]);
			test(pop, *s_arr[4], 4, *s_arr[3]);
			test(pop, *s_arr[4], 5, *s_arr[4]);
			test(pop, *s_arr[4], 6, *s_arr[29]);
			test(pop, *s_arr[6], 0, *s_arr[0]);
			test(pop, *s_arr[6], 1, *s_arr[1]);
			test(pop, *s_arr[6], 5, *s_arr[4]);
			test(pop, *s_arr[6], 9, *s_arr[5]);
			test(pop, *s_arr[6], 10, *s_arr[6]);
			test(pop, *s_arr[6], 11, *s_arr[29]);
			test(pop, *s_arr[8], 0, *s_arr[0]);
			test(pop, *s_arr[8], 1, *s_arr[1]);
			test(pop, *s_arr[8], 10, *s_arr[6]);
			test(pop, *s_arr[8], 19, *s_arr[7]);
			test(pop, *s_arr[8], 20, *s_arr[8]);
			test(pop, *s_arr[8], 21, *s_arr[29]);
			test(pop, *s_arr[36], 0, *s_arr[0]);
			test(pop, *s_arr[36], 1, *s_arr[40]);
			test(pop, *s_arr[36], 69, *s_arr[44]);
			test(pop, *s_arr[36], 70, *s_arr[36]);
			test(pop, *s_arr[36], 71, *s_arr[29]);

			test(pop, *s_arr[0], *s_arr[0]);
			test(pop, *s_arr[4], *s_arr[0]);
			test(pop, *s_arr[18], *s_arr[0]);
			test(pop, *s_arr[8], *s_arr[0]);
			test(pop, *s_arr[36], *s_arr[0]);

			nvobj::transaction::run(pop, [&] {
				for (unsigned i = 0; i < 45; ++i) {
					nvobj::delete_persistent<S>(s_arr[i]);
				}
			});
		} catch (std::exception &e) {
			UT_FATALexc(e);
		}
	}

	pop.close();

	return 0;
}
