/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa_r.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: coren <coren@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 12:39:19 by coren             #+#    #+#             */
/*   Updated: 2026/10/29 12:47:11 by coren            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/libft.h"

static int	intlen(int n)
{
	int	result;

	result = 0;
	if (n == 0)
		return (1);
	if (n < 0)
	{
		n = -n;
		result++;
	}
	while (n != 0)
	{
		n = n / 10;
		result++;
	}
	return (result);
}

int		ft_itoa_r(int n, char *dest, size_t dest_size)
{
	int		i;
	int		u;
	long	nbr;

	nbr = n;
	i = intlen(n) - 1;
	u = -1;

	if (dest == NULL || (int)dest_size <= i + 2)
		return 1;

	if (nbr < 0)
	{
		dest[0] = '-';
		nbr = -nbr;
		u = 0;
	}
	dest[i + 1] = '\0';
	while (i != u)
	{
		dest[i] = (nbr % 10) + 48;
		nbr = nbr / 10;
		i--;
	}
	return 0;
}
