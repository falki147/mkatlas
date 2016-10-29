#pragma once

#include <algorithm>
#include <cassert>
#include <string>

struct Range {
	bool isInside(unsigned int value) {
		return value >= m_beg && value <= m_end;
	}

	bool isIntersecting(Range r) {
		return isInside(r.m_beg) || isInside(r.m_end) ||
			r.isInside(m_beg) || r.isInside(m_end);
	}

	Range join(Range r) {
		assert(isIntersecting(r) && "can't join ranges");

		return { std::min(m_beg, r.m_beg), std::max(m_end, r.m_end) };
	}

	static Range fromString(const std::string& str) {
		auto del = str.find('-');

		if (del != std::string::npos)
			return { std::stoul(str), std::stoul(str.substr(del + 1)) };
		
		auto num = std::stoul(str);
		return { num, num };
	}

	unsigned int m_beg;
	unsigned int m_end;
};
