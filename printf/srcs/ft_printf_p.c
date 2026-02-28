/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_p.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snaji <snaji@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/10 20:15:06 by snaji             #+#    #+#             */
/*   Updated: 2022/11/14 15:54:27 by snaji            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/utils.h"

static int	ft_printf_p_rec(size_t n, int fd, char *baseset)
{
	if (n)
		return (ft_printf_p_rec(n / 16, fd, baseset)
			+ ft_printf_c(baseset[n % 16], fd));
	return (0);
}

int	ft_printf_p(void *addr, int fd)
{
	if ((size_t)addr == 0)
		return (ft_printf_s("(nil)", fd));
	return (ft_printf_s("0x", fd)
		+ ft_printf_p_rec((size_t)addr, fd, "0123456789abcdef"));
}
