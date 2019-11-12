/* Copyright (c) 2009 the authors listed at the following URL, and/or
the authors of referenced articles or incorporated external code:
http://en.literateprograms.org/Merge_sort_(C_Plus_Plus)?action=history&offset=20090103220905

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Retrieved from: http://en.literateprograms.org/Merge_sort_(C_Plus_Plus)?oldid=15751
*/


// This implementation is originally based on a public domain D program by
// David Medlock (http://www.digitalmars.com/drn-bin/wwwnews?digitalmars.D/32113).


#include<vector>
#include<string>
#include<algorithm>
#include<iostream>



template<typename IT, typename VT> void insert(IT begin, IT end, const VT &v)
{
	while(begin+1!=end && *(begin+1)<v) {
		std::swap(*begin, *(begin+1));
		++begin;
	}
	*begin=v;
}



template<typename IT> void merge(IT begin, IT begin_right, IT end)
{
	for(;begin<begin_right; ++begin) {
		if(*begin>*begin_right) {
			typename std::iterator_traits<IT>::value_type v(*begin);
			*begin=*begin_right;
			insert(begin_right, end, v);
		}
	}
}


template<typename IT> void mergesort(IT begin, IT end)
{
	size_t size(end-begin);
	if(size<2) return;

	IT begin_right=begin+size/2;

	mergesort(begin, begin_right);
	mergesort(begin_right, end);
	merge(begin, begin_right, end);
}



template<typename T> void print(const T &data)
{
	std::cout<<" "<<data;
}


int main()
{
	std::vector<std::string> v(10);
	v[0]="Paris";
	v[1]="London";
	v[2]="Stockholm";
	v[3]="Berlin";
	v[4]="Oslo";
	v[5]="Rome";
	v[6]="Madrid";
	v[7]="Tallinn";
	v[8]="Amsterdam";
	v[9]="Dublin";

	std::cout<<"v before qsort: ";
	std::for_each(v.begin(), v.end(), print<std::string>);
	std::cout<<'\n';

	mergesort(v.begin(), v.end());

	std::cout<<"v after mergesort: ";
	std::for_each(v.begin(), v.end(), print<std::string>);
	std::cout<<'\n';

	int a[]={3,8,0,6,7,4,2,1,9,3,1,8,3,9,2,0,9};
	int *a_end=a+sizeof a/sizeof(int);
	

	std::cout<<"a before mergesort: ";
	std::for_each(a, a_end, print<int>);
	std::cout<<'\n';

	mergesort(a, a_end);

	std::cout<<"a after mergesort: ";
	std::for_each(a, a_end, print<int>);
	std::cout<<'\n';

	return 0;
}



