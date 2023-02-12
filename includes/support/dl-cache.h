
#ifndef	__DL_CACHE_H
#define	__DL_CACHE_H

#include	<QString>
#include	<vector>
class	dlCache {
private:
   std::vector<QString> cache;
   int	p;
   int	size;
#define	CACHE_SIZE	16
#define CACHE_MASK	(CACHE_SIZE - 1)
public:
	dlCache	(int size) {
	   (void)size;
	   cache. resize (CACHE_SIZE);
	   this	-> size	= CACHE_SIZE;
	   p = 0;
	}

	dlCache	() {
	}

void	add		(const QString &s) {
	cache [p] = s;
	p = (p + 1) & CACHE_MASK;
}

bool	isMember	(const QString &s) {
	for (int i = 0; i < CACHE_SIZE; i ++)
	   if (cache [i] == s)
	      return true;
	return false;
}

bool	addifNew	(const QString &s) {
	for (uint16_t i = p; i < p + CACHE_SIZE; i ++) {
	   if (cache [i & CACHE_MASK] == s) {
	      for (uint16_t j = i; j < (p - 1) + CACHE_SIZE; j ++)
	         cache [j & CACHE_MASK] = cache [(j + 1) & CACHE_MASK];
	      cache [(p - 1 + CACHE_SIZE) & CACHE_MASK] = s;
	      return true;
	   }
	}
	cache [p] = s;
	p = (p + 1) & CACHE_MASK;
	return false;
}
};
#endif
