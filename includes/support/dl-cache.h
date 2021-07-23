
#ifndef	__DL_CACHE__
#define	__DL_CACHE__

#include	<QString>
#include	<vector>
class	dlCache {
private:
std::vector<QString> cache;
int	p;
int	size;
public:
	dlCache	(int size) {
	   cache. resize (size);
	   this	-> size	= size;
	   p = 0;
	}

	dlCache	() {
	}

void	add		(const QString &s) {
	cache [p] = s;
	p = (p + 1) % size;
}

bool	isMember	(const QString &s) {
	for (int i = 0; i < size; i ++)
	   if (cache [i] == s)
	      return true;
	return false;
}
};

#endif
