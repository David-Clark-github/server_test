#include "ft_itoa_string.hpp"

static int		malloc_len(long nb)
{
	int	res;

	res = 1;
	if (nb == 0)
		return (2);
	if (nb < 0)
	{
		res++;
		nb *= -1;
	}
	while (nb)
	{
		nb /= 10;
		res++;
	}
	return (res);
}

void alloc_dest(char *dest, long nb, int len)
{
	if (nb == 0)
		dest[0] = '0';
	if (nb < 0)
	{
		nb *= -1;
		dest[0] = '-';
	}
	while (nb)
	{
		dest[len] = (nb % 10) + '0';
		nb /= 10;
		len--;
	}
}

void	ft_itoa_string(int n, std::string &s)
{
	long	nb;
	int		m_len;
	char	dest[16];

	memset(dest, 0, sizeof(dest));
	nb = (long)n;
	m_len = malloc_len(nb);
	//dest[m_len - 1] = '\0';
	alloc_dest(dest, nb, m_len - 2);
	s.append(dest);
}