#ifndef _QUERY_INFO_H_INCLUDED_
#define _QUERY_INFO_H_INCLUDED_

#include <stdint.h>
#include <iostream>
#include <map>

#include <set>
#include <vector>

#include "wmc_trace.h"

namespace wmc {

typedef uint32_t    RegId;
typedef std::map<RegId, int>     Regions;

struct QueryInfo;

//typedef std::vector<QueryStat> KeyStat;
//typedef std::map<RegId, QueryStat> RegStat;
    
struct QueryInfo {
    QueryInfo()
            : region(0)
            , showsCount(0), showsPosWeight(0)
            , clicksCount(0), clickPosWeight(0) 
            {}

    std::string query;
    int region;
    int showsCount;
    int showsPosWeight;
    int clicksCount;
    int clickPosWeight;
};


/*!
 * Accumulator to collect total or regional statistics
 */
struct QueryStat {
    int shows;         //!> the sum of all show counters belonging to the children of the current node
    int clicks;
    QueryStat() : shows(0), clicks(0) {}
    QueryStat& operator+=(QueryInfo const& info) { shows += info.showsCount; clicks += info.clicksCount; return *this; }
};

struct RegQueryStat : QueryStat {
    RegId region;
    RegQueryStat() : region(0) {}
};

/*
struct QueryStat1 {
    int showsCount;         //!> the sum of all show counters belonging to the children of the current node
    int showsPosWeight;     //!> = SUM(nShows * nPosition)
    int clickCount;
    int clickPosWeight;
    int numRecords;
    QueryStat1() : showsCount(0), showsPosWeight(0), clickCount(0), clickPosWeight(0), numRecords(0) {}
    QueryStat1(QueryInfo const& info);
    void add(QueryInfo const& info);
};
*/
struct QueryStat1 : QueryStat {
    int showsPosWeight;         //!> the sum of all show counters belonging to the children of the current node
    int clickPosWeight;
    int numRecords;
    QueryStat1() : showsPosWeight(0), clickPosWeight(0), numRecords(0) {}
    QueryStat1& operator+=(QueryInfo const& info) { 
    	QueryStat::operator+=(info);
        showsPosWeight += info.showsPosWeight; 
        clickPosWeight += info.clickPosWeight; 
        numRecords += 1;
    	return *this; 
    }
};


struct TopQueryInfo : public QueryInfo {
    TopQueryInfo()
            : QueryInfo(),
            inTopShows(false),
            inTopClicks(false) {}

    bool inTopShows;
    bool inTopClicks;
};

} // namespace wmc

std::ostream& operator<< (std::ostream& out, const wmc::QueryInfo& info);


#endif
