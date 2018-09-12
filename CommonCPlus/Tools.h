#ifndef TOOL_H_
#define TOOL_H_
#include <vector>

bool	 CompValue(INT32 p1, INT32 p2)
{
	return p1 < p2;
}
//if the values is the same
bool	Comp2Vector(std::vector<INT32> & vs, std::vector<INT32> & vd)
{
	if (vs.size() < 1 || vd.size() < 1 || vs.size() != vd.size() )
	{
		return false;
	}
	std::sort(vs.begin(),vs.end(),CompValue);
	std::sort(vd.begin(),vd.end(),CompValue);

	auto itrs = vs.begin();
	auto itrd = vd.begin();
	while (itrs != vs.end() && itrd != vd.end() && *itrs == *itrd)
	{
		++itrs;
		++itrd;
	}
	if (itrs == vs.end())
	{
		return true;
	}
	return false;
}

 
#endif