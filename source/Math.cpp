#include <cstdlib>

#include "Math.hpp"

using std::atoi;
using std::string;

int saw(int a, int b)
{
	if( a >= 0 )
		return a % b;
	return b - ((b - (a + 1)) % b) - 1;
}
